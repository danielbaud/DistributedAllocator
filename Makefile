MPICC=mpic++
MPIRUN=mpirun
MPIFLAGS=-hostfile src/hostfile

.PHONY: main

main: 
	$(MPICC) src/main.cc src/funcs.cc -o main

run:
	$(MPIRUN) $(MPIFLAGS) main

clean:
	rm -f main
