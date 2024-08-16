#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <mpi.h>

const int MATRIX_SIZE = 2048; // Adjust this based on your system's memory
const int NUM_ITERATIONS = 10;

// Function to initialize a matrix with random values
void initializeMatrix(std::vector<std::vector<double>>& matrix) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix[0].size(); ++j) {
            matrix[i][j] = dis(gen);
        }
    }
}

// Function to perform matrix multiplication
void matrixMultiply(const std::vector<std::vector<double>>& A,
                    const std::vector<std::vector<double>>& B,
                    std::vector<std::vector<double>>& C,
                    int start_row, int end_row) {
    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            double sum = 0.0;
            for (int k = 0; k < MATRIX_SIZE; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Calculate the number of rows each process will handle
    int rows_per_process = MATRIX_SIZE / world_size;
    int start_row = world_rank * rows_per_process;
    int end_row = (world_rank == world_size - 1) ? MATRIX_SIZE : start_row + rows_per_process;

    // Allocate matrices
    std::vector<std::vector<double>> A(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));
    std::vector<std::vector<double>> B(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));
    std::vector<std::vector<double>> C(MATRIX_SIZE, std::vector<double>(MATRIX_SIZE));

    // Initialize matrices on the root process
    if (world_rank == 0) {
        initializeMatrix(A);
        initializeMatrix(B);
    }

    // Broadcast matrices A and B to all processes
    MPI_Bcast(A.data()->data(), MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B.data()->data(), MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Warm-up run
    matrixMultiply(A, B, C, start_row, end_row);

    // Measure performance
    double total_time = 0.0;
    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        MPI_Barrier(MPI_COMM_WORLD);
        auto start = std::chrono::high_resolution_clock::now();
        
        matrixMultiply(A, B, C, start_row, end_row);
        
        MPI_Barrier(MPI_COMM_WORLD);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        total_time += diff.count();
    }

    // Gather results
    MPI_Gather(C[start_row].data(), rows_per_process * MATRIX_SIZE, MPI_DOUBLE,
               C.data()->data(), rows_per_process * MATRIX_SIZE, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        double average_time = total_time / NUM_ITERATIONS;
        double gflops = (2.0 * MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE) / (average_time * 1e9);

        std::cout << "Matrix size: " << MATRIX_SIZE << "x" << MATRIX_SIZE << std::endl;
        std::cout << "Number of processes: " << world_size << std::endl;
        std::cout << "Average time per multiplication: " << average_time << " seconds" << std::endl;
        std::cout << "Performance: " << gflops << " GFLOPS" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
