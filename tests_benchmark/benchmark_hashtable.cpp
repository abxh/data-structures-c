// references:
// https://github.com/tsoding/rust-hash-table/blob/main/src/main.rs
// https://cplusplus.com/reference/unordered_map/unordered_map/find/

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <unordered_map>

extern "C" {
#define PREFIX uint_ht
#define KEY_TYPE uint64_t
#define VALUE_TYPE uint64_t
#include "fixed-containers/K_to_V_hashtable.h"

void benchmark_uint_ht(size_t n) {
    uint_ht_type* ht_p;
    uint_ht_init(&ht_p, n);
    for (size_t i = 0; i < n; i++) {
        uint64_t key = rand() & 1;
        uint64_t* value_p = uint_ht_get_mut(ht_p, key);
        if (value_p == NULL) {
            uint_ht_set(ht_p, key, 1);
        } else {
            *value_p = *value_p + 1;
        }
    }
}
}

void benchmark_std_unordered_map(size_t n) {
    std::unordered_map<uint64_t, uint64_t> map;
    for (size_t i = 0; i < n; i++) {
        uint64_t key = rand();
        std::unordered_map<uint64_t, uint64_t>::iterator res_iterator = map.find(key);
        if (res_iterator == map.end()) {
            map.insert_or_assign(key, 1);
        } else {
            res_iterator->second = res_iterator->second + 1;
        }
    }
}

int main(void) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::microseconds;

    for (size_t N = 10; N < 10000000; N *= 10) {
        srand(N);
        auto c_start1 = high_resolution_clock::now();
        benchmark_uint_ht(N);
        auto c_end1 = high_resolution_clock::now();

        srand(N);
        auto c_start2 = high_resolution_clock::now();
        benchmark_std_unordered_map(N);
        auto c_end2 = high_resolution_clock::now();

        std::cout << "time elapsed for " << N << " elements:" << std::endl;
        std::cout << " custom hashtable: " << duration_cast<microseconds>(c_end1 - c_start1).count() << " μs" << std::endl;
        std::cout << " c++ unordered map: " << duration_cast<microseconds>(c_end2 - c_start2).count() << " μs" << std::endl;
    }

    return 0;
}
