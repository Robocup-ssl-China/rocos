import sys, signal
signal.signal(signal.SIGINT, signal.SIG_DFL)
import itertools as it
import numpy as np
import torch
from PySide6.QtCore import Qt, QObject, Slot, Signal
from PySide6.QtWidgets import (
    QApplication,
    QMainWindow,
    QSplitter,
    QCheckBox,
)
import pyqtgraph as pg
from tbox.components.CustomTree import CustomTreeWidget, NodeExpand
from tbox.components.Params import (
    ParamBool,
    ParamNumber,
)
from trainMoveTime import MyModel

MODEL = MyModel(3,32,1,0.0)
MODEL.load_state_dict(torch.load("MoveTime_3.pth"))

# use for any class that has ParamBase as attribute
class InputExpand(NodeExpand):
    def __init__(self, name, param, callback):
        super().__init__()
        self.param = param
        self.checkbox = ParamBool(name, False, callback=lambda v : callback())
        self.value = ParamNumber("", 0, param[0],param[1],param[2], callback=lambda v: callback())
    def getWidgets(self, node, columns):
        return [self.checkbox.widget, self.value.widget]
    def getValue(self):
        if self.checkbox.value:
            step = int((self.param[1] - self.param[0]) // self.param[2])
            return np.linspace(self.param[0],self.param[1],step)
        return [self.value.value]

def convert2TreeData(config, callback):
    data = {}
    for k,v in config.items():
        data[k] = InputExpand(k,v,callback)
    return data

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("ModelViz")

        self.plt = pg.PlotWidget()
        self.plt.showGrid(x=True, y=True, alpha = 0.3)
        self.plt.addLegend()
        self.plt.setBackground('transparent')

        self.inputs = convert2TreeData({
            "Dist" : [0,1,0.002],
            # "VelX" : [-1,1,0.05],
            # "VelY" : [-1,1,0.05],
            "RawVX" : [-1,1,0.01],
            "RawVY" : [-1,1,0.01],
        }, self.updatePlot)

        self.tree = CustomTreeWidget(data=self.inputs,titles=["Property","Value"])

        self.splitter = QSplitter(Qt.Vertical)
        self.splitter.addWidget(self.plt)
        self.splitter.addWidget(self.tree)
        
        self.setCentralWidget(self.splitter)

    @torch.no_grad
    def updatePlot(self):
        expandAxis = 0
        modelInput = []
        for i,(_,input) in enumerate(self.inputs.items()):
            values = input.getValue()
            if len(values) > 1:
                expandAxis = i
            modelInput.append(values)
        all_input = list(it.product(*modelInput))
        all_input = torch.Tensor(all_input)
        output = MODEL(all_input)
        # print(all_input,output)
        self.plt.clear()
        # self.plt.getViewBox().enableAutoRange(True)
        self.plt.plot(modelInput[expandAxis],output.numpy().reshape(-1))


if __name__ == "__main__":
    import qdarktheme
    qdarktheme.enable_hi_dpi()
    app = QApplication(sys.argv)
    qdarktheme.setup_theme("auto")
    window = MainWindow()
    window.resize(1200,800)
    window.show()
    app.exec()