#define PS1 "> "
#include "funcs/funcs.hh"

using namespace std;

int main(int argc, char **argv)
{
    int rank, size, len;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    bool tty = isatty(STDOUT_FILENO) != 0;
    // Master (election?)
    int master = 0;
    if (rank == master)
    {
        string command;
        string temp;
        do
        {
            if (tty)
                cout << PS1;

            cin >> command;

            string args;
            getline(cin, args);

            if (cin.eof() || command == "exit")
            {
                if (cin.eof())
                    cout << endl;
                send_all(master, "ret", size);
                MPI_Finalize();
                return 0;
            }
            else if (command == "help")
                help();
            else if (command == "alloc")
                alloc(master, rank, size, args);
            else if (command == "read")
                read(master, rank, size, args);
            else if (command == "list")
                list(master, rank, size);
            else if (command == "kill")
                kill(master, rank, size, args);
            else if (command == "free")
                free_chain(master, rank, size, args);
            else
                cout << command << ": command not found" << endl;
        }
        while (tty);

        send_all(master, "ret", size);
        MPI_Finalize();
        return 0;
    }
    else
    {
        char buffer[6];
        do
        {
            MPI_Recv(buffer, 6, MPI_CHAR, master, 0, MPI_COMM_WORLD, NULL);
            if (!strcmp(buffer, "ret"))
            {
                MPI_Finalize();
                return 0;
            }
            else if (!strcmp(buffer, "alloc"))
                alloc(master, rank, size, "");
            else if (!strcmp(buffer, "read"))
                read(master, rank, size, "");
            else if (!strcmp(buffer, "kill"))
                kill(master, rank, size, "");
            else if (!strcmp(buffer, "list"))
                list(master, rank, size);
            else if (!strcmp(buffer, "free"))
                free_chain(master, rank, size, "");
            else
                cout << buffer << ": unknown MPI message" << endl;
        }
        while (tty);
        MPI_Recv(buffer, 6, MPI_CHAR, master, 0, MPI_COMM_WORLD, NULL);
        MPI_Finalize();
        return 0;
    }
}
