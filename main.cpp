#include <iostream>
#include <chrono>
#include <random>
#include <cmath>
#include "tdl.h"

// Computational constants
constexpr std::size_t data_size = 100000;
constexpr std::size_t random_range = 100000000000;

template <class iterator, class function>
void parallel_for(iterator begin, iterator end, function f) {
    // Calculating range division parameters
    std::size_t range = (end - begin);
    std::size_t partitions = tdl::get_worker_count();
    std::size_t minimum_size = range / partitions;
    std::size_t excess = range % partitions;

    // Creating partitions
    for (std::size_t i = 0; i < partitions; i++) {
        // Calculating current partition size
        std::size_t current_size = minimum_size;
        if (excess) { current_size++; excess--; }

        // Submitting current range calculation
        tdl::spawn(tdl::discards([=](){
            std::for_each(begin, begin + current_size, f);
        }));

        // Moving begin iterator to next range
        begin += current_size;
    }
}

using namespace std::chrono;

int main() {

    /*****************************************************************
     * Parallelism is much more effective when Tasks are relatively
     * longer. With extremely short Tasks, cache-coherence, locality
     * of reference and administration overhead of parallel computing
     * can make a serial approach faster.
     *
     * The following code calculates a given number of random values,
     * then finds their square roots. This is a relatively short time,
     * so the serial part finishes faster. If we simulate a longer
     * execution time, the parallel part becomes better, due to the
     * overhead becoming negligible.
     *
     * Enable one of the following defines to test performance.
     * Experiment with the iteration count, the equilibrium between
     * the serial and parallel solution should be around 100.
     ****************************************************************/

    // #define SIMULATE_LONG_EXECUTION
    #define SIMULATE_LONG_EXECUTION for(int i=0; i<70;i++)

    /*****************************************************************
     * SERIAL COMPUTATION:
     ****************************************************************/

    // Initialization & setup
    std::srand(std::time(0));

    // Serial data arrays
    double array_serial[data_size] = {0};

    high_resolution_clock::time_point serial_start = high_resolution_clock::now();

    // Serially generating random values
    for (std::size_t i = 0; i < data_size; i++) {
        array_serial[i] = std::rand() % random_range;
        SIMULATE_LONG_EXECUTION;
    }

    // Serially calculating the roots
    for (std::size_t i = 0; i < data_size; i++) {
        array_serial[i] = std::sqrt(array_serial[i]);
        SIMULATE_LONG_EXECUTION;
    }

    high_resolution_clock::time_point serial_end = high_resolution_clock::now();
    auto serial_elapsed = duration_cast<microseconds>(serial_end - serial_start).count();

    std::cout << "Serial   execution time: " << serial_elapsed << " us" << std::endl;

    /*****************************************************************
     * PARALLEL COMPUTATION:
     ****************************************************************/

    // Parallel data arrays
    double array_parallel[data_size] = {0};

    // Initialization
    tdl::initialize();

    // Parallel generation of random values
    auto random_filler = tdl::discards([&](){
        parallel_for(std::begin(array_parallel), std::end(array_parallel), [&](double &value) {
            value = 1 + std::rand() % random_range;
            SIMULATE_LONG_EXECUTION;
        });
    });

    // Parallel calculation of the roots
    auto root_finder = tdl::discards([&](){
        parallel_for(std::begin(array_parallel), std::end(array_parallel), [](double &value) {
            value = std::sqrt(value);
            SIMULATE_LONG_EXECUTION;
        });
    });

    // Setting up dependencies
    random_filler->set_continuation(root_finder);

    high_resolution_clock::time_point parallel_start = high_resolution_clock::now();

    // Execute computation
    tdl::submit(random_filler);
    root_finder->wait();

    high_resolution_clock::time_point parallel_end = high_resolution_clock::now();
    auto parallel_elapsed = duration_cast<microseconds>(parallel_end - parallel_start).count();

    std::cout << "Parallel execution time: " << parallel_elapsed << " us." << std::endl;

    tdl::shutdown();
    return 0;
}
