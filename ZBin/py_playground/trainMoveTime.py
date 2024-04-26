from rocos.dataset import MoveTimeDataset
from torch.utils.data import DataLoader
import torch
from torch import nn
from torch.utils.tensorboard import SummaryWriter
writer = SummaryWriter()
import matplotlib.pyplot as plt
device = (
    "cuda"
    if torch.cuda.is_available()
    else "mps"
    if torch.backends.mps.is_available()
    else "cpu"
)
print(f"Using {device} device")
class MyModel(nn.Module):
    def __init__(self, n_feature, n_hidden, n_output, p=0.1):
        super().__init__()
        self.linear_relu_stack = nn.Sequential(
            nn.Linear(n_feature, n_hidden),
            nn.LeakyReLU(),
            nn.Dropout(p=p),
            nn.Linear(n_hidden, n_hidden),
            nn.LeakyReLU(),
            nn.Dropout(p=p),
            nn.Linear(n_hidden, n_output),
        )
    
    def forward(self, x):
        x = self.linear_relu_stack(x)
        return x

EPOCH = 10000

if __name__ == '__main__':
    from datetime import datetime
    SYMBOL = str(datetime.now())
    train_dataset = MoveTimeDataset('../__data/robot_run/train')
    test_dataset = MoveTimeDataset('../__data/robot_run/test', skipFrame=30)
    print("train dataset length : ", len(train_dataset))
    print("test dataset length : ", len(test_dataset))

    train_dataloader = DataLoader(train_dataset,batch_size=32, shuffle=True)
    test_dataloader = DataLoader(test_dataset,batch_size=32, shuffle=True)

    model = MyModel(3, 32, 1).to(device)
    print(model)
    optimizer = torch.optim.Adam(model.parameters(), lr=1e-3)
    loss_func = torch.nn.MSELoss()

    losses = []
    for epoch in range(EPOCH):
        sum_loss = 0.0
        for i, (x,y) in enumerate(train_dataloader):
            x, y = x.to(device), y.to(device)
            output = model(x)
            loss = loss_func(output, y)
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            sum_loss += loss.data.cpu().numpy()
        sum_loss = sum_loss / len(train_dataset)
        writer.add_scalar("Loss/train", sum_loss, epoch)
        if epoch % 20 == 0:
            print(f"Train Loss : {sum_loss}")
            with torch.no_grad():
                sum_loss = 0.0
                for i, (x,y) in enumerate(test_dataloader):
                    x, y = x.to(device), y.to(device)
                    output = model(x)
                    loss = loss_func(output, y)
                    sum_loss += loss.data.cpu().numpy()
                sum_loss = sum_loss / len(test_dataset)
                writer.add_scalar("Loss/test", sum_loss, epoch)
            print(f"Test Loss : {sum_loss}")
        if epoch % 100 == 0:
            torch.save(model.state_dict(), "model/" + f'{SYMBOL}_model_{epoch//100}.pth')
    plt.plot(losses)
    plt.show()