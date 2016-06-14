import sys
from exceptions import ValueError, IOError

from Distrs import Normal
from Plotter import BuildHistogram, BuildPlot, ShowPlot
from Common import CountDeathsFreq, DataFileReader


def main(argv):
    if len(argv) == 2 and argv[1] == "RUN_TESTS":
        distr_analyzer = Normal.NormalDistrAnalyzer()
        distr_analyzer.TestMe()
        return 0
    elif len(argv) == 2:
        death_file = argv[1]
        fitness_file = None
    elif len(argv) == 3:
        death_file = argv[1]
        fitness_file = argv[2]
    else:
        print "Usage: python Analyzer.py death-file [fitness-file]"
        sys.exit(-1)

    try:
        occurrences = CountDeathsFreq(death_file)

        data = occurrences.values()
        distr_analyzer = Normal.NormalDistrAnalyzer()
        distr = distr_analyzer.Estimate(data)
        if distr_analyzer.TestDistr(distr, data):
            print "Distribution obeys Normal law"
        else:
            print "Distribution does NOT obey Normal law"

        BuildHistogram(occurrences)
        if fitness_file is not None:
            BuildPlot(DataFileReader(fitness_file))
        ShowPlot()

    except IOError as ex:
        print "Failed to open file: %s" % ex.message
        return -1
    except ValueError as ex:
        print "File contains invalid data: %s" % ex.message
        return -1
    except Exception as ex:
        print "Unexpected error: %s" % ex.message
        return -1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
