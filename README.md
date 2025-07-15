# GameOfMPI
A simple MPI toy example to test trivially test some profiling and tracing tools.

Imagine something like Conways game of life, but not with the right rules.
We start with a random `MPI_Cart` grid, distribute it to a square number of processes.
For each point apply some random rules based on direct neighbors N, E, S, and W.
Then iterate through all the points and get neighbor values through an MPI boundary exchange.
Exchange happens mostly through `MPI_Sendrecv`.

This could implemetn Conways game of life, if we'd add the missing diagonal neighbors as well and apply the right rules.

## Build
```
# Make sure you have GCC, CMake and OpenMPI (tested with 4.1.6 and 5.0.7)
# In my case through modules:
module load 2025  GCC/14.2.0 CMake/3.31.3  OpenMPI/5.0.7 buildenv/default

# Out of source build, parallel to git clone
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo../gameofmpi
cmake --build .

# Start with square numbers of processes, e.g. 1, 4, 9, 16, 25, ...
mpirun -np 4 src/main/main
```

## Whats with the tests?
This repo is based on a C++ template I use with doctests, automatic linting, and a couple of cmake test targets.
No tests are implemented in this example though.
Linting is disabled in some places since I through this together quickly for the sake of doing useless MPI calculations.
