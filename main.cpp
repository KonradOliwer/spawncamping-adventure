#include <string>
#include <vector>

#include <mpi.h>

#include "ParallelSolver.h"

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    const string FILE_NAME = argc > 1 ? argv[1] : "queen4";

    comm_size > 1 ? ParallelSolver(FILE_NAME).solve()
                  : Solver(FILE_NAME).solve();

    MPI_Finalize();
    return 0;
}
