import numpy as np
import matplotlib.pyplot as plt
from os.path import dirname, join

base_dir = dirname(__file__)
images = join(base_dir, 'images')
dpi = 300

data = np.loadtxt(join(base_dir, f'data_for_plots.dat'))
#print(data[(data[:, 0] == 2000) & (data[:, 1] == 1)][0, 2])

SE = [(data[(data[:, 0] == n) & (data[:, 1] == 1)][0, 2] / t,
      data[(data[:, 0] == n) & (data[:, 1] == 1)][0, 2] / t / p)
      for n, p, t in data]

data = np.concatenate((data, np.array(SE)), axis=1)

ps = [1, 2, 4, 8]
Ns = [p*1000 for p in ps]

labels_ps = [fr'$P={p}$' for p in ps]
labels_Ns = [fr'$N={N }$' for N  in Ns]

def plot(x, ys, xlabel, ylabel, labels, fname):
    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    plt.plot(x, ys.T)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.xticks(x)
    if labels is not None:
        plt.legend(labels)
    plt.grid()
    plt.savefig(fname)
    plt.close()


t = np.array([data[data[:, 1] == p][:, 2] for p in ps])
s = np.array([data[data[:, 1] == p][:, 3] for p in ps])
e = np.array([data[data[:, 1] == p][:, 4] for p in ps])
plot(Ns, t, r'$N$', r'$T$', labels_ps, join(images, "1_T"))
plot(Ns, s, r'$N$', r'$S$', labels_ps, join(images, "1_S"))
plot(Ns, e, r'$N$', r'$E$', labels_ps, join(images, "1_E"))

t = np.array([data[data[:, 0] == N][:, 2] for N in Ns])
s = np.array([data[data[:, 0] == N][:, 3] for N in Ns])
e = np.array([data[data[:, 0] == N][:, 4] for N in Ns])
plot(ps, t, r'$P$', r'$T$', labels_Ns, join(images, "2_T"))
plot(ps, s, r'$P$', r'$S$', labels_Ns, join(images, "2_S"))
plot(ps, e, r'$P$', r'$E$', labels_Ns, join(images, "2_E"))

t = np.array([data[(data[:, 0] == N) & (data[:, 1] == N//1000)][0, 2] for N in Ns])
s = np.array([data[(data[:, 0] == N) & (data[:, 1] == N//1000)][0, 3] for N in Ns])
e = np.array([data[(data[:, 0] == N) & (data[:, 1] == N//1000)][0, 4] for N in Ns])
plot(ps, t, r'$P$', r'$T$', None, join(images, "3_T"))
plot(ps, s, r'$P$', r'$S$', None, join(images, "3_S"))
plot(ps, e, r'$P$', r'$E$', None, join(images, "3_E"))
