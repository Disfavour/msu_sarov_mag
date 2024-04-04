from os.path import dirname, join


base_dir = dirname(__file__)
batches_dir = join(base_dir, 'run.sh')

m = int(2e4)
k = 20
ps = [2, 4, 8, 16]


with open(join(base_dir, 'run.sh'), 'w') as f:
    for p in ps:
        f.write(f'mpirun -n {p} --oversubscribe ./a.out {m} {k}\n')

