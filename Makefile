MPICC=mpic++
MPIRUN=mpirun
MPIFLAGS=-hostfile src/hostfile

.PHONY: main



main: 
	$(MPICC) src/main.cc src/funcs/funcs.cc src/chunk/chunk.cc -o main -g

run:
	$(MPIRUN) $(MPIFLAGS) main

clean:
	rm -f main
