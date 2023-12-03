from pulp import LpMaximize, LpMinimize, LpProblem, LpVariable, lpSum
import numpy as np

def main():
    n   = 2           # number of products
    m   = 2           # number of parts to be ordered
    S   = 2           # number of scenarios
    p_s = [0.5, 0.5]  # probability / density of each scenario

    # Demand for each product in each scenario ( [n x S] matrix )
    D = np.zeros ((n, S))
    for i in range (n):
        D[i] = binomial (10, 0.5, S)

    # Bill of materials ( [m x n] matrix )
    A = np.zeros ((m, n))
    for i in range (m):
        A[i] = randint (1, 10, n)

    # Preorder cost for each part ( [1 x m] vector )
    b = randint (5, 10, m)

    # Salvage values ( [1 x m] vector )
    s = randint (1, 10, m)

    # Additional costs ( [1 x n] vector )
    l = randint (100, 200, n)

    # Unit selling prices ( [1 x n] vector )
    q = randint (1000, 1400, n) 

    print("D = \n", D)
    print("A = \n", A, '\n')
    print("b = ", b)
    print("s = ", s)
    print("l = ", l)
    print("q = ", q)

    # D = np.array([[76, 99, 59, 88, 74, 87, 61, 66],
    #               [70, 72, 83, 82, 62, 92, 53, 89]])

    # b = np.array([18, 12, 13, 17, 18])
    # A = np.array ([[11, 8, 9, 8, 9],
    #                [6, 5, 14, 14, 11],
    #                [7, 6, 9, 7, 14],
    #                [7, 9, 12, 11, 14],
    #                [11, 10, 10, 5, 7],
    #                [6, 8, 10, 6, 13],
    #                [13, 9, 5, 7, 11],
    #                [5, 10, 10, 7, 10]])
    
    # s = np.array([13, 0, 10, 10, 11])
    # l = np.array([221, 190, 175, 144, 135, 242, 249, 169])
    # q = np.array([1344, 642, 1123, 1282, 1359, 1457, 619, 1444])

    # Cost coefficients ( [1 x n] vector )
    c = l - q

    D = D.transpose()
    A = A.transpose()

    prob1 = LpProblem(name = "2_Stage_Problem", sense = LpMinimize)
    x1 = LpVariable.dicts("x", range(m), cat = 'Integer')
    y1 = LpVariable.dicts("y", ( (i, j) for i in range(m) for j in range(S) ), cat = "Integer")
    z1 = LpVariable.dicts("z", ( (i, j) for i in range(n) for j in range(S) ), cat = "Integer")

    for i in range(m):
        # x >= 0
        prob1 += x1[i] >= 0
        
        # y >= 0
        for j in range(S):
            prob1 += y1[(i, j)] >= 0


    # 0 <= z1 <= D^T
    for i in range(n):
        for j in range(S):
            prob1 += z1[(i, j)] >= 0
            prob1 += z1[(i, j)] <= D[i][j]

    # y = x - A^T * z
    for k in range(S):
        for i in range(m):
            prob1 += y1[(i, k)] == x1[i] - lpSum( A[i][j] * z1[(j, k)] for j in range(n) )

    # Objective function
    prob1 += lpSum ([b[i] * x1[i] for i in range(m)]) + lpSum([(lpSum([c[j] * z1[(j, k)] for j in range(n)]) - lpSum([s[i] * y1[(i, k)] for i in range(m)]) * p_s[k]) for k in range(S)])

    prob1.solve()

    value_x1 = [x1[i].varValue for i in range(m)]
    print("x1 = ", value_x1)






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

if __name__ == "__main__":
    main()