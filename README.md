# cpu-benchmark


* Defines two large matrices (A and B) and fills them with random double-precision floating-point numbers.
* Performs matrix multiplication `C = A * B`.
* Uses OpenMP to parallelize the computation, which will take advantage of multiple cores in Genoa/Milan CPUs.
* Repeats the multiplication several times and measures the average execution time.
* Calculates and reports the performance in GFLOPS (Giga Floating-Point Operations Per Second).

### Compilation


* Adjust the `MATRIX_SIZE` constant based on system's memory. Larger sizes will stress the memory subsystem more.
* Make sure to compile with optimizations enabled (`-O3` flag).
* If you want to test specific instruction sets, you can add flags like -march=znver3 for Zen 3 (Milan) or -march=znver4 for Zen 4 (Genoa).

#### Compiling the `openmp` version
```bash
g++ -O3 -fopenmp cpu-benchmark-openmp.cpp -o cpu_benchmark
```
#### Compiling the `mpi` version

```bash
mpic++ -O3 cpu-benchmark-mpi.cpp -o mpi_cpu_benchmark
```
