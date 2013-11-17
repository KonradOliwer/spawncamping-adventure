#ifndef PARALLELSOLVER_H
#define PARALLELSOLVER_H

#include "Solver.h"

class ParallelSolver : public Solver {

public:
    ParallelSolver(const string &file_name) : Solver(file_name) {
    }

    bool solve() {
        cout << "Parallel dummy" << endl;
    }
};

#endif // PARALLELSOLVER_H
