#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <omp.h>

const int MATRIX_SIZE = 2048; // Adjust this based on your system's memory
const int NUM_ITERATIONS = 100;

// Function to initialize a matrix with random values
void initializeMatrix(std::vector<std::vector<double>>& matrix) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
}

// Function to perform matrix multiplication
void matrixMultiply(const std::vector<std::vector<double>>& A,
                    const std::vector<std::vector<double>>& B,
                    std::vector<std::vector<double>>& C) {
    #pragma omp parallel for collapse(2)
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

int main() {
    std::vector<std::vector<double>> A(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));
    std::vector<std::vector<double>> B(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));
    std::vector<std::vector<double>> C(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));

    initializeMatrix(A);
    initializeMatrix(B);

    // Warm-up run
    matrixMultiply(A, B, C);

    // Measure performance
    double total_time = 0.0;
    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        auto start = std::chrono::high_resolution_clock::now();
        
        matrixMultiply(A, B, C);
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        total_time += diff.count();
    }

    double average_time = total_time / NUM_ITERATIONS;
    double gflops = (2.0 * MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE) / (average_time * 1e9);

    std::cout << "Matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE << std::endl;
    std::cout << "Average time per multiplication: " << average_time << " seconds" << std::endl;
    std::cout << "Performance: " << gflops << " GFLOPS" << std::endl;

    return 0;
}
