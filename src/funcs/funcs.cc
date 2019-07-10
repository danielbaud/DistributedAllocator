#include "funcs.hh"

using namespace std;

bool help()
{
    cout << "Available commands:" << endl;
    cout << "\talloc -f <file> | <string>\tAllocate either a file in memory or the string <string>" << endl;
    cout << "\tfree <chain>\t\t\tFrees the chain <chain>" << endl;
    cout << "\tread <chain>\t\t\tReads the chain <chain>" << endl;
    cout << "\tlist\t\t\t\tShows a list of the processes and their allocated memory" << endl;
    cout << "\tkill <process>\t\t\tKills the process <process>" << endl;
    cout << "\texit | EOF\t\t\tExits the program" << endl;
    cout << "\thelp\t\t\t\tDisplays this message" << endl;
    return true;
}

Chain *alloc(int master, int rank, int size, string args, Chunk *chunk)
{
    Chain *chain = new Chain(-1, -1, FREE);
    Chain *par = chain;
    if (master == rank)
    {
        int length = args.length();
        string sentence = args;
        if (args.length() == 0)
        {
            cerr << "alloc: argument needed" << endl;
            return nullptr;
        }
        if (args.length() > 3 && args[0] == '-' && args[1] == 'f' && args[2] == ' ')
        {
            args = args.substr(3);
            int fd = open(args.c_str(), O_RDONLY);
            if (fd == -1)
            {
                cerr << args << ": No such file" << endl;
                return nullptr;
            }
            struct stat buf;
            fstat(fd, &buf);
            length = buf.st_size;
            close(fd);
            ifstream in(args);
            sentence = "";
            while (in.good())
            {
                char c = in.get();
                sentence += c;
            }
        }
        send_all(master, "alloc", size);
        int slave = 0;
        int recv = 0;
        int sent = 0;
        while (sent < length)
        {
            if (slave != master)
            {
                MPI_Send(&length, 1, MPI_INT, slave, 0, MPI_COMM_WORLD);
                MPI_Recv(&recv, 1, MPI_INT, slave, 0, MPI_COMM_WORLD, NULL);
                if (recv > 0)
                {
                    MPI_Send(sentence.substr(sent, recv).c_str(), recv, MPI_CHAR, slave, 0, MPI_COMM_WORLD);
                    int s = recv;
                    sent += recv;
                    MPI_Recv(&recv, 1, MPI_INT, slave, 0, MPI_COMM_WORLD, NULL);
                    par->next = new Chain(slave, recv, s);
                    par = par->next;
                }
            }
            slave += 1;
        }
        int st = -1;
        for (unsigned i = 0; i < size; ++i)
            if (i != master)
                MPI_Send(&st, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    else
    {
        int recv = 0;
        while (recv != -1)
        {
            MPI_Recv(&recv, 1, MPI_INT, master, 0, MPI_COMM_WORLD, NULL);
            if (recv > 0)
            {
                int where = 0;
                while (chunk && chunk->get_state() == ALLOCATED)
                {
                    where += chunk->get_size();
                    chunk = chunk->get_next();
                }
                if (chunk)
                {
                    int send;
                    if (chunk->get_size() > recv)
                    {
                        send = recv;
                        chunk->split(send, ALLOCATED, FREE);
                    }
                    else
                        send = chunk->get_size();
                    MPI_Send(&send, 1, MPI_INT, master, 0, MPI_COMM_WORLD);
                    MPI_Recv(chunk->start, send, MPI_CHAR, master, 0, MPI_COMM_WORLD, NULL);
                    MPI_Send(&where, 1, MPI_INT, master, 0, MPI_COMM_WORLD);
                }
            }
        }
    }
    if (master == rank)
        receive_all_end(master, size);
    else
        MPI_Send("end", 4, MPI_CHAR, master, 0, MPI_COMM_WORLD);
    return chain;
}

string read(int master, int rank, int size, string args, Chunk *chunk, vector<Chain*> *chains)
{
    string result = "";
    if (master == rank)
    {
        send_all(master, "read", size);
        int slot = stoi(args);
        vector<Chain*> npchains = *chains;
        Chain *read = npchains[slot];
        read = read->next;
        while (read)
        {
            char buffer[read->size];
            MPI_Send(&(read->where), 1, MPI_INT, read->process, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer, read->size, MPI_CHAR, read->process, 0, MPI_COMM_WORLD, NULL);
            for (unsigned i = 0; i < read->size; ++i)
                result += buffer[i];
            read = read->next;
        }
        int st = -1;
        for (unsigned i = 0; i < size; ++i)
            if (i != master)
                MPI_Send(&st, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    else
    {
        int recv = 0;
        while (recv != -1)
        {
            MPI_Recv(&recv, 1, MPI_INT, master, 0, MPI_COMM_WORLD, NULL);
            if (recv >= 0)
            {
                Chunk *ch = chunk;
                int where = 0;
                while (where < recv)
                {
                    where += ch->get_size();
                    ch = ch->get_next();
                }

                MPI_Send(ch->start, ch->get_size(), MPI_CHAR, master, 0, MPI_COMM_WORLD);
            }
        }
    }
    if (master == rank)
        receive_all_end(master, size);
    else
        MPI_Send("end", 4, MPI_CHAR, master, 0, MPI_COMM_WORLD);
    return result;
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

bool kill(int master, int rank, int size, string args)
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

bool free_chain(int master, int rank, int size, string args)
{
    if (master == rank)
        send_all(master, "free", size);
    cout << rank << ": free" << endl;
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
