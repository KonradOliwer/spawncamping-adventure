#ifndef PARALLELSOLVER_H
#define PARALLELSOLVER_H

#include <cstdlib>

#include <mpi.h>

#include "Solver.h"

class ParallelSolver : public Solver {
    static const int WORK_REQUEST_TAG = 1;
    static const int WORK_RESPONSE_TAG = 2;

    int rank;
    int comm_size;

    int choosePeer() {
        return (rank + 1 + rand() % (comm_size-1)) % comm_size;
    }

    void handleRequest() {
        MPI_Status status;
        int msg_flag;

        MPI_Iprobe(MPI_ANY_SOURCE, WORK_REQUEST_TAG, MPI_COMM_WORLD, &msg_flag, &status);
        if (msg_flag) {
            cout << rank << ": Received request, stack size is " << stack.size() << endl;

            MPI_Recv(NULL, 0, MPI_INT, status.MPI_SOURCE, WORK_REQUEST_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (stack.size() > 1) {
                const int state_count = stack.size() / 2; // Send half of the available work
                const int data_size = State::dataSize(size);
                const int send_size = state_count * data_size;
                int *send_buf = new int[send_size];

                // Serialize and discard the required number of states
                for (int *data = send_buf, i = 0; i < state_count; i++) {
                    stack.back().serialize(data);
                    stack.pop_back();
                    data += data_size;
                }

                cout << rank << ": Sending work (" << state_count << ") to " << status.MPI_SOURCE << endl;
                MPI_Send(send_buf, send_size, MPI_INT, status.MPI_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD);

                delete[] send_buf;
            } else {
                // Negative response
                cout << rank << ": Sending negative response to " << status.MPI_SOURCE << endl;
                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD);
            }
        }
    }

    bool requestWork() {
        // Choose a peer and send a request
        const int peer = choosePeer();
        MPI_Request request;
        MPI_Isend(NULL, 0, MPI_INT, peer, WORK_REQUEST_TAG, MPI_COMM_WORLD, &request);

        cout << rank << ": Asking " << peer << " for work" << endl;

        while (true) {
            MPI_Status status;
            int msg_flag;
            int response;

            // Handle the response if it has arrived
            MPI_Iprobe(peer, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &msg_flag, &status);
            if (msg_flag) {
                int recv_size;
                MPI_Get_count(&status, MPI_INT, &recv_size);

                if (recv_size) {
                    // Receive work in raw form
                    int *recv_buf = new int[recv_size];
                    MPI_Recv(recv_buf, recv_size, MPI_INT, peer, WORK_RESPONSE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Allocate space for deserialized work
                    const int data_size = State::dataSize(size);
                    const int state_count = recv_size / data_size;
                    stack.reserve(state_count);

                    cout << rank << ": Positive response from " << peer << " (" << state_count << ")" << endl;

                    // Deserialize work
                    for (int *data = recv_buf, i = 0; i < state_count; i++) {
                        stack.push_back(State(size, data));
                        data += data_size;
                    }

                    delete[] recv_buf;

                    return true;
                } else {
                    cout << rank << ": Negative response from " << peer << endl;

                    // No luck, try again?
                    return false;
                }
            }

            handleRequest();
        }
    }

public:
    ParallelSolver(const string &file_name) : Solver(file_name) {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
        srand(time(NULL) + rank*123);
    }

    bool solve() {
        cout << rank << ": Parallel dummy starting "<< endl;

        // WARNING: This is just a test, works only with 2 processes
        if (rank == 0) {
            for (int i = 0; i < 4; i++) {
                stack.push_back(State(size, -1));
            }

            while (stack.size() > 1) {
                handleRequest();
            }

            // Wait to handle one last request from the other process
            MPI_Probe(MPI_ANY_SOURCE, WORK_REQUEST_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            handleRequest();
        } else {
            while (requestWork());
        }

        cout << rank << ": Parallel dummy finished" << endl;
    }
};

#endif // PARALLELSOLVER_H
