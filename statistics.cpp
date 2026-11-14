#include "statistics.h"
#include <vector>
#include <cmath>

namespace Statistics {

    // Merge two sorted subarrays: arr[left..mid] and arr[mid+1..right]
    void merge(double arr[], int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        std::vector<double> L(n1);
        std::vector<double> R(n2);

        for (int i = 0; i < n1; ++i)
            L[i] = arr[left + i];
        for (int j = 0; j < n2; ++j)
            R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i] <= R[j]) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }

        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }

    // Merge Sort recursively divides and sorts in O(n log n)
    void mergeSort(double arr[], int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(arr, left, mid);
            mergeSort(arr, mid + 1, right);
            merge(arr, left, mid, right);
        }
    }

    double calculateMedian(const double arr[], int n) {
        if (n <= 0) return 0.0;
        
        // Make a local copy so the input is not mutated
        std::vector<double> temp(arr, arr + n);
        mergeSort(temp.data(), 0, n - 1);

        if (n % 2 != 0) {
            return temp[n / 2];
        } else {
            return (temp[(n - 1) / 2] + temp[n / 2]) / 2.0;
        }
    }

    double calculateMAD(const double arr[], int n, double median) {
        if (n <= 0) return 0.0;

        // Calculate absolute deviations from the median
        std::vector<double> deviations(n);
        for (int i = 0; i < n; ++i) {
            deviations[i] = std::fabs(arr[i] - median);
        }

        // MAD is the median of those deviations
        return calculateMedian(deviations.data(), n);
    }
}
