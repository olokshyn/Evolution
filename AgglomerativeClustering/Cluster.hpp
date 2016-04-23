//
// Created by Oleg on 3/13/16.
//

#ifndef WISHART_CLUSTER_H
#define WISHART_CLUSTER_H

#include <vector>
#include <algorithm>
#include <exception>



class IndexOutOfRangeException : public std::exception {
};


template <typename T>
class Cluster {

public:
    typedef double (*measure_type)(const T&, const T&);

    Cluster()
            : vectors() {
    }

    Cluster(size_t size)
            : vectors(size) {
    }

    Cluster(const Cluster& other)
            : vectors(other.vectors) {
    }

    T& operator[](int i) {
        if (i < 0 || i > (int)vectors.size()) {
            throw IndexOutOfRangeException();
        }
        return vectors[i];
    }

    void Add(const T& value) {
        vectors.push_back(value);
    }

    void Add(const Cluster& other) {
        vectors.insert(vectors.end(),
                       other.vectors.begin(),
                       other.vectors.end());
    }

    bool IsInCluster(const T& value) {
        return find(vectors.begin(), vectors.end(), value) != vectors.end();
    }

    size_t GetSize() const {
        return vectors.size();
    }

    double GetNormSum(const Cluster& other, measure_type measure) const {
        double sum = 0;
        for (size_t i = 0; i < vectors.size(); ++i) {
            for (size_t j = 0; j < other.vectors.size(); ++j) {
                sum += measure(vectors[i], other.vectors[j]);
            }
        }
        return sum;
    }

    double GetDistance(const Cluster& other, measure_type measure) {
        return GetNormSum(other, measure)
               / (double)vectors.size()
               / (double)other.vectors.size();
    }

private:
    std::vector<T> vectors;
};

#endif //WISHART_CLUSTER_H
