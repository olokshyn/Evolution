import math


def dist(x, y):
    return math.sqrt(sum(map(lambda x, y: (x - y) ** 2, x, y)))


def dist_matrix(vertices):
    matrix = []
    for vert in vertices:
        row = []
        for vert2 in vertices:
            row.append(dist(vert, vert2))
        matrix.append(row)
    return matrix


def k_dists(vertices, k):
    k_ds = []
    for dist in dist_matrix(vertices):
        k_ds.append(sorted(dist)[min(k, len(vertices) - 1)])
    return k_ds


def adjacency_matrix(vertices, k):
    k_ds = k_dists(vertices, k)
    matrix = []
    for i, v1 in enumerate(vertices):
        row = []
        for j, v2 in enumerate(vertices):
            if i != j and dist(v1, v2) <= k_ds[i]:
                row.append(1)
            else:
                row.append(0)
        matrix.append(row)
    return matrix


def print_matrix(matrix):
    for row in matrix:
        print '\t\t'.join([str(x) for x in row])
    print ''


def print_k_dist(vertices, k):
    for i, k_dist in enumerate(k_dists(vertices, k)):
        print '{}: {}'.format(i, k_dist)


def read_vertices(count):
    vertices = []
    i = 0
    while i != count:
        line = raw_input('Enter {} vertex: '.format(i + 1))
        vertices.append(map(float, line.split()))
        i += 1
    return vertices


d = dist
dm = dist_matrix
kd = k_dists
am = adjacency_matrix
pm = print_matrix
pkd = print_k_dist
rv = read_vertices
