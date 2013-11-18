#ifndef STATE_H
#define STATE_H

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class State
{
    vector<int> board;
    int position;

public:

    State(int size, int position) :
        board(vector<int>(size, -1)),
        position(position)
    {
    }

    State(const State &state, int position, int value) :
        board(state.board),
        position(position)
    {
        board[position] = value;
    }

    State(int size, int *data)
    {
        board.reserve(size);

        int i = -1;
        while (++i < size)
            board.push_back(data[i]);

        position = data[i];
    }

    void serialize(int *data)
    {
        int i = -1;
        while (++i < board.size())
            data[i] = board[i];

        data[i] = position;
    }

    static int dataSize(int size)
    {
        return size + 1; // board + position
    }

    string str()
    {
        stringstream s;

        s << '[';

        for (int i = 0; i < board.size(); i++) {
            s << board[i] + 1;
            if (i < board.size() - 1)
                s << ',';
        }

        s << ']';

        return s.str();
    }

    bool isLeaf()
    {
        return position + 1 == board.size();
    }

    bool isValid()
    {
        for (int i = 0; i < position; i++)
            if (board[position] == board[i] || position - i == abs(board[position] - board[i]))
                return false;

        return true;
    }

    State getChild(int set_possition)
    {
        return State(*this, position + 1, set_possition);
    }

    void unpackToStack(vector<State> &stack, vector<bool> &fine_points)
    {
        for (int i = 0; i < board.size(); i++)
            if (fine_points.at((position + 1) * board.size() + i))
                stack.push_back(getChild(i));
    }
};

#endif // STATE_H
