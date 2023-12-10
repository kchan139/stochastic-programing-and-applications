import pandas as pd
import numpy as np
from gamspy import Container, Set, Parameter, Variable, Equation, Model, Sum, Sense

def binomial (n, p, size):
    """
    Generate random numbers following a binomial distribution.

    Parameters:
    - n:    Number of trials (number of times an event is repeated).
    - p:    Probability of success for each trial.
    - size: Number of times to repeat the binomial experiment.

    Returns:
    - An array of random numbers representing the number of successes in each experiment.
    """
    return np.random.binomial(n, p, size)

def randint (low, high, size):
    """
    Generate random integers from a discrete uniform distribution.

    Parameters:
    - low:  The minimum (inclusive) value of the range.
    - high: The maximum (exclusive) value of the range.
    - size: The number of random integers to generate.

    Returns:
    - An array of random integers within the specified range.
    """
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
# A = np.transpose(A)

s = randint (1, 10, m)       # salvage values
l = randint (100, 200, n)    # additional costs
q = randint (1000, 1400, n)  # unit selling prices

c = l - q #cost coefficients

print('Preorder cost: ', b)
print('Material usage:\n', A)
print('Salvage value: ', s)
print('Additional producing cost: ', l)
print('Selling prices: ', q)
print('Cost coefficient:', c)
# print('Demand:\n', D)

m = Container()

products = ['product1', 'product2', 'product3', 'product4', 'product5','product6','product7','product8']
# products = [0,1,2,3,4,5,6,7]
i = Set(
    container=m,
    name='products',
    description='products of the firm',
    records=products
)

parts = ['part1', 'part2', 'part3', 'part4', 'part5']
# parts = [0,1,2,3,4]
j = Set(
    container=m,
    name='parts',
    description='Parts to build the products',
    records=parts
)

B = Parameter(
    container=m,
    name='Preorder_price',
    domain=j,
    description='Price to preorder a part',
    records=b
)

a = Parameter (
    container=m,
    name='parts_usages',
    domain=[i, j],
    description='Numbers of parts needed for n products',
    records=A
)

sc = Set(
    container=m,
    name="scenario",
    description=" adf ad",
    records=["s1", "s2"]
)

p = Parameter(
    container=m,
    name="scenario_p",
    domain=sc
)
p[sc] = 0.5

d = Parameter(
    container=m,
    name='Demand',
    domain=[sc,i],
    description="Demand for products",
    records = D
)

sal = Parameter(
    container=m,
    name='Salvage_price',
    domain=j,
    records=s
)

Apc = Parameter(
    container=m,
    name='Addition_producing_cost',
    domain=i,
    records = l
)

Sp = Parameter(
    container=m,
    name='Selling_price',
    domain=i,
    records=q
)

x = Variable(
    container=m,
    name='x',
    domain=j,
    type="positive",
    description='Number of preordered parts'
)

z = Variable(
    container=m,
    name='z',
    domain=i,
    type="positive",
    description='Number of produced products'
)

y = Variable(
    container=m,
    name='y',
    domain=j,
    type="positive",
    description='Number of unused parts'
)

con_1_4 = Equation(
    container=m,
    name='Constraint_1_4',
    domain=j
)
con_1_4[j] = x[j] - Sum((i,j), a[i,j]*z[i])

con_1_5 = Equation(
    container=m,
    name='Constraint_1_5',
    domain=i,
    definition= z <= d
)

obj = Sum(j, B[j]*x[j]) + Sum(sc, p[sc]* (Sum(i, (Apc[i]-Sp[i])*z[i]) - Sum(j, sal[j]*y[j])))

production = Model(
    container=m,
    name='production',
    equations=m.getEquations(),
    problem='LP',
    sense=Sense.MIN,
    objective=obj
)

import sys
production.solve(output=sys.stdout)