import numpy as np
import networkx as nx
import matplotlib.pyplot as plt


def h1(state, target_state):
    value = 0
    for i1, row in enumerate(state):
        for j1, element in enumerate(row):
            if element > 0:
                value += element != target_state[i1, j1]
    return value

def f1(state, target_state, depth):
    return depth + h1(state, target_state)


def h2(state, target_state):
    value = 0
    for i1, row in enumerate(state):
        for j1, element in enumerate(row):
            i2, j2 = map(lambda x: x[0], np.where(target_state == element))
            value += abs(i1-i2) + abs(j1-j2)
    return value


def get_linear(state):
    return list(np.concatenate([state[0, :], state[1:, -1], state[-1, -2::-1], state[-2::-1, 0]]))


def S(state, target_state):
    value = 0
    cur_linear, target_linear = map(get_linear, (state, target_state))
    for i, element in enumerate(cur_linear[:-1]):
        if element == 0:
            continue
        next_element = cur_linear[i+1]
        next_element_goal = target_linear[target_linear.index(element)+1]
        value += 2*(next_element != next_element_goal)
    value += state[1, 1] != 0
    return value


def h3(state, target_state):
    return h2(state, target_state) + 3*S(state, target_state)


def f2(state, target_state, depth):
    return depth + h3(state, target_state)


def generate_new_states(state):
    new_states = []
    i, j = map(lambda x: x[0], np.where(state == 0))

    if i > 0:
        new_state = state.copy()
        new_state[i, j], new_state[i-1, j] = new_state[i-1, j], new_state[i, j]
        new_states.append(new_state)
    if i < 2:
        new_state = state.copy()
        new_state[i, j], new_state[i+1, j] = new_state[i+1, j], new_state[i, j]
        new_states.append(new_state)
    
    if j > 0:
        new_state = state.copy()
        new_state[i, j], new_state[i, j-1] = new_state[i, j-1], new_state[i, j]
        new_states.append(new_state)
    if j < 2:
        new_state = state.copy()
        new_state[i, j], new_state[i, j+1] = new_state[i, j+1], new_state[i, j]
        new_states.append(new_state)

    return new_states


class Tree:
    def __init__(self, state, weight, depth):
        self.children = []
        self.state = state
        self.weight = weight
        self.depth = depth


def DFS_min_state(v, min_v, closed):
    if not any(np.array_equal(v.state, closed_state) for closed_state in closed) and min_v.weight > v.weight:
        min_v = v
    for child in v.children:
        min_v = DFS_min_state(child, min_v, closed)
    return min_v


def BFS_min_state(root, min_v, closed):
    vertexes = [root]
    while len(vertexes) > 0:
        v = vertexes.pop(0)
        if not any(np.array_equal(v.state, closed_state) for closed_state in closed) and min_v.weight > v.weight:
            min_v = v
        vertexes += v.children
    return min_v


def heuristic_search(root, target_state, f, closed, v4start):
    vertex = BFS_min_state(root, v4start, closed)
    for new_state in generate_new_states(vertex.state):
        if not any(np.array_equal(new_state, closed_state) for closed_state in closed):
            new_vertex = Tree(new_state, f(new_state, target_state, vertex.depth+1), vertex.depth+1)
            vertex.children.append(new_vertex)
            if np.array_equal(new_state, target_state):
                return
    closed.append(vertex.state)
    # plot_tree(root)
    return heuristic_search(root, target_state, f, closed, vertex.children[0])


def get_str_state(state):
    res = ''
    for row in state:
        res += str(row)[1:-1] + '\n'
    return res[:-1].replace('0', '  ')


def get_graph_from_tree(root):
    G = nx.DiGraph()
    stack = [root]
    while len(stack) > 0:
        v = stack.pop(0)
        G.add_weighted_edges_from([(get_str_state(v.state), get_str_state(child.state), child.weight) for child in v.children])
        stack += v.children
    return G


def plot_tree(root):
    G = get_graph_from_tree(root)
    options = {
        'node_color': 'yellow',
        'node_size': 3000,
        'width': 1,
        'arrowsize': 18,
        'font_size': 12,
    }
    pos = nx.nx_agraph.graphviz_layout(G, prog="dot")
    nx.draw(G, pos, with_labels=True, **options)
    ax = plt.gca()
    ax.collections[0].set_edgecolor("#000000")
    nx.draw_networkx_edge_labels(G, pos, edge_labels=nx.get_edge_attributes(G, 'weight'), font_size=12)
    plt.show()

# pip install networkx[default]
# sudo apt-get install graphviz graphviz-dev
# pip install pygraphviz
# Вес ребра - это вес вершины, в которую оно направлено

if __name__ == '__main__':
    # start_state = np.array([
    #     [2, 8, 3],
    #     [1, 6, 4],
    #     [7, 0, 5]
    # ])
    start_state = np.array([
        [2, 8, 1],
        [4, 0, 3],
        [7, 6, 5]
    ])
    target_state = np.array([
        [1, 2, 3],
        [8, 0, 4],
        [7, 6, 5]
    ])

    root = Tree(start_state, f2(start_state, target_state, 0), 0)
    closed = []
    heuristic_search(root, target_state, f2, closed, root)
    plot_tree(root)
