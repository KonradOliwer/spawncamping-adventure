#ifndef PARALLELSOLVER_H
#define PARALLELSOLVER_H

#include <cstdlib>
#include <unistd.h>

#include <mpi.h>

#include "sgr.h"

#include "Solver.h"
#include "Terminator.h"

class ParallelSolver : public Solver
{
    static const int WORK_REQUEST_TAG = 1;
    static const int WORK_RESPONSE_TAG = 2;

    int rank;
    int comm_size;

    Terminator *terminator;

    int choosePeer()
    {
        return (rank + 1 + rand() % (comm_size-1)) % comm_size;
    }

    // Handle one incoming request, if there are any
    void handleWorkRequest()
    {
        cout << sgr("34") << rank << ": Handling requests" << sgr() << endl;

        int msg_flag;
        MPI_Status status;

        MPI_Iprobe(MPI_ANY_SOURCE, WORK_REQUEST_TAG, MPI_COMM_WORLD, &msg_flag, &status);

        // Check whether there is a pending requst
        if (msg_flag) {
            cout << rank << ": Receiving request from " << status.MPI_SOURCE << ", stack size is " << stack.size() << endl;

            // Acknowledge the request
            MPI_Recv(NULL, 0, MPI_INT, status.MPI_SOURCE, WORK_REQUEST_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Check if there is enough work to share
            if (stack.size() > 1) {
                // Calculate various sizes used during the transfer
                const int state_count = stack.size() / 2; // Send half of the available work
                const int data_size = State::dataSize(size);
                const int send_size = state_count * data_size;

                // Allocate space for serialization
                int *send_buf = new int[send_size];

                // Serialize and discard the required number of states
                for (int *data = send_buf, i = 0; i < state_count; i++) {
                    stack.back().serialize(data);
                    stack.pop_back();
                    data += data_size;
                }

                cout << rank << ": Sending work (" << state_count << ") to " << status.MPI_SOURCE << endl;

                // Respond with some serialized work
                MPI_Send(send_buf, send_size, MPI_INT, status.MPI_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD);

                // Notify the terminator about the situation
                terminator->onWorkSent(status.MPI_SOURCE);

                // Clean up
                delete[] send_buf;
            } else {
                cout << rank << ": Sending negative response to " << status.MPI_SOURCE << endl;

                // Not enough work, send a negative response
                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, WORK_RESPONSE_TAG, MPI_COMM_WORLD);
            }
        }
    }

    // Ask someone for more work
    bool requestWork()
    {
        // Choose someone to cummunicate with
        const int peer = choosePeer();

        cout << sgr("33") << rank << ": Asking " << peer << " for work" << sgr() << endl;

        // Send a request for work
        MPI_Request request;
        MPI_Isend(NULL, 0, MPI_INT, peer, WORK_REQUEST_TAG, MPI_COMM_WORLD, &request);

        // Used for probing
        int msg_flag;
        MPI_Status status;

        while (true) {
            cout << rank << ": Waiting for response from " << peer << endl;

            // There is a chance that the peer died and will not respond
            if (terminator->checkDeath())
                return false;

            // Check whether the response has arrived
            MPI_Iprobe(peer, WORK_RESPONSE_TAG, MPI_COMM_WORLD, &msg_flag, &status);

            // Act accordingly to the progress with the response
            if (msg_flag) {
                // Check the length of the response
                int recv_size;
                MPI_Get_count(&status, MPI_INT, &recv_size);

                // Non-empty response means some work has been sent
                if (recv_size) {
                    // Receive the work in serialized form
                    int *recv_buf = new int[recv_size];
                    MPI_Recv(recv_buf, recv_size, MPI_INT, peer, WORK_RESPONSE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Allocate space for deserialized work
                    const int data_size = State::dataSize(size);
                    const int state_count = recv_size / data_size;
                    stack.reserve(state_count);

                    cout << sgr("32") << rank << ": Positive response from " << peer << " (" << state_count << ")" << sgr() << endl;

                    // Deserialize the work
                    for (int *data = recv_buf, i = 0; i < state_count; i++) {
                        stack.push_back(State(size, data));
                        data += data_size;
                    }

                    // Clean up
                    delete[] recv_buf;
                    MPI_Wait(&request, MPI_STATUS_IGNORE);

                    // Notify about the success
                    return true;
                } else {
                    cout << sgr("31") << rank << ": Negative response from " << peer << sgr() << endl;

                    // Clean up
                    MPI_Wait(&request, MPI_STATUS_IGNORE);

                    // Notify about the failure
                    return false;
                }
            }

            // Do not block other nodes, respond to their requests
            handleWorkRequest();

            /*sleep(1);*/
        }
    }

    void processWork()
    {
        cout << sgr("34") << rank << ": Working" << sgr() << endl;

        /* Dummy work *********************************************************/

        /*stack.pop_back();*/

        /* Real work **********************************************************/

        State current = stack.back();
        stack.pop_back();

        if (current.isValid()) {
            if (current.isLeaf()) {
                cout << sgr("32") << "Solution: " << current.str() << sgr() << endl;
                terminator->broadcastDeath();
            } else {
                current.unpackToStack(stack, fine_points);
            }
        }
    }

    void mainLoop()
    {
        while (true) {
            cout << sgr("35") << rank << ": I have " << stack.size() << " tasks" << sgr() << endl;

            if (stack.empty()) {
                bool more_work = requestWork();

                bool death = !more_work && !terminator->onIdle();

                if (death)
                    return;
            } else {
                // Do some work
                processWork();
            }

            // Process requests from other nodes
            handleWorkRequest();

            /*sleep(1);*/
        }
    }

public:

    ParallelSolver(const string &file_name) : Solver(file_name)
    {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
        srand(time(NULL) + rank*123);

        terminator = new Terminator(rank, comm_size);
    }

    ~ParallelSolver()
    {
        delete terminator;
    }

    bool solve()
    {
        cout << sgr("36") << rank << ": Parallel dummy starting " << sgr() << endl;

        // Push some dummy states
        if (rank == 0) {
            /*for (int i = 0; i < 40; i++) {
                stack.push_back(State(size, -1));
            }*/

            stack.push_back(State(size, -1));
        }

        mainLoop();

        cout << sgr("36") << rank << ": Parallel dummy finished" << sgr() << endl;
    }
};

#endif // PARALLELSOLVER_H
