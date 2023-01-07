import matplotlib.pyplot as plt
import numpy as np

x = np.linspace(-3, 2, 1000)

y1 = 2 ** x + 1

y2 = x ** 5

y3 = (1 - x) / 3

plt.plot(x, y1)
plt.plot(x, y2)
plt.plot(x, y3)
plt.ylim(-3, 7)

plt.legend(["2 ^ x + 1", "x ^ 5", "(1 - x) / 3"])

plt.savefig("chart", dpi=100)
