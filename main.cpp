#include <iostream>
#include <string>
#include <vector>

#include <mpi.h>

#include "ParallelSolver.h"

using namespace std;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    const string file_name = argc > 1 ? argv[1] : "queen6-bad";

    Solver *solver = comm_size > 1 ? new ParallelSolver(file_name)
                                   : new Solver(file_name);

    MPI_Barrier(MPI_COMM_WORLD);
    double startStamp = MPI_Wtime();

    solver->solve();

    MPI_Barrier(MPI_COMM_WORLD);
    double endStamp = MPI_Wtime();

    if (rank == 0)
        cout << "s=" << solver->getSize() << " n=" << comm_size << " t=" << (endStamp-startStamp) << endl;

    MPI_Finalize();
    delete solver;
    return 0;
}
