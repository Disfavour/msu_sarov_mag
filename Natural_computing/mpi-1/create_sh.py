from os.path import dirname, join


base_dir = dirname(__file__)
batches_dir = join(base_dir, 'run.sh')

a, b, x, prob = 0, 3000, 1500, 0.5

ps = [1, 2, 4, 8]
Ns = [p*1000 for p in ps]
print(Ns)

with open(join(base_dir, 'run.sh'), 'w') as f:
    for N in Ns:
        for p in ps:
            f.write(f'mpirun -n {p} ./a.out {a} {b} {x} {prob} {N}\n')

