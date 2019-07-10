#pragma once

#include <cstdlib>
#include <sys/mman.h>

enum State
{
    ALLOCATED,
    FREE
};

class Chunk
{
public:
    Chunk(int size, void *start, State state);
    void set_next(Chunk *n);
    Chunk *get_next();
    int get_size();
    void set_state(State s);
    State get_state();
    void split(int where, State s1, State s2);
    void *start;

private:
    int size;
    Chunk *next;
    State state;
};

class Chain
{
public:
    Chain(int process, int where, int size);
    ~Chain();
    int process;
    int where;
    int size;
    Chain *next;
};
