with open('batches/example_OpenMP', 'r') as base_f:
    with open('batches/run_openmp', 'w') as f:
        f.write(base_f.read())

        T, Lx, Ly = 5, 6, 4

        for p in [2 ** i for i in range(6)] + [36]:
            for N in [T * 100 * 2 ** i for i in range(4)]:
                for Mx, My in zip([Lx * 100 * 2 ** i for i in range(4)], [Ly * 100 * 2 ** i for i in range(4)]):
                        f.write(f'OMP_NUM_THREADS={p} ./wave_eq_2d_openmp {T} {Lx} {Ly} {N} {Mx} {My}\n')
