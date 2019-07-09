#pragma once

#define MAX_SIZE 10

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <mpi.h>

using namespace std;

bool help();
bool alloc(int master, int rank, int size, string args);
bool free_chain(int master, int rank, int size, string args);
bool read(int master, int rank, int size, string args);
bool list(int master, int rank, int size);
bool kill(int master, int rank, int size, string args);
void send_all(int master, const char *message, int size);
void receive_all_end(int master, int size);
