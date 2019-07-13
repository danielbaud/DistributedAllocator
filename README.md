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

* alloc -f _file_ | _string_		Allocate either a file in memory or the string _string_
* free _chain_ 				Frees the chain _chain_
* read _chain_				Reads the chain _chain_
* list					Shows a list of the processes and their allocated memory
* kill _process_			Kills the process _process_
* exit | EOF				Exits the program
* help					Display help

# Credit

rivier_c (vincent1.riviere@epita.fr)
baud_f (daniel.baud@epita.fr)
