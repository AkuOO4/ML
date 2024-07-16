#include <iostream>
#include <vector>
#include <thread>
#include <numeric> // for std::accumulate
#include <atomic>  // for std::atomic

// Function to compute the sum of elements in a partition of the array
void sumPartition(const std::vector<int>& arr, int start, int end, std::atomic<int>& global_sum) {
    int local_sum = 0;
    for (int i = start; i < end; ++i) {
        local_sum += arr[i];
    }
    global_sum += local_sum;
}

// Function to search for a key element in a partition of the array
void searchPartition(const std::vector<int>& arr, int start, int end, int key, std::atomic<bool>& found) {
    for (int i = start; i < end && !found.load(); ++i) {
        if (arr[i] == key) {
            found = true;
            return;
        }
    }
}

int main() {
    // Example array
    std::vector<int> arr = {1, 5, 3, 8, 2, 7, 4, 10, 6, 9};
    int num_threads = 4;
    int n = arr.size();
    int key = 7; // Element to search for

    // Sum computation
    std::atomic<int> global_sum(0);
    std::vector<std::thread> sum_threads;

    int part_size = (n + num_threads - 1) / num_threads; // ceil(n / num_threads)

    for (int i = 0; i < num_threads; ++i) {
        int start = i * part_size;
        int end = std::min(start + part_size, n);
        sum_threads.emplace_back(sumPartition, std::ref(arr), start, end, std::ref(global_sum));
    }

    for (auto& t : sum_threads) {
        t.join();
    }

    std::cout << "Sum of elements in the array: " << global_sum.load() << std::endl;

    // Search computation
    std::atomic<bool> found(false);
    std::vector<std::thread> search_threads;

    for (int i = 0; i < num_threads; ++i) {
        int start = i * part_size;
        int end = std::min(start + part_size, n);
        search_threads.emplace_back(searchPartition, std::ref(arr), start, end, key, std::ref(found));
    }

    for (auto& t : search_threads) {
        t.join();
    }

    if (found.load()) {
        std::cout << "Element " << key << " is found in the array." << std::endl;
    } else {
        std::cout << "Element " << key << " is not found in the array." << std::endl;
    }

    return 0;
}
