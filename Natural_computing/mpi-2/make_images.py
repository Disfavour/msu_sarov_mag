import numpy as np
import matplotlib.pyplot as plt
from os.path import dirname, join

base_dir = dirname(__file__)
images = join(base_dir, 'images')
dpi = 300

def plot(x, ys, xlabel, ylabel, labels, fname):
    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    plt.plot(x, ys.T)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    #plt.xticks(x)
    if labels is not None:
        plt.legend(labels)
    plt.grid()
    plt.savefig(fname)
    plt.close()


for i in range(1, 4):
    data = np.loadtxt(join(base_dir, f'output{i}.dat'))
    plot(list(range(1000)), data, r"Эпоха", r"Ошибка", ["Средняя", "Лучшая"], join(images, str(i)))