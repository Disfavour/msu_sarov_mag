import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
from os.path import join, dirname


def whoisnext(state):
    return 1 if state.sum() // 10 == state.sum() % 10 else 10


def is_game_ended(state):
    data = np.concatenate([state.sum(1), state.sum(0), np.array((state.trace(), np.fliplr(state).trace()))])
    return any(data == 3) or any(data == 30) or (np.count_nonzero(state) == 9)


def whoiswinner(state):
    data = np.concatenate([state.sum(1), state.sum(0), np.array((state.trace(), np.fliplr(state).trace()))])
    return any(data == 3), any(data == 30)


def get_weight(state, sign):
    data = np.concatenate([state.sum(1), state.sum(0), np.array((state.trace(), np.fliplr(state).trace()))])
    possible_1 = data.size - np.count_nonzero(data // 10)
    possible_10 = data.size - np.count_nonzero(data % 10)
    res = None
    if sign == 1:
        res = possible_1 - possible_10
        if any(data == 3):
            res = np.inf
        elif any(data == 30):
            res = -np.inf
    elif sign == 10:
        res = possible_1 - possible_10
        if any(data == 30):
            res = np.inf
        elif any(data == 3):
            res = -np.inf

    return res


def generate_new_states(state):
    sign = whoisnext(state)
    new_states = []

    free_elements = np.where(state == 0)

    # if len(free_elements) < 2:
    #     return new_states

    for i, j in zip(free_elements[0], free_elements[1]):
        new_state = state.copy()
        new_state[i, j] = sign
        new_states.append(new_state)
    return new_states


def get_vertexes_at_depth(root, depth):
    vertexes_at_depth = []
    vertexes = [root]
    while len(vertexes) > 0:
        v = vertexes.pop(0)
        if v.depth == depth:
            vertexes_at_depth.append(v)
        vertexes += v.children
    return vertexes_at_depth


class Tree:
    def __init__(self, parent, depth, state):
        self.parent = parent
        self.children = []
        self.depth = depth
        self.state = state
        self.weight = None


def construct_tree(state, nsteps):
    depth = 0
    root = Tree(None, depth, state)
    for depth in range(2*nsteps):
        vertexes_at_depth = get_vertexes_at_depth(root, depth)
        for v in vertexes_at_depth:
            if not is_game_ended(v.state):
                for new_state in generate_new_states(v.state):
                    new_vertex = Tree(v, depth+1, new_state)
                    v.children.append(new_vertex)
    return root
    
def set_weights(root, nsteps, sign):
    for v in get_vertexes_at_depth(root, 2*nsteps):
        v.weight = get_weight(v.state, sign)
    
    for depth in range(2*nsteps - 1, 0, -1):
        choose = min if depth % 2 else max
        for v in get_vertexes_at_depth(root, depth):
            if len(v.children) > 0:
                v.weight = choose([child.weight for child in v.children])
            else:
                v.weight = get_weight(v.state, sign)


def minmax_step(state, nsteps):
    sign = whoisnext(state)
    root = construct_tree(state, nsteps)
    set_weights(root, nsteps, sign)
    best_state = max(root.children, key=lambda x: x.weight).state
    return best_state


def get_tree(state, nsteps):
    sign = whoisnext(state)
    root = construct_tree(state, nsteps)
    set_weights(root, nsteps, sign)
    return root


def get_str_state(state):
    state = np.where(state == 10, 9, state)
    res = ''
    for row in state:
        res += str(row)[1:-1] + '\n'
    return res[:-1].replace('9', 'O').replace('1', 'X').replace('0', '_')


def get_graph_from_tree(root):
    G = nx.DiGraph()
    stack = [root]
    while len(stack) > 0:
        v = stack.pop(0)
        G.add_weighted_edges_from([(get_str_state(v.state), get_str_state(child.state), child.weight) for child in v.children])
        stack += v.children
    return G


def plot_tree(root, fname=None):
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
    #plt.savefig(fname)
    plt.show()


if __name__ == '__main__':
    cur_dir = dirname(__file__)
    state = np.array([
        [10, 1, 10],
        [0, 0, 1],
        [1, 1, 10]
    ])

    # state = np.array([
    #     [10, 10, 1],
    #     [0, 0, 10],
    #     [0, 1, 1]
    # ])

    #state = np.zeros((3, 3), dtype=int)

    nsteps=2
    root = get_tree(state, nsteps)
    plot_tree(root)
    #plot_tree(root, join(cur_dir, f'tic_tac_toe_{nsteps}_step.pdf'))

    #print(minmax_step(state, nsteps=1))
