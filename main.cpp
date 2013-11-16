#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "State.h"

using namespace std;

vector<bool> read_file(string &file_path, int &size);

int main(int argc, char** argv) {
    string FILE_NAME = "queen4";
    vector<State> stack;
    int size;

    vector<bool> fine_points = read_file(FILE_NAME, size);

    stack.push_back(State(size, -1));
//    cout << "debug: stack initialized" << endl;
    while (stack.size() > 0) {
        State current = stack.back();
        stack.pop_back();
        if (current.is_valid()) {
            if (!current.is_last()) {
                current.unpackToStack(stack, fine_points);
            } else {
                current.print();
                return 0;
            }
        }
    }

    cout << "There is no possible solution for " << size << "-queen problem defined in: "
         << FILE_NAME << endl;
    return 0;
}

vector<bool> read_file(string &file_name, int &size) {
    string content;
    int x;
    int y;

    ifstream f(file_name.c_str());
    f >> content;
    f >> size;
    f >> content;

    vector<bool> fine_points(size * size, true);
    while (!f.eof()) {
        f >> x;
        f >> y;
        fine_points[((x - 1) * size + (y - 1))] = false;
//        cout << "debug: beaned x=" << x << ", y=" << y << endl;
    }
//    cout << "debug: file red" << endl;
    return fine_points;
}
