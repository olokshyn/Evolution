import sys
import matplotlib.pyplot as plt
from exceptions import ValueError, IOError


def AddEmptyKeys(occurrences):
    for k in xrange(max(occurrences.keys())):
        if k not in occurrences:
            occurrences[k] = 0


def CountDeathsFreq(file_name):
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


def BuildPlot(occurrences):
    items = occurrences.items()
    items.sort(key=lambda x: x[0])
    plt.bar([x[0] for x in items], [x[1] for x in items], align="center")
    plt.show()


def main():
    if len(sys.argv) != 2:
        print "Usage: python plotter.py [data-file]"
        sys.exit(-1)

    file_name = sys.argv[1]

    try:
        BuildPlot(CountDeathsFreq(file_name))
    except IOError:
        print "Failed to open file %s" % file_name
        return -1
    except ValueError:
        print "File %s contains invalid data" % file_name
        return -1
    except Exception:
        print "Unexpected error"
        return -1
    return 0


if __name__ == "__main__":
    sys.exit(main())
