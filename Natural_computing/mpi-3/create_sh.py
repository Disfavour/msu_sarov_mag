from os.path import dirname, join


base_dir = dirname(__file__)
batches_dir = join(base_dir, 'run.sh')

ps = [1, 2, 3]
Ns = [1200, 600, 300, 150]
T = Ns[0] * 4
Ns = list(reversed(Ns))
#print(Ns)

with open(join(base_dir, 'run.sh'), 'w') as f:
    for N in Ns:
        for p in ps:
            f.write(f'mpirun -n {p**2} --oversubscribe ./a.out {N // p} {T}\n')

