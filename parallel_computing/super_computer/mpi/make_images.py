import numpy as np
import matplotlib.pyplot as plt
from os.path import dirname, join

dpi = 300
c = ('b', 'g', 'r', 'c', 'm', 'y', 'k')
l = ('-', '--', ':', '-.') # 
lw = [0.9, 1.2, 1.5]

p1 = (34, 258, 2006, 16085)
labels = ('D', 'C', 'B', 'A')
# norm_L2, norm_C, elapsed_time, T, Lx, Ly, N, Mx, My, omp_get_num_threads(), size


def mpi_compare(mpi_per_node, mpi_per_node_v2, mpi_per_numa, mpi_per_numa_v2, fname):
    p = np.array(range(12, 217, 12))
    
    mpi_per_node = mpi_per_node[np.isin(mpi_per_node[:, 9], range(2, 37, 2))]
    mpi_per_node_v2 = mpi_per_node_v2[np.isin(mpi_per_node_v2[:, 9], range(2, 37, 2))]

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, mpi_per_node[i::4, 2], f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, mpi_per_node_v2[i::4, 2], f'{l[1]}{c[i]}')
        plt.plot(p, mpi_per_numa[i::4, 2], f'{l[2]}{c[i]}')
        plt.plot(p, mpi_per_numa_v2[i::4, 2], f'{l[3]}{c[i]}')
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Время (секунды)")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(fname)
    plt.semilogy()
    plt.savefig(f'{fname}_log')
    plt.close()

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, p1[i] / mpi_per_node[i::4, 2], f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, p1[i] / mpi_per_node_v2[i::4, 2], f'{l[1]}{c[i]}')
        plt.plot(p, p1[i] / mpi_per_numa[i::4, 2], f'{l[2]}{c[i]}')
        plt.plot(p, p1[i] / mpi_per_numa_v2[i::4, 2], f'{l[3]}{c[i]}')
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Ускорение")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(f'{fname}_S')
    plt.close()

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, p1[i] / mpi_per_node[i::4, 2] / p, f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, p1[i] / mpi_per_node_v2[i::4, 2] / p, f'{l[1]}{c[i]}')
        plt.plot(p, p1[i] / mpi_per_numa[i::4, 2] / p, f'{l[2]}{c[i]}')
        plt.plot(p, p1[i] / mpi_per_numa_v2[i::4, 2] / p, f'{l[3]}{c[i]}')
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Эффективность распараллеливания")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(f'{fname}_E')
    plt.close()


def omp_compare(omp, mpi_per_node, mpi_per_node_v2, mpi_per_numa, mpi_per_numa_v2, fname):
    p = np.array((12, 24, 36))

    mpi_per_node = mpi_per_node[np.isin(mpi_per_node[:, 9], (2, 4, 6))]
    mpi_per_node_v2 = mpi_per_node_v2[np.isin(mpi_per_node_v2[:, 9], (2, 4, 6))]
    mpi_per_numa = mpi_per_numa[np.isin(mpi_per_numa[:, 9], (1, 2, 3))]
    mpi_per_numa_v2 = mpi_per_numa_v2[np.isin(mpi_per_numa_v2[:, 9], (1, 2, 3))]

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, omp[i::4, 2], f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, mpi_per_node_v2[i::4, 2], f'{l[1]}{c[i]}')
        plt.plot(p, mpi_per_numa_v2[i::4, 2], f'{l[2]}{c[i]}')
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Время (секунды)")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(fname)
    plt.semilogy()
    plt.savefig(f'{fname}_log')
    plt.close()

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, p1[i] / omp[i::4, 2], f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, p1[i] / mpi_per_node_v2[i::4, 2], f'{l[1]}{c[i]}')
        plt.plot(p, p1[i] / mpi_per_numa_v2[i::4, 2], f'{l[2]}{c[i]}')
    
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Ускорение")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(f'{fname}_S')
    plt.close()

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, p1[i] / omp[i::4, 2] / p, f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, p1[i] / mpi_per_node_v2[i::4, 2] / p, f'{l[1]}{c[i]}')
        plt.plot(p, p1[i] / mpi_per_numa_v2[i::4, 2] / p, f'{l[2]}{c[i]}')
    
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Эффективность распараллеливания")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(f'{fname}_E')
    plt.close()


def analytic(N, Mx, My, pnum):
    return (N+1) * ((Mx+1) * (My+1)) * 381 / 2.3e9 / pnum


def analytic_compare(mpi_per_node, mpi_per_node_v2, mpi_per_numa, mpi_per_numa_v2, fname):
    p = np.arange(12, 217, 12)
    
    mpi_per_node = mpi_per_node[np.isin(mpi_per_node[:, 9], range(2, 37, 2))]
    mpi_per_node_v2 = mpi_per_node_v2[np.isin(mpi_per_node_v2[:, 9], range(2, 37, 2))]

    analytic_vec = np.vectorize(analytic)

    analytic_A = analytic_vec(4000, 4800, 3200, p)
    analytic_B = analytic_vec(2000, 2400, 1600, p)
    analytic_C = analytic_vec(1000, 1200, 800, p)
    analytic_D = analytic_vec(500, 600, 400, p)
    data = np.array((analytic_D, analytic_C, analytic_B, analytic_A, analytic_B))

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, data[i, :], f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, mpi_per_node_v2[i::4, 2], f'{l[1]}{c[i]}')
        plt.plot(p, mpi_per_numa_v2[i::4, 2], f'{l[2]}{c[i]}')
        pass
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Время (секунды)")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(fname)
    plt.close()

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, p1[i] / data[i, :], f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, p1[i] / mpi_per_node_v2[i::4, 2], f'{l[1]}{c[i]}')
        plt.plot(p, p1[i] / mpi_per_numa_v2[i::4, 2], f'{l[2]}{c[i]}')
        pass
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Ускорение")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(f'{fname}_S')
    plt.close()

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for i in range(3, -1, -1):
        plt.plot(p, p1[i] / data[i, :] / p, f'{l[0]}{c[i]}', label=labels[i])
        plt.plot(p, p1[i] / mpi_per_node_v2[i::4, 2] / p, f'{l[1]}{c[i]}')
        plt.plot(p, p1[i] / mpi_per_numa_v2[i::4, 2] / p, f'{l[2]}{c[i]}')
        pass
    plt.xlabel(r"Количество нитей")
    plt.ylabel(r"Эффективность распараллеливания")
    plt.xticks(p)
    plt.grid()
    plt.legend()
    plt.savefig(f'{fname}_E')
    plt.close()


if __name__ == '__main__':
    base_dir = dirname(__file__)
    images_dir = join(base_dir, 'images')
    results_dir = join(base_dir, 'results')

    omp = np.loadtxt(join(results_dir, 'omp_299.out'))
    for i in range(300, 309):
        omp = np.minimum(omp, np.loadtxt(join(results_dir, f'omp_{i}.out')))

    mpi_per_node = np.loadtxt(join(results_dir, 'mpi_per_node_309.out'))
    for i in range(310, 319):
        mpi_per_node = np.minimum(mpi_per_node, np.loadtxt(join(results_dir, f'mpi_per_node_{i}.out')))

    mpi_per_node_v2 = np.loadtxt(join(results_dir, 'mpi_per_node_v2_319.out'))
    for i in range(320, 329):
        mpi_per_node_v2 = np.minimum(mpi_per_node_v2, np.loadtxt(join(results_dir, f'mpi_per_node_v2_{i}.out')))

    mpi_per_numa = np.loadtxt(join(results_dir, 'mpi_per_numa_329.out'))
    for i in range(330, 339):
        mpi_per_numa = np.minimum(mpi_per_numa, np.loadtxt(join(results_dir, f'mpi_per_numa_{i}.out')))

    mpi_per_numa_v2 = np.loadtxt(join(results_dir, 'mpi_per_numa_v2_339.out'))
    for i in range(340, 349):
        mpi_per_numa_v2 = np.minimum(mpi_per_numa_v2, np.loadtxt(join(results_dir, f'mpi_per_numa_v2_{i}.out')))

    omp_compare(omp, mpi_per_node, mpi_per_node_v2, mpi_per_numa, mpi_per_numa_v2, join(images_dir, 'openmp_compare'))

    mpi_compare(mpi_per_node, mpi_per_node_v2, mpi_per_numa, mpi_per_numa_v2, join(images_dir, 'mpi_compare'))

    analytic_compare(mpi_per_node, mpi_per_node_v2, mpi_per_numa, mpi_per_numa_v2, join(images_dir, 'analytic_compare'))