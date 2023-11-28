from os.path import dirname, join


base_dir = dirname(__file__)
batches_dir = join(base_dir, 'batches')

T, Lx, Ly = 5, 6, 4
params = tuple(tuple(map(lambda x: x * 100 * 2**i, (T, Lx, Ly))) for i in range(4))


with open(join(batches_dir, 'omp'), 'w') as f:
    f.write('''#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=36

#SBATCH --time=24:00:00

#SBATCH --output=results/omp_%j.out
#SBATCH --error=mpi_err.err

export OMP_PROC_BIND=spread
export OMP_PLACES=cores

''')

    for threads in range(12, 37, 12):
         f.write(f'export OMP_NUM_THREADS={threads}\n')
         for N, Mx, My in params:
              f.write(f'mpirun --bind-to none ./wave_eq_2d_mpi_openmp 1 1 {T} {Lx} {Ly} {N} {Mx} {My}\n')


def create_batch_mpi_per_node(filename, prog):
    with open(join(batches_dir, filename), 'w') as f:
        f.write(f'''#!/bin/bash
#SBATCH --nodes=6
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=36

#SBATCH --time=24:00:00

#SBATCH --output=results/{filename}_%j.out
#SBATCH --error=mpi_err.err

export OMP_PROC_BIND=spread
export OMP_PLACES=cores

''')

        for threads in range(2, 37, 2):
            f.write(f'export OMP_NUM_THREADS={threads}\n')
            for N, Mx, My in params:
                f.write(f'mpirun --bind-to none ./{prog} 2 3 {T} {Lx} {Ly} {N} {Mx} {My}\n')

create_batch_mpi_per_node('mpi_per_node', 'wave_eq_2d_mpi_openmp')
create_batch_mpi_per_node('mpi_per_node_v2', 'wave_eq_2d_mpi_openmp_v2')

def create_batch_mpi_per_numa(filename, prog):
    with open(join(batches_dir, filename), 'w') as f:
        f.write(f'''#!/bin/bash
#SBATCH --nodes=6
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=18

#SBATCH --time=24:00:00

#SBATCH --output=results/{filename}_%j.out
#SBATCH --error=mpi_err.err

export OMP_PROC_BIND=close
export OMP_PLACES=cores

''')

        for threads in range(1, 19):
            f.write(f'export OMP_NUM_THREADS={threads}\n')
            for N, Mx, My in params:
                f.write(f'mpirun --map-by numa ./{prog} 3 4 {T} {Lx} {Ly} {N} {Mx} {My}\n')

create_batch_mpi_per_numa('mpi_per_numa', 'wave_eq_2d_mpi_openmp')
create_batch_mpi_per_numa('mpi_per_numa_v2', 'wave_eq_2d_mpi_openmp_v2')

with open(join(base_dir, 'run_all'), 'w') as f:
    for i in range(10):
        f.write('sbatch batches/omp\n')
    
    for i in range(10):
        f.write('sbatch batches/mpi_per_node\n')
    
    for i in range(10):
        f.write('sbatch batches/mpi_per_node_v2\n')

    for i in range(10):
        f.write('sbatch batches/mpi_per_numa\n')

    for i in range(10):
        f.write('sbatch batches/mpi_per_numa_v2\n')
