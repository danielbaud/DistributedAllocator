#pragma once

#define MAX_SIZE 100

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <mpi.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include "../chunk/chunk.hh"

using namespace std;

bool help();
Chain *alloc(int master, int rank, int size, string args, Chunk *chunk);
string free_chain(int master, int rank, int size, string args, Chunk *chunk, vector<Chain*> *chains);
string read(int master, int rank, int size, string args, Chunk *chunk, vector<Chain*> *chains);
bool list(vector<Chain*> chains);
bool kill(int master, int rank, int size, string args);
void send_all(int master, const char *message, int size);
void receive_all_end(int master, int size);
