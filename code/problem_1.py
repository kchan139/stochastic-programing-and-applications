import numpy as np

def main():
    n   = 8           # number of products
    S   = 2           # number of scenarios
    p_s = [0.5, 0.5]  # probability of each scenario / density
    m   = 5           # number of parts to be ordered

    D = np.array([binomial (10, 0.5, n),
                  binomial (10, 0.5, n)]) # demand for each product in each scenario

    b = randint (5, 10, m) # preorder cost for each part

    A = np.array (randint (1, 10, m),
                  randint (1, 10, m),
                  randint (1, 10, m),
                  randint (1, 10, m),
                  randint (1, 10, m),
                  randint (1, 10, m),
                  randint (1, 10, m),
                  randint (1, 10, m)) # 

    print (D)

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