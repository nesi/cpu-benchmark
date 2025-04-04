#!/bin/bash -e 

echo "Starting stress test on $(hostname) at $(date)"
echo "Allocated resources: 80 CPUs, 1.3TB memory"

# Load any required modules
# module load ...

# Create a directory for temporary files
TEMPDIR=$(mktemp -d)
cd $TEMPDIR

# Function to clean up on exit
cleanup() {
    echo "Cleaning up temporary files..."
    rm -rf $TEMPDIR
    echo "Stress test completed or interrupted at $(date)"
}
trap cleanup EXIT

# CPU stress test using stress-ng (install if not available)
# Using different stressors to exercise various parts of the CPU
echo "Starting CPU stress test..."
stress-ng --cpu 80 --cpu-method all --metrics-brief &
CPU_STRESS_PID=$!

# Memory stress test using a combination of tools
echo "Starting memory stress test..."

# Calculate memory to use (slightly less than total to avoid OOM killer)
# Using 95% of the allocated memory
MEM_BYTES=$(( 1300 * 1024 * 1024 * 1024 * 95 / 100 ))
MEM_GB=$(( MEM_BYTES / 1024 / 1024 / 1024 ))
echo "Will allocate approximately ${MEM_GB}GB of memory"

# Option 1: Using stress-ng for memory stress
stress-ng --vm 40 --vm-bytes $(( MEM_BYTES / 40 ))B --vm-method all --metrics-brief &
MEM_STRESS_PID=$!