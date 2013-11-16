#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <bitset>
#include <iostream>

using namespace std;

struct Point {
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) {
    }

    void print() {
        cout << "Point(" << x << "," << y << ")" << endl;
    }
};

class State {
    vector<int> board;
    int position;

public:

    State(int size, int position) : board(vector<int>(size, -1)), position(position) {
    }

    State(const State &state, int position, int value) : board(state.board), position(position) {
        board[position] = value;
    }

    void print() {
        cout << "[";
        for (int i = 0; i < board.size(); i++) {
            cout << board[i] + 1;
            if (i < board.size() - 1) {
                cout << ",";
            }
        }
        cout << "]" << endl;
    }

    bool is_last() {
        return position + 1 == board.size();
    }

    bool is_valid() {
        for (int i = 0; i < position; i++) {
            if (!(board[position] != board[i]
                    && abs(position - i) != abs(board[position] - board[i]))) {
                return false;
            }
        }
        return true;
    }

    State getChild(int set_possition) {
        State child(*this, position + 1, set_possition);
        return child;
    }

    void unpackToStack(vector<State> &stack, vector<bool> &fine_points) {
        for (int i = 0; i < board.size(); i++) {
            if (fine_points.at((position + 1) * board.size() + i)) {
                stack.push_back(getChild(i));
            }
        }
    }
};

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
