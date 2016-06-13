import sys
from exceptions import ValueError, IOError

from Distrs import Normal
from Plotter import BuildPlot
from Common import CountDeathsFreq


def main(argv):
    if len(argv) != 2:
        print "Usage: python Analyzer.py [data-file]"
        sys.exit(-1)

    if argv[1] == "RUN_TESTS":
        distr_analyzer = Normal.NormalDistrAnalyzer()
        distr_analyzer.TestMe()
        return 0

    file_name = argv[1]

    try:
        occurrences = CountDeathsFreq(file_name)

        data = occurrences.values()
        distr_analyzer = Normal.NormalDistrAnalyzer()
        distr = distr_analyzer.Estimate(data)
        if distr_analyzer.TestDistr(distr, data):
            print "Distribution obeys Normal law"
        else:
            print "Distribution does NOT obey Normal law"

        BuildPlot(occurrences)

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
