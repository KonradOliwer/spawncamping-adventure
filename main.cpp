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

    State(int size, int position) : board(vector<int>(size, 0)), position(position) {
    }

    bool validate() {
        for (int i = 0; i < position; i++) {
            if (!board[position] != board[i]
                    && abs(position - i) != abs(board[position] - board[i])) {
                return false;
            }
        }
        return true;
    }

    void unpack() {

    }
};

void read_file(string &file_path, vector<Point> &banned_points, int &size);

int main(int argc, char** argv) {
    string FILE_NAME = "queen4";
    vector<Point> banned_points;
    vector<Point> stack;
    int size;

    read_file(FILE_NAME, banned_points, size);

    stack.push_back(State(size, -1));

    while (stack.size() > 0) {
        State current = stack.back();
        stack.pop_back();

    }

    return 0;
}

void read_file(string &file_name, vector<Point> &banned_points, int &size) {
    string content;
    Point point;

    ifstream f(file_name.c_str());
    f >> content;
    f >> size;
    f >> content;
    while (!f.eof()) {
        f >> point.x >> point.y;
        banned_points.push_back(point);
    }
}