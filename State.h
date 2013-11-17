#ifndef STATE_H
#define STATE_H

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

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
            if (board[position] == board[i] || position - i == abs(board[position] - board[i])) {
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

#endif // STATE_H
