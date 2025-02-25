import sys, os, copy
from torch.utils.data import Dataset
import numpy as np
from tzcp.ssl.vision.messages_robocup_ssl_wrapper_tracked_pb2 import TrackerWrapperPacket
from tzcp.ssl.vision.messages_robocup_ssl_detection_tracked_pb2 import TrackedFrame, TeamColor
sys.path.append('../../..')
from rocos.log.tools.logread import get_content, read_log, MSG_INDEX as I
from rocos.log.tools.logdefine import TYPE

LOG_MIN_LEN = 100

class TrackerVisionDataset(Dataset):
    def __init__(self, data_dir, obs_len = 8, pred_len = 12, skip = 5):
        super(TrackerVisionDataset, self).__init__()
        self.data_dir = data_dir
        self.obs_len = obs_len
        self.pred_len = pred_len
        self.skip = skip
        self.seq_len = self.obs_len + self.pred_len

        self.dtype = np.float32

        self.obs_data = []
        self.pred_data = []
        
        all_log_files = []
        # get log files
        for root, dirs, files in os.walk(self.data_dir):
            for file in files:
                all_log_files.append(os.path.join(root, file))
        

        for log_file in all_log_files:
            content = get_content(log_file)
            msgs, type = read_log(content)
            assert type == TYPE.SSL_VISION_TRACKER_2020, "Unknown message type"
            obs_seqs, pred_seqs = self.generate_seq(msgs, TrackerWrapperPacket)
            self.obs_data.extend(obs_seqs)
            self.pred_data.extend(pred_seqs)
        
        print(f"Data Dir : {data_dir}, Total search {len(all_log_files)} log files, found {len(self.obs_data)} sequences")

    def generate_seq(self, msgs, MsgType):
        if len(msgs) < LOG_MIN_LEN:
            return [], []
        
        obs_seqs = []
        pred_seqs = []

        data_seq = []
        for data in msgs:
            msg = MsgType()
            msg.ParseFromString(data[I.MSG])
            data = self.parse_single_msg(msg)
            if data is None:
                print("parse error in msg, maybe ball not detected or robot number not correct. skip")
                break
            data_seq.append(data)
        
        for i in range(0, len(data_seq) - self.seq_len, self.skip):
            single_seq = self.generate_single_seq(data_seq[i:i+self.seq_len])
            obs, pred = [d[:self.obs_len] for d in single_seq], [d[self.obs_len:] for d in single_seq]
            obs_seqs.append(obs)
            pred_seqs.append(pred)
        
        return obs_seqs, pred_seqs

    def parse_single_msg(self, msg: TrackerWrapperPacket):
        frame = msg.tracked_frame
        if len(frame.balls) == 0:
            return None
        ball = frame.balls[0]
        robot_blue = {}
        robot_yellow = {}

        for r in frame.robots:
            # robot_data
            rd = np.array([r.pos.x, r.pos.y, r.orientation, r.vel.x, r.vel.y, r.vel_angular],dtype=self.dtype)
            if r.robot_id.team_color == TeamColor.TEAM_COLOR_BLUE:
                robot_blue[r.robot_id.id] = rd
            elif r.robot_id.team_color == TeamColor.TEAM_COLOR_YELLOW:
                robot_yellow[r.robot_id.id] = rd
        return {
            "ball": np.array([ball.pos.x, ball.pos.y, ball.vel.x, ball.vel.y],dtype=self.dtype),
            "blue": robot_blue,
            "yellow": robot_yellow
        }
    
    def generate_single_seq(self, seqs):
        if len(seqs) == 0:
            return []
        blue_id = list(seqs[0]["blue"].keys())
        yellow_id = list(seqs[0]["yellow"].keys())
        checked_blue_id = copy.deepcopy(blue_id)
        checked_yellow_id = copy.deepcopy(yellow_id)
        for seq in seqs:
            checked_blue_id = list(set(checked_blue_id) & set(seq["blue"].keys()))
            checked_yellow_id = list(set(checked_yellow_id) & set(seq["yellow"].keys()))
        assert len(checked_blue_id) == len(blue_id) and len(checked_yellow_id) == len(yellow_id), "robot number not correct"
        
        _ball_seq = np.empty((0, seqs[0]["ball"].shape[0]), dtype=self.dtype)
        _blue_seq = np.empty((0, len(blue_id), seqs[0]["blue"][blue_id[0]].shape[0]), dtype=self.dtype)
        _yellow_seq = np.empty((0, len(yellow_id), seqs[0]["yellow"][yellow_id[0]].shape[0]), dtype=self.dtype)

        for seq in seqs:
            ball = seq["ball"]
            blue = np.array([seq["blue"][i] for i in blue_id])
            yellow = np.array([seq["yellow"][i] for i in yellow_id])
            _ball_seq = np.vstack((_ball_seq, ball))
            _blue_seq = np.vstack((_blue_seq, blue[None, :, :]))
            _yellow_seq = np.vstack((_yellow_seq, yellow[None, :, :]))
        
        return [_ball_seq, _blue_seq, _yellow_seq]

    def __len__(self):
        return len(self.obs_data)

    def __getitem__(self, idx):
        return [
            self.obs_data[idx],
            self.pred_data[idx]
        ]

if __name__ == "__main__":
    data_dir = sys.argv[1]
    dataset = TrackerVisionDataset(data_dir)