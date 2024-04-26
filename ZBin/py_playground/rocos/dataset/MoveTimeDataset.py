import os
import numpy as np
from rocos.utils.Geom import Vec

# get all file with walk
def get_all_files(path):
    all_files = []
    for root, dirs, files in os.walk(path):
        for file in files:
            all_files.append(os.path.join(root, file))
    return all_files

def parse_file(filename):
    datas = []
    with open(filename, 'r') as f:
        lines = f.readlines()
    for line in lines:
        data = line[:-2].split(';')
        data = [float(x) for x in data]
        datas.append(data)
    return datas


# 0   :  os.clock(),
# 1   :  v:X(),
# 2   :  v:Y(),
# 3   :  v:VelX(),
# 4   :  v:VelY(),
# 5   :  v:RawRotVel(),
# 6   :  v:RotVel(),
# 7   :  v:RawVel():x(),
# 8   :  v:RawVel():y(),
# 9   :  runX,
# 10  :  runY,

class State:
    FRAME_RATE = 62.5
    def __init__(self,data):
        self.time = data[0]
        self.pos = Vec(data[1], data[2])
        self.vel = Vec(data[3], data[4])
        self.rawVel = Vec(data[7], data[8])
        self.target = Vec(data[9], data[10])

def parse_data(data, skip = 10):
    def get_train_data(startState, endState):
        time = endState.time - startState.time
        target = endState.pos - startState.pos
        vel = startState.vel
        rawVel = startState.rawVel
        # rotate
        rotAngle = target.dir
        target = target.rotate(-rotAngle) / 3000
        vel = vel.rotate(-rotAngle) / 3000
        rawVel = rawVel.rotate(-rotAngle) / 3000
        # return [target[0], vel.x, vel.y, rawVel.x, rawVel.y], [time]
        # print("{}\t{}\t{}\t{}".format(target[0], rawVel.x, rawVel.y, time))
        return [target[0], rawVel.x, rawVel.y], [time/5.0]
    frame = len(data)
    endData = data[-1]
    endData[0] = (frame-1)/State.FRAME_RATE
    endState = State(endData)

    trainDatas = []
    res = []
    for i in range(0, frame-1, skip):
        data[i][0] = i/State.FRAME_RATE
        state = State(data[i])
        trainData, time = get_train_data(state, endState)
        trainDatas.append(trainData)
        res.append(time)
    return trainDatas, res

import torch
from torch.utils.data import Dataset
class MoveTimeDataset(Dataset):
    def __init__(self, path, skipFrame = 10):
        self.data = []
        self.output = []
        files = get_all_files(path)
        for file in files:
            all_data = parse_file(file)
            data, output = parse_data(all_data, skipFrame)
            self.data.extend(data)
            self.output.extend(output)
    def __len__(self):
        return len(self.data)
    def __getitem__(self, idx):
        input = torch.tensor(self.data[idx], dtype=torch.float32)
        output = torch.tensor(self.output[idx], dtype=torch.float32)
        return input, output
