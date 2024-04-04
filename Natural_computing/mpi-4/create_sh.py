from os.path import dirname, join


base_dir = dirname(__file__)
batches_dir = join(base_dir, 'run.sh')

k = 20
ps = [2, 4, 8, 16]

m = 2000
with open(join(base_dir, 'run1.sh'), 'w') as f:
    for p in ps:
        f.write(f'mpirun -n {p} --oversubscribe ./a.out {m} {k}\n')

m = 16000
with open(join(base_dir, 'run2.sh'), 'w') as f:
    for p in ps:
        f.write(f'mpirun -n {p} --oversubscribe ./a.out {m} {k}\n')

m = 2500
with open(join(base_dir, 'run3.sh'), 'w') as f:
    for p in ps:
        f.write(f'mpirun -n {p} --oversubscribe ./a.out {m} {k}\n')
