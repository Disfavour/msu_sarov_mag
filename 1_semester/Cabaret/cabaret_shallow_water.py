import numpy as np

g = 9.8


def cabaret_shallow_water(h, u, r, h_x, u_l, u_r):
    h_t = get_h_t(h, u, r, h_x)

    h_n_plus_half, u_n_plus_half = phase_1(h, u, h_t, h_x)

    h_n_plus_one, u_n_plus_one = phase_2(h_n_plus_half, u_n_plus_half, h, u, u_l, u_r)

    h_n_plus_one_c, u_n_plus_one_c = phase_3(h_n_plus_half, u_n_plus_half, h_n_plus_one, u_n_plus_one, h_t, h_x)

    h_ans = np.empty(h.shape)
    h_ans[::2] = h_n_plus_one
    h_ans[1::2] = h_n_plus_one_c

    u_ans = np.empty(u.shape)
    u_ans[::2] = u_n_plus_one
    u_ans[1::2] = u_n_plus_one_c

    return h_ans, u_ans


def phase_3(h_n_plus_half, u_n_plus_half, h_n_plus_one, u_n_plus_one, h_t, h_x):
    n = h_n_plus_one.shape[0] - 1

    h_n_plus_one_c = h_n_plus_half - h_t / (2 * h_x) * \
                     (h_n_plus_one[1:] * u_n_plus_one[1:] - h_n_plus_one[:n] * u_n_plus_one[:n])
    u_n_plus_one_c = (h_n_plus_half * u_n_plus_half - h_t / (2 * h_x) *
                      (h_n_plus_one[1:] * u_n_plus_one[1:] ** 2 - h_n_plus_one[:n] * u_n_plus_one[:n] ** 2 +
                       g / 2 * (h_n_plus_one[1:] ** 2 - h_n_plus_one[:n] ** 2))) / h_n_plus_one_c

    return h_n_plus_one_c, u_n_plus_one_c


def phase_2(h_n_plus_half, u_n_plus_half, h, u, u_l, u_r):
    R_n_plus_one, Q_n_plus_one = get_invariants(h_n_plus_half, u_n_plus_half, h, u, u_l, u_r)

    u_n_plus_one = (R_n_plus_one + Q_n_plus_one) / 2
    h_n_plus_one = (R_n_plus_one - u_n_plus_one) ** 2 / (4 * g)

    return h_n_plus_one, u_n_plus_one


def get_invariants(h_n_plus_half, u_n_plus_half, h, u, u_l, u_r):
    n = h.shape[0] - 1

    R_n = u + 2 * np.sqrt(g * h)
    Q_n = u - 2 * np.sqrt(g * h)

    R_n_plus_half = u_n_plus_half + 2 * np.sqrt(g * h_n_plus_half)
    Q_n_plus_half = u_n_plus_half - 2 * np.sqrt(g * h_n_plus_half)

    R_n_plus_one = 2 * R_n_plus_half - R_n[:n:2]
    Q_n_plus_one = 2 * Q_n_plus_half - Q_n[2::2]

    R_min = np.minimum.reduce((R_n[:n:2], R_n[1::2], R_n[2::2]))
    R_max = np.maximum.reduce((R_n[:n:2], R_n[1::2], R_n[2::2]))

    Q_min = np.minimum.reduce((Q_n[:n:2], Q_n[1::2], Q_n[2::2]))
    Q_max = np.maximum.reduce((Q_n[:n:2], Q_n[1::2], Q_n[2::2]))

    R_n_plus_one = np.where(R_n_plus_one < R_min, R_min, R_n_plus_one)
    R_n_plus_one = np.where(R_n_plus_one > R_max, R_max, R_n_plus_one)

    Q_n_plus_one = np.where(Q_n_plus_one < Q_min, Q_min, Q_n_plus_one)
    Q_n_plus_one = np.where(Q_n_plus_one > Q_max, Q_max, Q_n_plus_one)

    R_n_plus_one = np.insert(R_n_plus_one, 0, -Q_n_plus_one[0] + 2 * u_l)
    Q_n_plus_one = np.append(Q_n_plus_one, -R_n_plus_one[-1] + 2 * u_r)

    return R_n_plus_one, Q_n_plus_one


def phase_1(h, u, h_t, h_x):
    n = h.shape[0] - 1

    h_n_plus_half = h[1::2] - h_t / (2 * h_x) * (h[2::2] * u[2::2] - h[:n:2] * u[:n:2])
    u_n_plus_half = (h[1::2] * u[1::2] - h_t / (2 * h_x) * (h[2::2] * u[2::2] ** 2 - h[:n:2] * u[:n:2] ** 2 +
                                                            g / 2 * (h[2::2] ** 2 - h[:n:2] ** 2))) / h_n_plus_half

    return h_n_plus_half, u_n_plus_half


def get_h_t(h, u, r, h_x):
    return (r * h_x / (np.abs(u) + np.sqrt(g * h))).min()


if __name__ == '__main__':
    pass
