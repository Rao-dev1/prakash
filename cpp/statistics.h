#ifndef STATISTICS_H
#define STATISTICS_H

#include <cmath>

namespace Statistics {

    // Classic Merge Sort algorithm (O(n log n)) for calculating median and MAD
    void merge(double arr[], int left, int mid, int right);
    void mergeSort(double arr[], int left, int right);

    // Calculates the sample median of an array using Merge Sort
    double calculateMedian(const double arr[], int n);

    // Calculates the Median Absolute Deviation (MAD):
    // MAD = median(|x_i - median(X)|)
    double calculateMAD(const double arr[], int n, double median);

    // Robust deviation score: Z = |x - median| / (MAD + epsilon)
    inline double calculateRobustZ(double value, double median, double mad, double epsilon = 1e-6) {
        return std::fabs(value - median) / (mad + epsilon);
    }
}

#endif // STATISTICS_H
