import matplotlib.pyplot as plt
import numpy as np
import random
from os.path import dirname, join
import time


def get_points(npoints, radius):
    angles = np.linspace(0, 2*np.pi, npoints, endpoint=False)
    x_coords = radius * np.cos(angles)
    y_coords = radius * np.sin(angles)
    points = list(zip(x_coords, y_coords))
    return points


def encode(points):
    n2p = {}
    for i, point in enumerate(points):
        n2p[i] = point
    return n2p


def get_distances_dict(n2p):
    d = {}
    for n1 in range(len(n2p)):
        for n2 in range(n1+1, len(n2p)):
            p1, p2 = n2p[n1], n2p[n2]
            distance = ((p2[0]-p1[0])**2 + (p2[1]-p1[1])**2) ** 0.5
            d[n1, n2] = distance
            d[n2, n1] = distance
    return d


def setup(npoints, radius):
    points = get_points(npoints, radius)
    n2p = encode(points)
    distances_dict = get_distances_dict(n2p)
    return distances_dict, n2p


def get_full_way(individual):
    return [0] + individual + [0]


def fitness_function(individual, distances_dict):
    distance = 0
    full_way = get_full_way(individual)
    for n1, n2 in zip(full_way, full_way[1:]):
        distance += distances_dict[n1, n2]
    return -distance


def roulette(population, estimates, nindividuals, offset=1):
    individuals = []

    normalized_estimates = []
    min_estimate = min(estimates) - offset
    for estimate in estimates:
        normalized_estimates.append(estimate - min_estimate)

    cumulative_probabilities = []
    cumulative_sum = 0
    normalized_estimates_sum = sum(normalized_estimates)
    for estimate in normalized_estimates:
        probability = estimate / normalized_estimates_sum
        cumulative_sum += probability
        cumulative_probabilities.append(cumulative_sum)

    while len(individuals) != nindividuals:
        rand_num = random.random()
        for i, cum_prob in enumerate(cumulative_probabilities):
            if rand_num <= cum_prob:
                individual = population[i]
                if not(len(individuals) % 2 and individual == individuals[-1]):
                    individuals.append(individual)
                break
    return individuals


# Partially Matched Crossover
def pmx_crossover(parent1, parent2):
    size = len(parent1)
    child = [-1] * size
    mapping = {}

    # p1 = random.randint(0, size - 1)
    # p2 = random.randint(0, size - 1)
    # if p1 > p2:
    #     p1, p2 = p2, p1

    # p1, p2 = size // 4, 3 * size // 4
    # child[p1:p2] = parent1[p1:p2]

    p1, p2 = size // 3, 2 * size // 3
    child[p1:p2+1] = parent1[p1:p2+1]

    for i in range(p1, p2+1):
        mapping[parent1[i]] = parent2[i]
    
    for i in range(size):
        if i < p1 or i > p2:
            if parent2[i] not in child:
                child[i] = parent2[i]
            else:
                mapped_gene = mapping[parent2[i]]
                while mapped_gene in child:
                    mapped_gene = mapping[mapped_gene]
                child[i] = mapped_gene
    return child


def mutation(population, probability):
    for individual in population:
        if random.random() <= probability:
            idx1, idx2 = random.randint(0, len(individual)-1), random.randint(0, len(individual)-1)
            individual[idx1], individual[idx2] = individual[idx2], individual[idx1]


def GA(population_size, epochs, mutation_probability, npoints, radius):
    best_estimates = []
    average_estimates = []
    worst_estimates = []
    best_estimate = -np.inf
    best_individual = None
    best_epoch = -1

    distances_dict, n2p = setup(npoints, radius)
    base_individual = list(range(1, npoints))

    epoch = 0
    
    population = []
    for i in range(population_size):
        new_individual = base_individual.copy()
        random.shuffle(new_individual)
        population.append(new_individual)

    estimates = []
    for individual in population:
        estimates.append(fitness_function(individual, distances_dict))

    def collect_data():
        nonlocal best_estimate, best_individual, best_epoch
        best_estimates.append(max(estimates))
        average_estimates.append(sum(estimates) / len(estimates))
        worst_estimates.append(min(estimates))
        if best_estimates[-1] > best_estimate:
            best_estimate = best_estimates[-1]
            best_individual = population[estimates.index(best_estimate)]
            best_epoch = epoch
        if epoch % 100 == 0:
            print(f'epoch {epoch} current best = {best_estimates[-1]} best = {best_estimate}')
    
    collect_data()

    for epoch in range(1, epochs + 1):
        new_population = []
        new_estimates = []

        crossover_individuals = roulette(population, estimates, population_size)
        for ind1, ind2 in zip(crossover_individuals[::2], crossover_individuals[1::2]):
            new_population.append(pmx_crossover(ind1, ind2))
            new_population.append(pmx_crossover(ind2, ind1))
        
        mutation(new_population, mutation_probability)
        
        for individual in new_population:
            new_estimates.append(fitness_function(individual, distances_dict))
        
        population = new_population
        estimates = new_estimates

        collect_data()
    
    return best_estimates, average_estimates, worst_estimates, best_estimate, best_individual, best_epoch, n2p


def plot_estimates(best, average, worst, fname):
    plt.figure(figsize=(6.4, 3.6), dpi=300, tight_layout=True)
    plt.plot(best)
    plt.plot(average)
    plt.plot(worst)
    plt.xlabel('epoch')
    plt.ylabel('fitness function')
    plt.legend(['best', 'average', 'worst'])
    plt.grid()
    plt.savefig(fname)
    plt.close()


def plot_individual(individual, n2p, fname):
    way = get_full_way(individual)
    way_points = []
    for i in way:
        way_points.append(n2p[i])
    x = [i[0] for i in way_points]
    y = [i[1] for i in way_points]
    plt.figure(figsize=(6.4, 3.6), dpi=300, tight_layout=True)
    plt.plot(x, y)
    plt.scatter(x, y)
    plt.xlabel('x')
    plt.ylabel('y')
    plt.grid()
    plt.savefig(fname)
    plt.close()


if __name__ == '__main__':
    base_dir = dirname(__file__)
    random.seed(1)

    populations_size = 200
    nepochs = 5000
    mutation_probability = 0.02

    start_time = time.time()

    best_estimates, average_estimates, worst_estimates, best_estimate, best_individual, best_epoch, n2p \
        = GA(populations_size, nepochs, mutation_probability, npoints=30, radius=5)

    print(f'Elapsed time {time.time() - start_time} sec')

    print(f'Best distance = {best_estimate}, best epoch = {best_epoch}')
    plot_estimates(best_estimates, average_estimates, worst_estimates, join(base_dir, f'best_estimates_{populations_size}_{nepochs}_{mutation_probability}.png'))
    plot_individual(best_individual, n2p, join(base_dir, f'best_individual_{populations_size}_{nepochs}_{mutation_probability}.png'))
