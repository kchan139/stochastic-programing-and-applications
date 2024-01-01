import gurobipy as gp
from gurobipy import GRB
import random
import numpy as np

def binomial (n, p, size):
    return np.random.binomial(n, p, size)

def randint (low, high, size):
    return np.random.randint (low, high, size)

n   = 8           # number of products
m   = 5           # number of parts to be ordered
S   = 2           # number of scenarios
p_s = [0.5, 0.5]  # probability of each scenario / density

D = np.array([binomial (10, 0.5, n),
              binomial (10, 0.5, n)]) # demand for each product in each scenario matrix [n x S]

b = randint(5, 10, m) # preorder cost for each part

A = []
for i in range(n):
    A.append(np.array(randint(1,10,m)))

A = np.array(A) # bill of materials matrix [m x n]


s = randint (1, 10, m)       # salvage values
l = randint (100, 200, n)    # additional costs
q = randint (1000, 1400, n)  # unit selling prices

c = l - q #cost coefficients

model = gp.Model("stage1")
A = A.transpose()
D = D.transpose()
p = [0.5,0.5]
x = model.addMVar((m,),  vtype=GRB.INTEGER, name="x1")
y1 = model.addMVar((m,S),  vtype=GRB.INTEGER, name="y1")
z1 = model.addMVar((n,S),  vtype=GRB.INTEGER, name="z1")
model.addConstrs((
    y1[:,k] == x - A @ z1[:,k] for k in range(S)
))

model.addConstr(x >= 0)
model.addConstr(y1 >= 0)
model.addConstr(z1 >= 0)
model.setObjective(b @ x + gp.quicksum((c @ z1[:,k] - s @ y1[:,k])*0.5 for k in range(S)))


model.setParam('OutputFlag', 0)
model.addConstr(z1 <= D)
model.ModelSense =GRB.MINIMIZE
model.optimize()

# if optimizing successfully, we will have x




x_opt = x.X
print(x_opt)
D_KNOWN = np.array([6, 9, 8, 8, 4, 4, 6, 5])
model2 = gp.Model("stage2")
# model2.setParam('OutputFlag', 0)
# model2.params.logtoconsole = 0
y1 = model2.addMVar((m,),  vtype=GRB.INTEGER, name="y")
z1 = model2.addMVar((n,),  vtype=GRB.INTEGER, name="z")

model2.addConstr(y1 == x_opt - A @ z1)
model2.addConstr(y1 >= 0)
model2.addConstr(z1 >= 0)
model2.addConstr(z1 <= D_KNOWN)

model2.setObjective(c @ z1 - s @ y1)


model2.ModelSense = GRB.MINIMIZE
model2.optimize()
