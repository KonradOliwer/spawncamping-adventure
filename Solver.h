#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <fstream>
#include <string>

#include "State.h"

using namespace std;

class Solver
{
protected:

    const string FILE_NAME;

    int size;
    vector<bool> fine_points;
    vector<State> stack;

public:

    Solver(const string &file_name) : FILE_NAME(file_name)
    {
        ifstream f(file_name.c_str());

        string content;

        f >> content;
        f >> size;
        f >> content;

        fine_points = vector<bool>(size * size, true);

        while (f.good()) {
            int x, y;
            f >> x >> y;
            fine_points[((x - 1) * size + (y - 1))] = false;
        }
    }

    virtual bool solve()
    {
        stack.push_back(State(size, -1));

        while (stack.size() > 0) {
            State current = stack.back();
            stack.pop_back();

            if (current.isValid()) {
                if (!current.isLeaf()) {
                    current.unpackToStack(stack, fine_points);
                } else {
                    current.print();
                    return true;
                }
            }
        }

        cout << "There is no possible solution for " << size << "-queen problem defined in: "
             << FILE_NAME << endl;

        return false;
    }
};

#endif // SOLVER_H
