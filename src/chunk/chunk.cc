#include "chunk.hh"

Chunk::Chunk(int size, void *start, State state)
: size(size)
, start(start)
, next(nullptr)
, state(state)
{}

void Chunk::set_next(Chunk *n)
{
    next = n;
}
Chunk *Chunk::get_next()
{
    return next;
}

int Chunk::get_size()
{
    return size;
}

void Chunk::set_state(State s)
{
    state = s;
}

State Chunk::get_state()
{
    return state;
}

void Chunk::split(int where, State s1, State s2)
{
    int size2 = size - where;
    int size1 = where;
    void *newst = (void*)(((char*)start) + where);
    Chunk *old = this->next;
    this->next = new Chunk(size2, newst, s2);
    this->next->next = old;
    this->size = size1;
    this->state = s1;
}

Chain::Chain(int process, int where, int size)
: process(process)
, where(where)
, size(size)
, next(nullptr)
{}
