import math

from Common import xmap, TestFailureException
from Distrs import BaseDistr, BaseDistrAnalyzer


class NormalDistr(BaseDistr):

    exp_value = None
    dev = None

    def __init__(self, data):
        super(NormalDistr, self).__init__(data)

        self.exp_value = sum(data) / float(len(data))
        self.dev = math.sqrt(sum(xmap(lambda x: (x - self.exp_value) ** 2, data)) / float(len(data) - 1))


class NormalDistrAnalyzer(BaseDistrAnalyzer):

    concrete_distr = NormalDistr

    def _TestDistr(self, distr, data, critical_value=12.384):

        def getC(c):
            n = len(c)
            for i in xrange(2 * n - 1):
                if i < n:
                    yield c[i]
                else:
                    yield -c[n - i - 2]

        k = 10
        k_c = [-1.2816, -0.8416, -0.5244, -0.2533, 0]

        points_sum = 0.0
        points_seen = 0
        a = None
        for c in getC(k_c):
            b = distr.exp_value + c * distr.dev
            m = 0
            for d in data:
                if d <= b:
                    if a is None or d > a:
                        m += 1
                        points_seen += 1
            points_sum += m ** 2
            a = b
        m = 0  # TODO: optimize this as points_sum += (len(data) - points_seen) ** 2 after debug
        for d in data:
            if d > a:
                m += 1
                points_seen += 1
        points_sum += m ** 2

        assert points_seen == len(data)

        n = float(len(data))
        khi_2 = k / n * points_sum - n
        print "khi_2 = %s" % khi_2
        return khi_2 <= critical_value

    def TestMe(self):
        max_error = 1

        exp_value = 101
        dev = 21.7
        data = [43, 76, 84, 91, 95, 101, 105, 114, 122, 129,
                54, 77, 84, 91, 96, 101, 106, 114, 122, 132,
                56, 77, 85, 91, 96, 101, 107, 115, 122, 134,
                57, 78, 85, 91, 96, 103, 107, 116, 123, 136,
                61, 78, 86, 92, 97, 103, 107, 116, 124, 136,
                64, 79, 87, 92, 97, 104, 108, 116, 124, 138,
                67, 79, 87, 93, 98, 104, 111, 117, 125, 143,
                73, 82, 87, 93, 98, 104, 112, 118, 125, 143,
                74, 82, 88, 93, 99, 104, 113, 118, 125, 145,
                76, 83, 89, 95, 101, 105, 114, 119, 126, 150]

        distr = self.Estimate(data)
        print distr.exp_value
        print distr.dev
        if math.fabs(distr.exp_value - exp_value) > max_error:
            raise TestFailureException("Expected value:\n expected: %s\n actual: %s" % (exp_value, distr.exp_value))
        if math.fabs(distr.dev - dev) > max_error:
            raise TestFailureException("Standard deviation:\n expected: %s\n actual: %s" % (dev, distr.dev))

        if not self.TestDistr(distr, data):
            raise TestFailureException("Test for Normal distribution failed")
