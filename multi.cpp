#include <cstdlib>
#include <mpi.h>

using namespace std;

static const int NO_TOKEN = 0;
static const int WHITE = 1;
static const int BLACK = 2;
static const int TOKEN_MSG = 1991;

//**********************************************************************************************
// Private! Don't call it:
void reciveToken(int source, int &token, MPI_Comm comm, MPI_Status status);
void passToeken(int destiny, int color, int &token, MPI_Comm comm);

// use always when process recive work:
void reciveWorkEvent(int myrank, int source, int &color);

// use always when process turn inactive:
void serveToken(int &color, int &token, int myrank, int ring_size, MPI_Comm comm, MPI_Status status,
        bool &fail);
//**********************************************************************************************

void reciveWorkEvent(int myrank, int source, int &color) {
    if (source > myrank) {
        color = BLACK;
    }
}

void passToeken(int destiny, int color, int &token, MPI_Comm comm) {
    if (color = BLACK) {
        token = BLACK;
    }
    MPI_Isend(&token, 1, MPI_INT, destiny, TOKEN_MSG, comm); //blocking
    token = NO_TOKEN;
    color = WHITE;
}

void reciveToken(int source, int &token, MPI_Comm comm, MPI_Status status) {
    MPI_Request request;
    int flag = MPI_Probe(source, TOKEN_MSG, comm, &status);
    if (flag) {
        MPI_Recv(&token, 1, MPI_INT, int source, int TOKEN_MSG, &status); //blocking
    }
}

void serveToken(int &color, int &token, int myrank, int ring_size, MPI_Comm comm, MPI_Status status, bool &fail) {
    if (myrank == 0) {
        color = WHITE;
    }
    if (token != NO_TOKEN) {
        passToeken((myrank + 1) % ring_size, &color, &token, comm);
    } else {
        reciveToken(myrank % ring_size, &token, comm, status);
        fail = myrank == 0 && token == WHITE; //should imply finalizing program with fail message
    }
}

int main(int argc, char** argv) {
    int myrank;
    int token;
    int color = WHITE;
    if (myrank == 0) {
        token = WHITE;
    } else {
        token = NO_TOKEN;
    }
    //TODO
    return 0;
}