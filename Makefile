MPICC=mpicc
MPIRUN=mpirun
MPIFLAGS=-hostfile src/hostfile

main: 
	$(MPICC) src/main.cc -o main

run:
	$(MPIRUN) $(MPIFLAGS) main

clean:
	rm -f main
