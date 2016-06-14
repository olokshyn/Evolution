import sys
import matplotlib.pyplot as plt


def BuildPlot(occurrences):
    items = occurrences.items()
    items.sort(key=lambda x: x[0])
    if items and items[0][0] == 0:
        items.pop(0)
    plt.bar([x[0] for x in items], [x[1] for x in items], align="center")
    plt.show()


def main(argv):
    if len(argv) != 2:
        print "Usage: python Plotter.py [data-file]"
        sys.exit(-1)

    from Common import CountDeathsFreq
    from exceptions import ValueError, IOError

    file_name = argv[1]

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
    sys.exit(main(sys.argv))
