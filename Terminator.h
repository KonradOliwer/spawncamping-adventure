#ifndef TERMINATOR_H
#define TERMINATOR_H

#include "sgr.h"

class Terminator
{
    static const int TOKEN_TAG = 1991;
    static const int DEATH_TAG = 1992;

    enum Color
    {
        NONE,
        WHITE,
        BLACK
    };

    const int rank;
    const int comm_size;

    int color;
    int token;

    int token_buf;
    MPI_Request token_request;

    bool token_initialized;
    bool finished;

    void passToken()
    {
        // Determine the detination
        const int successor = (rank+1) % comm_size;

        cout << sgr("35") << rank << ": Passing token to " << successor << sgr() << endl;

        // Clean up after the previous transmission
        if (token_buf != -1)
            MPI_Wait(&token_request, MPI_STATUS_IGNORE);

        token_buf = token;
        token = NONE;
        MPI_Isend(&token_buf, 1, MPI_INT, successor, TOKEN_TAG, MPI_COMM_WORLD, &token_request);

        color = WHITE;
    }

public:

    Terminator(int rank, int comm_size) :
        rank(rank),
        comm_size(comm_size),
        token_buf(-1),
        token_initialized(false),
        finished(false)
    {
        cout << sgr("36") << rank << ": Terminator initialzed" << sgr() << endl;
    }

    // Tell everyone to die, blocking
    void broadcastDeath()
    {
        cout << sgr("35") << rank << ": Telling everone to die" << sgr() << endl;

        finished = true;

        MPI_Request request;

        for (int i = 0; i < comm_size; i++)
            MPI_Send(NULL, 0, MPI_INT, i, DEATH_TAG, MPI_COMM_WORLD);

        // TODO: Nonblocking send?
    }

    // Check for the termination message
    bool checkDeath()
    {
        int msg_flag;
        MPI_Status status;

        MPI_Iprobe(MPI_ANY_SOURCE, DEATH_TAG, MPI_COMM_WORLD, &msg_flag, &status);

        if (msg_flag) {
            MPI_Recv(NULL, 0, MPI_INT, status.MPI_SOURCE, DEATH_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            finished = true;
        }

        return finished;
    }

    // Call whenever sharing work
    void onWorkSent(int dst)
    {
        if (dst < rank) {
            cout << sgr("35") << rank << ": " << "Work sent, I am black now" << endl;

            color = BLACK;
        }

        cout << sgr("35") << rank << ": " << "Work sent, no change in color" << sgr() << endl;
    }

    // Call whenever unable to get any more work
    bool onIdle()
    {
        if (finished) {
            cout << rank << ": Idle and finished" << endl;

            return false;
        } else {
            cout << rank << ": Idle and waiting" << endl;

            if (rank != 0 && checkDeath())
                return false;

            if (rank == 0 && !token_initialized) {
                // Initialize the token
                color = WHITE;
                token = WHITE;
                token_initialized = true;
            } else {
                // Determine the possible sender
                const int predecessor = (rank-1 + comm_size) % comm_size;

                // Check whether the token is available
                int msg_flag;
                MPI_Iprobe(predecessor, TOKEN_TAG, MPI_COMM_WORLD, &msg_flag, MPI_STATUS_IGNORE);

                if (msg_flag) {
                    // Receive the token
                    MPI_Recv(&token, 1, MPI_INT, predecessor, TOKEN_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Special handling for the root node
                    if (rank == 0) {
                        // Check the termination condition
                        if (token == WHITE) {
                            broadcastDeath();
                            return false;
                        } else {
                            token = WHITE;
                        }
                    }
                }
            }

            if (token) {
                if (color == BLACK)
                    token = BLACK;

                passToken();
            }

            return true;
        }
    }
};

#endif // TERMINATOR_H
