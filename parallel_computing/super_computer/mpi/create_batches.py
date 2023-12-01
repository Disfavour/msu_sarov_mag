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

export OMP_PROC_BIND=close
export OMP_PLACES=cores

''')

    for threads in range(12, 37, 12):
         f.write(f'export OMP_NUM_THREADS={threads}\n')
         for N, Mx, My in params:
              f.write(f'mpirun --bind-to none ./wave_eq_2d_mpi_openmp {T} {Lx} {Ly} {N} {Mx} {My}\n')


def create_batch_mpi_per_node(filename, prog):
    with open(join(batches_dir, filename), 'w') as f:
        f.write(f'''#!/bin/bash
#SBATCH --nodes=6
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=36

#SBATCH --time=24:00:00

#SBATCH --output=results/{filename}_%j.out
#SBATCH --error=mpi_err.err

export OMP_PROC_BIND=close
export OMP_PLACES=cores

''')

        for threads in range(2, 37, 2):
            f.write(f'export OMP_NUM_THREADS={threads}\n')
            for N, Mx, My in params:
                f.write(f'mpirun --bind-to none ./{prog} {T} {Lx} {Ly} {N} {Mx} {My}\n')

create_batch_mpi_per_node('mpi_node', 'wave_eq_2d_mpi_openmp')
create_batch_mpi_per_node('mpi_node_isend', 'wave_eq_2d_mpi_openmp_isend')
create_batch_mpi_per_node('mpi_node_send', 'wave_eq_2d_mpi_openmp_send')
create_batch_mpi_per_node('mpi_node_sendrecv', 'wave_eq_2d_mpi_openmp_sendrecv')

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
                f.write(f'mpirun --map-by numa ./{prog} {T} {Lx} {Ly} {N} {Mx} {My}\n')

create_batch_mpi_per_numa('mpi_numa', 'wave_eq_2d_mpi_openmp')
create_batch_mpi_per_numa('mpi_numa_isend', 'wave_eq_2d_mpi_openmp_isend')
create_batch_mpi_per_numa('mpi_numa_send', 'wave_eq_2d_mpi_openmp_send')
create_batch_mpi_per_numa('mpi_numa_sendrecv', 'wave_eq_2d_mpi_openmp_sendrecv')

with open(join(base_dir, 'run_all'), 'w') as f:
    k = 10
    for i in range(k):
        f.write('sbatch batches/omp\n')
    
    for i in range(k):
        f.write('sbatch batches/mpi_node\n')
    
    for i in range(k):
        f.write('sbatch batches/mpi_node_isend\n')

    for i in range(k):
        f.write('sbatch batches/mpi_node_send\n')

    for i in range(k):
        f.write('sbatch batches/mpi_node_sendrecv\n')
    
    for i in range(k):
        f.write('sbatch batches/mpi_numa\n')
    
    for i in range(k):
        f.write('sbatch batches/mpi_numa_isend\n')

    for i in range(k):
        f.write('sbatch batches/mpi_numa_send\n')

    for i in range(k):
        f.write('sbatch batches/mpi_numa_sendrecv\n')
