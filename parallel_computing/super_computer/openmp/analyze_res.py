import numpy as np
import matplotlib.pyplot as plt
from os.path import dirname, join

images = join(dirname(__file__), 'images')

a = np.loadtxt(f'slurms/slurm-29.out')
for i in range(30, 36):
    #a += np.loadtxt(f'slurms/slurm-{i}.out')
    a = np.minimum(a, np.loadtxt(f'slurms/slurm-{i}.out'))

#a /= 7
a_1 = a[a[:, 9] == 1]
idx = np.argsort(a_1[:, 2])
a_1 = a_1[idx]

xticks = range(a_1.shape[0])
xlabels = []
for N, Mx, My in a_1[:, 6:9]:
    xlabels.append(rf'{int(N):4d}, {int(Mx):4d}, {int(My):4d}')

plt.figure(figsize=(6.4, 5), dpi=600, tight_layout=True)
for p in [2 ** i for i in range(6)] + [36]:
    a_p = a[a[:, 9] == p]
    a_p = a_p[idx]
    plt.plot(a_p[:, 2], label=f'{p}')

plt.xticks(ticks=xticks, labels=xlabels, rotation='vertical')
legend = plt.legend()
legend.set_title("Количество нитей")
plt.grid()
plt.xlabel(r"Размеры сеток (N, Mx, My)")
plt.ylabel(r"Время (секунды)")
plt.savefig(join(images, f'omp_time.png'))
plt.semilogy()
plt.savefig(join(images, f'omp_time_log.png'))
plt.clf()

for p in [2 ** i for i in range(6)] + [36]:
    a_p = a[a[:, 9] == p]
    a_p = a_p[idx]
    plt.plot(a_1[:, 2] / a_p[:, 2], label=f'{p}')

plt.xticks(ticks=xticks, labels=xlabels, rotation='vertical')
legend = plt.legend()
legend.set_title("Количество нитей")
plt.grid()
plt.xlabel(r"Размеры сеток (N, Mx, My)")
plt.ylabel(r"Ускорение")
plt.savefig(join(images, f'omp_S.png'))
plt.clf()

for p in [2 ** i for i in range(6)] + [36]:
    a_p = a[a[:, 9] == p]
    a_p = a_p[idx]
    plt.plot(a_1[:, 2] / a_p[:, 2] / p, label=f'{p}')

plt.xticks(ticks=xticks, labels=xlabels, rotation='vertical')
legend = plt.legend()
legend.set_title("Количество нитей")
plt.grid()
plt.xlabel(r"Размеры сеток (N, Mx, My)")
plt.ylabel(r"Эффективность")
plt.savefig(join(images, f'omp_E.png'))
plt.clf()

#print(a_1)
a_1 = a[a[:, 9] == 1]
a_1[:, 2] = a_1[:, 3] / a_1[:, 6]
a_1[:, 3] = a_1[:, 4] / a_1[:, 7]
print(a_1[:, :4])