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

        while (true) {
            int x, y;
            f >> x >> y;
            if (f.fail()) {
                break;
            } else {
                fine_points[((x - 1) * size + (y - 1))] = false;
            }
        }
    }

    int getSize()
    {
        return size;
    }

    virtual bool solve()
    {
        stack.push_back(State(size, -1));

        while (stack.size() > 0) {
            State current = stack.back();
            stack.pop_back();

            if (current.isValid()) {
                if (current.isLeaf()) {
#ifndef QUIET
                    cout << sgr("32")  << 0 << ": Solution: " << current.str() << sgr() << endl;
#endif
                    return true;
                } else {
                    current.unpackToStack(stack, fine_points);
                }
            }
        }
#ifndef QUIET
        cout << sgr("32") << 0 << ": Solution: n/a" << sgr() << endl;
#endif

        return false;
    }
};

#endif // SOLVER_H
