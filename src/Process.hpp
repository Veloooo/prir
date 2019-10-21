#include "mpi.h";

class Process {
public :
	int myid;
	int numprocs;
    MPI_Status status;
};
