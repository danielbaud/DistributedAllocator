#include "funcs.hh"

using namespace std;

bool help()
{
    cout << "Available commands:" << endl;
    cout << "\talloc -f <file> | <string>\tAllocate either a file in memory or the string <string>&" << endl;
    cout << "\tread <chunk>\t\t\tReads the chunk <chunk>" << endl;
    cout << "\tlist\t\t\t\tShows a list of the processes and their allocated memory" << endl;
    cout << "\tkill <process>\t\t\tKills the process <process>" << endl;
    cout << "\thelp\t\t\t\tDisplays this message" << endl;
    return true;
}

bool alloc(int master, int rank, int size)
{
    if (master == rank)
        send_all(master, "alloc", size);
    cout << rank << ": alloc" << endl;
    if (master == rank)
        receive_all_end(master, size);
    else
        MPI_Send("end", 4, MPI_CHAR, master, 0, MPI_COMM_WORLD);
    return true;
}

bool read(int master, int rank, int size)
{
    if (master == rank)
        send_all(master, "read", size);
    cout << rank << ": read" << endl;
    if (master == rank)
        receive_all_end(master, size);
    else
        MPI_Send("end", 4, MPI_CHAR, master, 0, MPI_COMM_WORLD);
    return true;
}

bool list(int master, int rank, int size)
{
    if (master == rank)
        send_all(master, "list", size);
    cout << "Process " << rank << " active" << endl;
    if (master == rank)
        receive_all_end(master, size);
    else
        MPI_Send("end", 4, MPI_CHAR, master, 0, MPI_COMM_WORLD);
    return true;
}

bool kill(int master, int rank, int size)
{
    if (master == rank)
        send_all(master, "kill", size);
    cout << rank << ": kill" << endl;
    if (master == rank)
        receive_all_end(master, size);
    else
        MPI_Send("end", 4, MPI_CHAR, master, 0, MPI_COMM_WORLD);
    return true;
}

void send_all(int master, const char *message, int size)
{
    for (unsigned i = 0; i < size; ++i)
        if (i != master)
            MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
}

void receive_all_end(int master, int size)
{
    char buffer[6];
    for (unsigned i = 0; i < size; ++i)
    {
        if (i != master)
        {
            MPI_Recv(buffer, 6, MPI_CHAR, i, 0, MPI_COMM_WORLD, NULL);
            if (strcmp(buffer, "end"))
                cerr << "Ending not received from " << i << endl;
        }
    }
}
