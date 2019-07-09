#pragma once

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <mpi.h>

bool help();
bool alloc(int master, int rank, int size);
bool read(int master, int rank, int size);
bool list(int master, int rank, int size);
bool kill(int master, int rank, int size);
void send_all(int master, const char *message, int size);
void receive_all_end(int master, int size);
