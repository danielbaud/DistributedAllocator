MPICC=mpicc
MPIRUN=mpirun
MPIFLAGS=-hostfile hostfile

main: 
	$(MPICC) src/main.cc -o main

run:
	$(MPIRUN) $(MPIFLAGS) src/main

clean:
	rm -f main
