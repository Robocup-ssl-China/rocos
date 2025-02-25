"""
script : train multi-agent diffusion model, use for trajectory prediction
author : Mark Huang
"""
import sys
import collections
import numpy as np
from datetime import datetime
import torch
import torch.nn as nn
from diffusers import DDPMScheduler, UNet1DModel
from diffusers.training_utils import EMAModel
from diffusers.optimization import get_scheduler
from tqdm.auto import tqdm
from torch_geometric.nn import GraphUNet

from torchviz import make_dot

sys.path.append('../..')
from rocos.nn import ConditionalUnet1D
from rocos.log.data.loader import data_loader as ssl_data_loader
from rocos.env.ssl import RocosEnv
from tzcp.ssl.rocos.zss_vision_detection_pb2 import Vision_DetectionFrame
from tzcp.ssl.rocos.zss_debug_pb2 import Debug_Msgs, Debug_Msg
from tzcp.ssl.rocos.zss_geometry_pb2 import Point

obs_dim = 4
action_dim = 4
obs_norm = [4.5, 3.0, 4.5, 3.0]
action_norm = [4.5, 3.0, 4.5, 3.0]
obs_horizon = 8
pred_horizon = 12
num_diffusion_iters = 50
noise_scheduler = DDPMScheduler(
    num_train_timesteps=num_diffusion_iters,
    # the choise of beta schedule has big impact on performance
    # we found squared cosine works the best
    beta_schedule='squaredcos_cap_v2',
    # clip output to [-1,1] to improve stability
    clip_sample=True,
    # our network predicts noise (instead of denoised action)
    prediction_type='epsilon'
)

# create network object
noise_pred_net = ConditionalUnet1D(
    input_dim=action_dim,
    global_cond_dim=obs_dim*obs_horizon
)
# noise_pred_net = GraphUNet()
# Exponential Moving Average
# accelerates training and improves stability
# holds a copy of the model weights
ema = EMAModel(
    parameters=noise_pred_net.parameters(),
    power=0.75)
def parse_train_args():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--data_dir', type=str, default='__log__')
    parser.add_argument('--skip', type=int, default=5)
    parser.add_argument('--batch_size', type=int, default=128)
    parser.add_argument('--loader_num_workers', type=int, default=4)
    parser.add_argument('--delim', type=str, default=' ')
    parser.add_argument('--obs_len', type=int, default=obs_horizon)
    parser.add_argument('--pred_len', type=int, default=pred_horizon)
    return parser.parse_args()

def train():
    args = parse_train_args()
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

    dataset, dataloader = ssl_data_loader(args, args.data_dir)
    print("Obs shape : ", [d.shape for d in dataset[0][0]])
    print("Pred shape : ", [d.shape for d in dataset[0][1]])
    print("Size : ", len(dataset))
    # for obs, pred in iter(loader):
    #     for d in obs:
    #         print(d.shape)
    #     break

    # observation and action dimensions corrsponding to
    # the output of PushTEnv

    # example inputs
    noised_action = torch.randn((1, pred_horizon, action_dim))
    obs = torch.zeros((1, obs_horizon, obs_dim))
    diffusion_iter = torch.zeros((1,))

    # the noise prediction network
    # takes noisy action, diffusion iteration and observation as input
    # predicts the noise added to action
    noise = noise_pred_net(
        sample=noised_action,
        timestep=diffusion_iter,
        global_cond=obs.flatten(start_dim=1)
    )


    # illustration of removing noise
    # the actual noise removal is performed by NoiseScheduler
    # and is dependent on the diffusion noise schedule
    denoised_action = noised_action - noise

    # device transfer
    device = torch.device('cuda')
    _ = noise_pred_net.to(device)
    _ = ema.to(device)

    #@markdown ### **Training**
    #@markdown
    #@markdown Takes about an hour. If you don't want to wait, skip to the next cell
    #@markdown to load pre-trained weights

    num_epochs = 100

    # Standard ADAM optimizer
    # Note that EMA parametesr are not optimized
    optimizer = torch.optim.AdamW(
        params=noise_pred_net.parameters(),
        lr=1e-4, weight_decay=1e-6)

    # Cosine LR schedule with linear warmup
    lr_scheduler = get_scheduler(
        name='cosine',
        optimizer=optimizer,
        num_warmup_steps=500,
        num_training_steps=len(dataloader) * num_epochs
    )

    with tqdm(range(num_epochs), desc='Epoch') as tglobal:
        # epoch loop
        for epoch_idx in tglobal:
            epoch_loss = list()
            # batch loop
            with tqdm(dataloader, desc='Batch', leave=False) as tepoch:
                for nbatch in tepoch:
                    obs_seqs, pred_seqs = nbatch

                    # only use ball data, ignore others for now
                    # TODO need graph_net to handle robots&ball data together
                    obs_seqs = obs_seqs[0]
                    pred_seqs = pred_seqs[0]
                    nobs = obs_seqs.to(device)
                    naction = pred_seqs.to(device)

                    nobs = nobs / torch.tensor(obs_norm, device=device)
                    naction = naction / torch.tensor(action_norm, device=device)

                    B = obs_seqs.shape[0]

                    # observation as FiLM conditioning
                    # (B, obs_horizon, obs_dim)
                    obs_cond = nobs[:,:obs_horizon,:]
                    # (B, obs_horizon * obs_dim)
                    obs_cond = obs_cond.flatten(start_dim=1)

                    # sample noise to add to actions
                    noise = torch.randn(naction.shape, device=device)

                    # sample a diffusion iteration for each data point
                    timesteps = torch.randint(
                        0, noise_scheduler.config.num_train_timesteps,
                        (B,), device=device
                    ).long()

                    # add noise to the clean images according to the noise magnitude at each diffusion iteration
                    # (this is the forward diffusion process)
                    noisy_actions = noise_scheduler.add_noise(
                        naction, noise, timesteps)

                    # predict the noise residual
                    noise_pred = noise_pred_net(
                        noisy_actions, timesteps, global_cond=obs_cond)

                    # print(f"noisy_action : {noisy_actions.shape}")
                    # print(f"obs_cond : {obs_cond.shape}")

                    # L2 loss
                    loss = nn.functional.mse_loss(noise_pred, noise)

                    # optimize
                    loss.backward()
                    optimizer.step()
                    optimizer.zero_grad()
                    # step lr scheduler every batch
                    # this is different from standard pytorch behavior
                    lr_scheduler.step()

                    # update Exponential Moving Average of the model weights
                    ema.step(noise_pred_net.parameters())

                    # logging
                    loss_cpu = loss.item()
                    epoch_loss.append(loss_cpu)
                    tepoch.set_postfix(loss=loss_cpu)
            tglobal.set_postfix(loss=np.mean(epoch_loss))

    # save the model
    time = datetime.now().strftime("%Y%m%d-%H%M%S")
    torch.save(noise_pred_net.state_dict(), f"ssl_diffusion_{time}.pth")
    # torch.save(ema.state_dict(), f"ssl_diffusion_ema_{time}.pth")

import time
def parse_test_args():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--model', type=str)
    parser.add_argument('--obs_len', type=int, default=obs_horizon)
    parser.add_argument('--pred_len', type=int, default=pred_horizon)
    return parser.parse_args()
def test():
    args = parse_test_args()
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

    # load the model
    noise_pred_net = ConditionalUnet1D(
        input_dim=action_dim,
        global_cond_dim=obs_dim*obs_horizon
    )
    noise_pred_net.load_state_dict(torch.load(args.model))
    noise_pred_net.eval()
    noise_pred_net.to(device)

    class Test:
        def __init__(self):
            self.dtype = np.float32
            self.data = collections.deque(maxlen=obs_horizon)
            self.env = RocosEnv(callbacks={'vision': self.vision_callback})
            self._r = 0.001
        def vision_callback(self, env, recv_data):
            msg, ep = recv_data
            ballMsg = self.convertBallMsg(msg)
            self.data.append(ballMsg)
        def convertBallMsg(self, msg: Vision_DetectionFrame):
            b = msg.balls
            return np.array([b.x, b.y, b.vel_x, b.vel_y], dtype=self.dtype)*self._r
        def get_obs(self):
            obs = np.array(self.data, dtype=self.dtype) / np.array(obs_norm, dtype=self.dtype)
            return obs
        def debug_ball(self, origin_obs, action):
            action = action*np.array(obs_norm, dtype=self.dtype)/self._r
            origin_obs = origin_obs*np.array(obs_norm, dtype=self.dtype)/self._r
            msg = Debug_Msgs()
            msg_points = Debug_Msg()
            msg_points.type = Debug_Msg.Debug_Type.POINTS
            for i,data in enumerate(action):
                msg_text = Debug_Msg()
                msg_text.type = Debug_Msg.Debug_Type.TEXT
                msg_text.text.text = f"b-{i},v-{data[2]:.2f},{data[3]:.2f}"
                msg_text.text.size = 20
                msg_text.text.pos.x = data[0]
                msg_text.text.pos.y = data[1]
                msg_point = Point()
                msg_point.x = data[0]
                msg_point.y = data[1]
                msg_points.points.point.append(msg_point)
                msg.msgs.append(msg_text)
            msg.msgs.append(msg_points)
            
            msg_points = Debug_Msg()
            msg_points.type = Debug_Msg.Debug_Type.POINTS
            msg_points.color = Debug_Msg.Color.BLUE
            for i,data in enumerate(origin_obs):
                msg_text = Debug_Msg()
                msg_text.color = Debug_Msg.Color.BLUE
                msg_text.type = Debug_Msg.Debug_Type.TEXT
                msg_text.text.text = f"b-{i},v-{data[2]:.2f},{data[3]:.2f}"
                msg_text.text.size = 20
                msg_text.text.pos.x = data[0]
                msg_text.text.pos.y = data[1]
                msg_point = Point()
                msg_point.x = data[0]
                msg_point.y = data[1]
                msg_points.points.point.append(msg_point)
                msg.msgs.append(msg_text)
            msg.msgs.append(msg_points)
            self.env.send_debug(msg)
    testEnv = Test()
    B = 1
    DrawModel = False
    while True:
        time.sleep(0.1)
        if testEnv.data.__len__() < obs_horizon:
            continue
        # print(f"{testEnv.get_obs()}")
        with torch.no_grad():
            obs_np = testEnv.get_obs()
            obs = torch.from_numpy(obs_np).to(device)
            obs_cond = obs.unsqueeze(0).flatten(start_dim=1)

            noisy_action = torch.randn((B, pred_horizon, action_dim), device=device)
            action = noisy_action

            noise_scheduler.set_timesteps(num_diffusion_iters)
            # print(f"noisy_action : {noisy_action.shape}")
            # print(f"obs_cond : {obs_cond.shape}")
            for k in noise_scheduler.timesteps:
                noise_pred = noise_pred_net(
                    sample=action,
                    timestep=k,
                    global_cond=obs_cond
                )
                if not DrawModel:
                    DrawModel = True
                    make_dot(noise_pred, params=dict(noise_pred_net.named_parameters())).render("model", format="png")
                # inverse diffusion step (remove noise)
                action = noise_scheduler.step(
                    model_output=noise_pred,
                    timestep=k,
                    sample=action
                ).prev_sample
            action = action.detach().cpu().numpy()
            testEnv.debug_ball(obs_np, action[0])
            print(f"action : {action}")

if __name__ == '__main__':
    # train()
    test()
