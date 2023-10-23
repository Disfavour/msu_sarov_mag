from fenics import *
import matplotlib.pyplot as plt

mesh = UnitSquareMesh(8, 8)
plot(mesh)
plt.show()
