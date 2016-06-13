from exceptions import TypeError


class BaseDistr(object):

    def __init__(self, data):
        pass


class BaseDistrAnalyzer(object):

    concrete_distr = BaseDistr

    def Estimate(self, data):
        return self.__class__.concrete_distr(data)

    def TestDistr(self, distr, data):
        if not isinstance(distr, self.__class__.concrete_distr):
            raise TypeError("Distribution object must be of type %s" % self.__class__.concrete_distr.__name__)
        return self._TestDistr(distr, data)

    def _TestDistr(self, distr, data):
        pass

    def TestMe(self):
        pass
