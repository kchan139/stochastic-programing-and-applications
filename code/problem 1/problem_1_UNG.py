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
p_s = [0.5, 0.5]  # probability of each scenario / density  x

D = np.array([binomial (10, 0.5, n),
              binomial (10, 0.5, n)]) # demand for each product in each scenario matrix [n x S]

b = randint(50, 100, m) # preorder cost for each part

A = []
for i in range(n):
    A.append(np.array(randint(0,5,m)))

A = np.array(A) # bill of materials matrix [m x n]

s = []                       # salvage price (< preorder price)
for i in b:
    s.append(random.randint(30,i-1))
s = np.array(s)

l = randint (100, 200, n)    # additional costs
q = randint (1000, 2000, n)  # unit selling prices

c = l - q #cost coefficients

print(f'INTRODUCING THE VALUES AND COEFFICIENTS')
print('-------------------------------------------------------------------------------------')
print(f'n   = {n}')
print(f'm   = {m}')
print(f'S   = {S}')
print(f'p_s (probability for each scenario) = {p_s}')
print(f'D (please keep in mind that this is random) = {D}')
print(f'A = {A}')
print(f'b = {b}')
print(f's = {s}')
print(f'q = {q}')
print(f'l = {l}')
print('-------------------------------------------------------------------------------------')

model = gp.Model("stage1")
A = A.transpose()
D = D.transpose()
p = [0.5,0.5]
x = model.addMVar((m,),  vtype=GRB.INTEGER, name="x")
y1 = model.addMVar((m,S),  vtype=GRB.INTEGER, name="y1")
z1 = model.addMVar((n,S),  vtype=GRB.INTEGER, name="z1")

model.addConstrs((y1[:,k] == x - A @ z1[:,k] for k in range(S)))
model.addConstr(x >= 0)
model.addConstr(y1 >= 0)
model.addConstr(z1 >= 0)
model.addConstr(z1 <= D)
model.setObjective(b @ x + gp.quicksum((c @ z1[:,k] - s @ y1[:,k])*0.5 for k in range(S)))

model.setParam('OutputFlag', 0)
model.ModelSense = GRB.MINIMIZE
model.optimize()
x_opt = x.X

# if optimizing successfully, we will have x

D_KNOWN = np.array([6, 9, 8, 8, 4, 4, 6, 5]) # hard-coded demand
model2 = gp.Model("stage2")
model2.setParam('OutputFlag', 0)
model2.params.logtoconsole = 0
y2 = model2.addMVar((m,),  vtype=GRB.INTEGER, name="y2")
z2 = model2.addMVar((n,),  vtype=GRB.INTEGER, name="z2")

model2.addConstr(y2 == x_opt.transpose() - A @ z2)
model2.addConstr(y2 >= 0)
model2.addConstr(z2 >= 0)
model2.addConstr(z2 <= D_KNOWN)

model2.setObjective(c @ z2 - s @ y2)

model2.ModelSense = GRB.MINIMIZE
model2.optimize()
print('-------------')
print('OPTIMIZING...')
print('-------------')

status = model2.Status
if status == GRB.OPTIMAL:
    print("Optimization was successful and an optimal solution is available.")
elif status == GRB.INF_OR_UNBD:
    print("Model is infeasible or unbounded.")
elif status == GRB.INFEASIBLE:
    print("Model is infeasible.")
elif status == GRB.UNBOUNDED:
    print("Model is unbounded.")
else:
    print("Optimization ended with status:", status)
if status == GRB.OPTIMAL:
    y_opt = y2.X
    z_opt = z2.X
    print('-------------------------------------------------------------------------------------')
    print(f'The optimal vector x (the number of parts we have to buy): {x_opt}')
    print('-------------------------------------------------------------------------------------')
    print(f'The optimal vector z (the number of production for each product): {z_opt}')
    print('-------------------------------------------------------------------------------------')
    print(f'The optimal vector y (the number of parts left in inventory for each product): {y_opt}')