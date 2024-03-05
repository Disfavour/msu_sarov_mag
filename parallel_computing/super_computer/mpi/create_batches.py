from os.path import dirname, join


base_dir = dirname(__file__)
batches_dir = join(base_dir, 'batches')

T, Lx, Ly = 5, 6, 4
params = tuple(tuple(map(lambda x: x * 100 * 2**i, (T, Lx, Ly))) for i in range(4))


def create_batch_omp(fname, prog):
    with open(join(batches_dir, fname), 'w') as f:
        f.write(f'''#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=36

#SBATCH --time=24:00:00

#SBATCH --output=results/{fname}_%j.out
#SBATCH --error=mpi_err.err

export OMP_PROC_BIND=close
export OMP_PLACES=cores

''')
        for threads in range(12, 37, 12):
            f.write(f'export OMP_NUM_THREADS={threads}\n')
            for N, Mx, My in params:
                f.write(f'mpirun --bind-to none ./{prog} {T} {Lx} {Ly} {N} {Mx} {My}\n')


def create_batch_mpi_per_node(fname, prog):
    with open(join(batches_dir, fname), 'w') as f:
        f.write(f'''#!/bin/bash
#SBATCH --nodes=6
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=36

#SBATCH --time=24:00:00

#SBATCH --output=results/{fname}_%j.out
#SBATCH --error=mpi_err.err

export OMP_PROC_BIND=close
export OMP_PLACES=cores

''')
        for threads in range(2, 37, 2):
            f.write(f'export OMP_NUM_THREADS={threads}\n')
            for N, Mx, My in params:
                f.write(f'mpirun --bind-to none ./{prog} {T} {Lx} {Ly} {N} {Mx} {My}\n')


def create_batch_mpi_per_numa(fname, prog):
    with open(join(batches_dir, fname), 'w') as f:
        f.write(f'''#!/bin/bash
#SBATCH --nodes=6
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=18

#SBATCH --time=24:00:00

#SBATCH --output=results/{fname}_%j.out
#SBATCH --error=mpi_err.err

export OMP_PROC_BIND=close
export OMP_PLACES=cores

''')
        for threads in range(1, 19):
            f.write(f'export OMP_NUM_THREADS={threads}\n')
            for N, Mx, My in params:
                f.write(f'mpirun --map-by numa ./{prog} {T} {Lx} {Ly} {N} {Mx} {My}\n')


if __name__ == '__main__':
    fnames_node = []
    fnames_numa = []
    progs = []

    for i in ['', '_O3']:
        fnames_node.append(f'mpi_node{i}')
        fnames_node.append(f'mpi_node_isend{i}')
        fnames_node.append(f'mpi_node_send{i}')
        fnames_node.append(f'mpi_node_sendrecv{i}')
        fnames_numa.append(f'mpi_numa{i}')
        fnames_numa.append(f'mpi_numa_isend{i}')
        fnames_numa.append(f'mpi_numa_send{i}')
        fnames_numa.append(f'mpi_numa_sendrecv{i}')
        progs.append(f'wave_eq_2d_mpi_openmp{i}')
        progs.append(f'wave_eq_2d_mpi_openmp_isend{i}')
        progs.append(f'wave_eq_2d_mpi_openmp_send{i}')
        progs.append(f'wave_eq_2d_mpi_openmp_sendrecv{i}')
    
    for i in ['', '_O3']:
        create_batch_omp(f'omp{i}', f'wave_eq_2d_mpi_openmp_isend{i}')
    
    for fname, prog in zip(fnames_node, progs):
        create_batch_mpi_per_node(fname, prog)

    for fname, prog in zip(fnames_numa, progs):
        create_batch_mpi_per_numa(fname, prog)

    k = 10

    with open(join(base_dir, 'run_all'), 'w') as f:
        for j in ['', '_O3']:
            for i in range(k):
                f.write(f'sbatch batches/omp{j}\n')
            
        for fname in fnames_node[:len(fnames_node)//2]:
            for i in range(k):
                f.write(f'sbatch batches/{fname}\n')
        
        for fname in fnames_numa[:len(fnames_numa)//2]:
            for i in range(k):
                f.write(f'sbatch batches/{fname}\n')
        
        for fname in fnames_node[len(fnames_node)//2:]:
            for i in range(k):
                f.write(f'sbatch batches/{fname}\n')
        
        for fname in fnames_numa[len(fnames_numa)//2:]:
            for i in range(k):
                f.write(f'sbatch batches/{fname}\n')
