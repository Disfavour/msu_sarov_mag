import numpy as np
import matplotlib.pyplot as plt
from os.path import dirname, join

dpi = 300
ltypes = ('-', '--', '-.', ':')
colors = ('b', 'g', 'r', 'c', 'm', 'y', 'k')
labels = ('A', 'B', 'C', 'D')
# norm_L2, norm_C, elapsed_time, T, Lx, Ly, N, Mx, My, omp_get_num_threads(), size


def plot_compare(p, t_p1, data, fname):
    maxp = len(p) * len(labels)

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for lt, d in zip(ltypes, data):
        for i, c in zip(range(len(labels)), colors):
            plt.plot(p, d[i:maxp:4, 2], lt+c)
    plt.xlabel(r'Количество вычислительных ядер')
    plt.ylabel(r'Время (секунды)')
    plt.xticks(p)
    plt.legend(labels)
    plt.grid()
    plt.savefig(fname)
    plt.semilogy()
    plt.savefig(f'{fname}_log')
    plt.close()

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for lt, d in zip(ltypes, data):
        for i, c, t1 in zip(range(len(labels)), colors, t_p1):
            plt.plot(p, t1 / d[i:maxp:4, 2], lt+c)
    plt.xlabel(r'Количество вычислительных ядер')
    plt.ylabel(r'Ускорение')
    plt.xticks(p)
    plt.legend(labels)
    plt.grid()
    plt.savefig(f'{fname}_S')
    plt.close()

    plt.figure(figsize=(6.4, 3.6), dpi=dpi, tight_layout=True)
    for lt, d in zip(ltypes, data):
        for i, c, t1 in zip(range(len(labels)), colors, t_p1):
            plt.plot(p, t1 / d[i:maxp:4, 2] / p, lt+c)
    plt.xlabel(r'Количество вычислительных ядер')
    plt.ylabel(r'Эффективность распараллеливания')
    plt.xticks(p)
    plt.legend(labels)
    plt.grid()
    plt.savefig(f'{fname}_E')
    plt.close()


def analytic(N, Mx, My, pnum):
    nx, ny = 4, 3
    return N * Mx * My * 381 / 2.3e9 / pnum + N * 4 * 8*(Mx//nx + My//ny) / 1.23e9


def load_data(fname, s, e):
    tmp = np.loadtxt(join(results_dir, f'{fname}_{s}.out'))
    for i in range(s+1, e+1):
        tmp = np.minimum(tmp, np.loadtxt(join(results_dir, f'{fname}_{i}.out')))
    return tmp


if __name__ == '__main__':
    base_dir = dirname(__file__)
    images_dir = join(base_dir, 'images')
    results_dir = join(base_dir, 'results')

    p_mpi = np.array(range(12, 217, 12))
    p_omp = np.array(range(12, 37, 12))

    t = load_data('p1', 614, 619)
    t_p1 = t[:4][:, 2]
    t_p1_O3 = t[4:][:, 2]

    omp = load_data('omp', 433, 442)
    omp_O3 = load_data('omp_O3', 443, 452)
    mpi_node = load_data('mpi_node', 453, 462)
    mpi_node_isend = load_data('mpi_node_isend', 463, 472)
    mpi_node_send = load_data('mpi_node_send', 473, 482)
    mpi_node_sendrecv = load_data('mpi_node_sendrecv', 483, 492)
    mpi_numa = load_data('mpi_numa', 493, 502)
    mpi_numa_isend = load_data('mpi_numa_isend', 503, 512)
    mpi_numa_send = load_data('mpi_numa_send', 513, 522)
    mpi_numa_sendrecv = load_data('mpi_numa_sendrecv', 523, 532)
    mpi_node_O3 = load_data('mpi_node_O3', 533, 542)
    mpi_node_isend_O3 = load_data('mpi_node_isend_O3', 543, 552)
    mpi_node_send_O3 = load_data('mpi_node_send_O3', 553, 562)
    mpi_node_sendrecv_O3 = load_data('mpi_node_sendrecv_O3', 563, 572)
    mpi_numa_O3 = load_data('mpi_numa_O3', 573, 582)
    mpi_numa_isend_O3 = load_data('mpi_numa_isend_O3', 583, 592)
    mpi_numa_send_O3 = load_data('mpi_numa_send_O3', 593, 602)
    mpi_numa_sendrecv_O3 = load_data('mpi_numa_sendrecv_O3', 603, 612)

    analytic_res = []
    analytic_res.append(analytic(500, 600, 400, p_mpi))
    analytic_res.append(analytic(1000, 1200, 800, p_mpi))
    analytic_res.append(analytic(2000, 2400, 1600, p_mpi))
    analytic_res.append(analytic(4000, 4800, 3200, p_mpi))

    analytical = np.empty(mpi_node.shape)
    for i, data in enumerate(analytic_res):
        analytical[i::4, 2] = data

    plot_compare(p_mpi, t_p1, (mpi_node, mpi_node_isend, mpi_node_send, mpi_node_sendrecv), join(images_dir, 'mpi_node'))
    plot_compare(p_mpi, t_p1, (mpi_numa, mpi_numa_isend, mpi_numa_send, mpi_numa_sendrecv), join(images_dir, 'mpi_numa'))

    plot_compare(p_mpi, t_p1, (mpi_node, mpi_node_isend, mpi_numa, mpi_numa_isend), join(images_dir, 'node_numa'))

    plot_compare(p_mpi, t_p1_O3, (mpi_node_O3, mpi_node_isend_O3, mpi_node_send_O3, mpi_node_sendrecv_O3), join(images_dir, 'mpi_node_O3'))
    plot_compare(p_mpi, t_p1_O3, (mpi_numa_O3, mpi_numa_isend_O3, mpi_numa_send_O3, mpi_numa_sendrecv_O3), join(images_dir, 'mpi_numa_O3'))

    plot_compare(p_omp, t_p1, (omp, mpi_node, mpi_numa), join(images_dir, 'omp'))

    plot_compare(p_mpi, t_p1, (analytical, mpi_node, mpi_numa), join(images_dir, 'analytical'))

    plot_compare(p_mpi, t_p1_O3, (analytical, mpi_node_O3, mpi_numa_O3), join(images_dir, 'analytical_O3'))
