from exceptions import Exception


class TestFailureException(Exception):
    pass


def xmap(func, iterable):
    for obj in iterable:
        yield func(obj)


def CountDeathsFreq(file_name):

    def AddEmptyKeys(occurrences):
        for k in xrange(max(occurrences.keys())):
            if k not in occurrences:
                occurrences[k] = 0

    occurrences = {}
    total_number_count = 0
    with open(file_name, 'r') as in_file:
        for line in in_file:
            n = int(line.strip())
            occurrences[n] = occurrences.get(n, 0) + 1
            total_number_count += 1
    total_number_count = float(total_number_count)
    for k in occurrences:
        occurrences[k] /= total_number_count
    AddEmptyKeys(occurrences)
    return occurrences
