import numpy as np

# all = []
# for i in range(14, 29):
#     all.append(np.loadtxt(f'slurms/slurm-{i}.out', delimiter=','))
#
# for i in range(len(all)):
#     for j in range(i+1, len(all)):
#         print(np.allclose(all[i], all[j]))

a = np.array([
    [5, 4, 3],
    [0, 1, 2],
    [8, 7, 6]
], dtype=float)

print(type(a), type(a[0][0]))

print(np.sum(a, axis = 0))
print(np.sum(a, axis = 1))

print(np.argsort(a, 1))
ind = np.argsort(a, 0)
print(ind)

b = np.array(a,
       dtype=[('norm_L2', float), ('norm_C', float), ('time', float), ('tau', float), ('h', float), ('p', float)])

print(b)

# Ваш двумерный массив
data = np.array([
    [1.0, 2.0, 100, 200, 30],
    [2.0, 1.0, 150, 250, 10],
    [3.0, 3.0, 100, 200, 20]
])
print(data)

# Индекс сортировки на основе пятой колонки (индекс 4)
sorting_index = np.argsort(data[:, 4])
print(sorting_index)

# Применяем индекс сортировки к массиву, чтобы получить отсортированные строки
sorted_data = data[sorting_index]

# Выводим отсортированный массив
print(sorted_data)