#define PS1 "> "
#include "funcs/funcs.hh"
#include "chunk/chunk.hh"

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
        vector<Chain*> chains = vector<Chain*>();
        do
        {
            if (tty)
                cout << endl << PS1;

            cin >> command;

            string args;
            getline(cin, args);
            if (args.length() > 1)
                args = args.substr(1);

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
            {
                Chain *c = alloc(master, rank, size, args, nullptr);
                if (c)
                {
                    unsigned i = 0;
                    for (; i < chains.size(); ++i)
                    {
                        if (!chains[i])
                        {
                            chains[i] = c;
                            break;
                        }
                    }
                    if (i == chains.size())
                        chains.push_back(c);
                    cout << "Allocated in memory slot " << i << endl;
                }
            }
            else if (command == "read")
                cout << read(master, rank, size, args, nullptr, &chains) << endl;
            else if (command == "list")
                list(chains);
            else if (command == "kill")
                kill(master, rank, size, args);
            else if (command == "free")
            {
                cout << free_chain(master, rank, size, args, nullptr, &chains) << endl;
                chains[stoi(args)] = nullptr;
            }
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
        void *mem = malloc(MAX_SIZE);
        Chunk *chunk = new Chunk(MAX_SIZE, mem, FREE);
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
                alloc(master, rank, size, "", chunk);
            else if (!strcmp(buffer, "read"))
                read(master, rank, size, "", chunk, nullptr);
            else if (!strcmp(buffer, "kill"))
                kill(master, rank, size, "");
            else if (!strcmp(buffer, "free"))
                free_chain(master, rank, size, "", chunk, nullptr);
            else
                cout << buffer << ": unknown MPI message" << endl;
        }
        while (tty);
        MPI_Recv(buffer, 6, MPI_CHAR, master, 0, MPI_COMM_WORLD, NULL);
        MPI_Finalize();
        return 0;
    }
}
