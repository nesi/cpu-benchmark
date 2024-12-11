#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <omp.h>
#include <thread>

const int MATRIX_SIZE = 2048;
const int NUM_ITERATIONS = 100;

// Function to get system information
void printSystemInfo() {
    int max_threads = omp_get_max_threads();
    int num_procs = std::thread::hardware_concurrency();
    
    std::cout << "System Information:" << std::endl;
    std::cout << "Maximum available threads: " << max_threads << std::endl;
    std::cout << "Hardware concurrency: " << num_procs << std::endl;
    std::cout << "Current threads being used: " << omp_get_num_threads() << std::endl;
    std::cout << "----------------------------------" << std::endl;
}

void initializeMatrix(std::vector<std::vector<double>>& matrix) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
}

void matrixMultiply(const std::vector<std::vector<double>>& A,
                    const std::vector<std::vector<double>>& B,
                    std::vector<std::vector<double>>& C) {
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            double sum = 0.0;
            for (int k = 0; k < MATRIX_SIZE; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

void runBenchmark(int num_threads) {
    std::cout << "\nRunning benchmark with " << num_threads << " threads" << std::endl;
    omp_set_num_threads(num_threads);
    
    std::vector<std::vector<double>> A(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));
    std::vector<std::vector<double>> B(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));
    std::vector<std::vector<double>> C(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));

    initializeMatrix(A);
    initializeMatrix(B);

    // Warm-up run
    matrixMultiply(A, B, C);

    // Measure performance
    double total_time = 0.0;
    double min_time = std::numeric_limits<double>::max();
    double max_time = 0.0;
    
    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        auto start = std::chrono::high_resolution_clock::now();
        matrixMultiply(A, B, C);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> diff = end - start;
        double current_time = diff.count();
        total_time += current_time;
        min_time = std::min(min_time, current_time);
        max_time = std::max(max_time, current_time);
    }

    double average_time = total_time / NUM_ITERATIONS;
    double gflops = (2.0 * MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE) / (average_time * 1e9);
    double variance = 0.0;

    std::cout << "Results for " << num_threads << " threads:" << std::endl;
    std::cout << "Matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE << std::endl;
    std::cout << "Average time: " << average_time << " seconds" << std::endl;
    std::cout << "Min time: " << min_time << " seconds" << std::endl;
    std::cout << "Max time: " << max_time << " seconds" << std::endl;
    std::cout << "Performance: " << gflops << " GFLOPS" << std::endl;
    std::cout << "----------------------------------" << std::endl;
}

int main() {
    printSystemInfo();
    
    // Get the number of physical cores (without SMT)
    int max_threads = omp_get_max_threads();
    int physical_cores = max_threads / 2; // Assumes 2-way SMT
    
    // Run tests with different thread counts
    std::cout << "\nRunning benchmarks with different thread configurations..." << std::endl;
    
    // Test with physical cores only (SMT disabled simulation)
    runBenchmark(physical_cores);
    
    // Test with all logical cores (SMT enabled)
    runBenchmark(max_threads);
    
    return 0;
}