#include <stdio.h>
#include "common.h"
#include "mpi.h"

// MPI_Send definition
// int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)

// MPI_Recv definition
// int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
void sendGridToWorkers(float **readingGrid, int numProcesses)
{
  int i, j;
  for (i = 1; i < numProcesses; i++)
  {
    for (j = 0; j < DIMENSION; j++)
    {
      MPI_Send(readingGrid[j], DIMENSION, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
    }
  }
}

void sendGridToMaster(float **writingGrid, int processId, int ini, int end)
{
  int i;
  for (i = ini; i < end; i++)
  {
    MPI_Send(writingGrid[i], DIMENSION, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
  }
}

void receiveGridFromWorkers(float **writingGrid, int numProcesses)
{
  int i, j, ini, end;

  for (i = 1; i < numProcesses; i++)
  {
    calculateOffset(i, numProcesses, &ini, &end);
    for (j = ini; j < end; j++)
    {
      MPI_Recv(writingGrid[j], DIMENSION, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }
}

void receiveGridFromMaster(float **readingGrid)
{
  int i;
  for (i = 0; i < DIMENSION; i++)
  {
    MPI_Recv(readingGrid[i], DIMENSION, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
}
