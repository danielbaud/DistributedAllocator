# Distributed Malloc

## How to install

**macOs**
```bash
brew install open-mpi
```

**Linux**
Download MPI then,
```bash
./configure --prefix="/home/$USER/.openmpi"
make
sudo make install
```

# How to run

```bash
make && make run
```
# How to use

> alloc -f <file> | <string>		Allocate either a file in memory or the string <string>
> free <chain> 				Frees the chain <chain>
> read <chain>				Reads the chain <chain>
> list					Shows a list of the processes and their allocated memory
> kill <process>			Kills the process <process>
> exit | EOF				Exits the program
> help					Display help

# Credit

rivier_c (vincent1.riviere@epita.fr)
baud_f (daniel.baud@epita.fr)
