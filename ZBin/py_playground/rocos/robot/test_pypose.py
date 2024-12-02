import torch
import pypose as pp

data = torch.randn(2, 3, requires_grad=True, device='cuda:0')
a = pp.LieTensor(data, ltype=pp.so3_type)
print('a:', a)
b = pp.so3(data)
print('b:', b)

x = pp.identity_SE3(2,1)
y = pp.randn_se3(2,2)
print('x.shape:', x.shape, '\nx.gshape:', x.lshape)
print(x.lview(2))
print(y)

print("-------------------")
res = (x * y.Exp()).Inv().Log()
print("res : ",res)

print("-------------------")
X = pp.randn_Sim3(6, dtype=torch.double)
a = pp.randn_sim3(6, dtype=torch.double)
b = X.AdjT(a)
print((X * b.Exp() - a.Exp() * X).abs().mean() < 1e-7)

X = pp.randn_SE3(8)
a = pp.randn_se3(8)
b = X.Adj(a)
print((b.Exp() * X - X * a.Exp()).abs().mean() < 1e-7)