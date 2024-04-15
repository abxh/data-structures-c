// references:
// https://github.com/tsoding/rust-hash-table/blob/main/src/main.rs
// https://cplusplus.com/reference/unordered_map/unordered_map/find/

#include <cstdint> // uint64_t
#include <cstdlib> // rand, srand
#include <ctime>
#include <iostream>
#include <unordered_map>

extern "C" {
#define PREFIX uint_ht
#define KEY_TYPE uint64_t
#define VALUE_TYPE uint64_t
#include "fixed-containers/K_to_V_hashtable.h" // uint_ht

void benchmark_uint_ht(size_t n, unsigned int seed) {
    uint_ht_type* ht_p;
    uint_ht_init(&ht_p, n);
    srand(seed);
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

void benchmark_std_unordered_map(size_t n, unsigned int seed) {
    std::unordered_map<uint64_t, uint64_t> map;
    srand(seed);
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
    for (size_t N = 10; N < 100000000; N *= 10) {
        std::clock_t c_start1 = std::clock();
        benchmark_uint_ht(N, N);
        std::clock_t c_end1 = std::clock();

        std::clock_t c_start2 = std::clock();
        benchmark_std_unordered_map(N, N);
        std::clock_t c_end2 = std::clock();

        std::cout << "time elapsed for " << N << " elements:" << std::endl;
        std::cout << " custom hashtable: " << (c_end1 - c_start1) / 1000.0 << "s" << std::endl;
        std::cout << " c++ unordered map: " << (c_end2 - c_start2) / 1000.0 << "s" << std::endl;
    }

    return 0;
}
