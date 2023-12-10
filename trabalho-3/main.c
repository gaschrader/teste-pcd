// Gabriel Schrader Vilas Boas, RA: 150981
// João Pedro Assumpção Evaristo, RA: 147887
// Ramon da Silva Passos, RA: 148011

// mpicc -o main main.c 
// mpiexec -np 4 ./main

#include <stdio.h>
#include "common.h"
#include "mpi.h"
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#define GENERATIONS 50
#define DIMENSION 2048

// MPI_Send definition
// int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)

// MPI_Recv definition
// int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
void sendGridToWorkers(float **readingGrid, int numProcesses) {
    int i, j;
    for (i = 1; i < numProcesses; i++) {
        for (j = 0; j < DIMENSION; j++) {
            MPI_Send(readingGrid[j], DIMENSION, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        }
    }
}

void receiveGridFromMaster(float **readingGrid) {
    int i;
    for (i = 0; i < DIMENSION; i++) {
        MPI_Recv(readingGrid[i], DIMENSION, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void calculateOffset(int processId, int numProcesses, int *ini, int *end) {
    int ini, end, numWorkers, div;
    numWorkers = numProcesses - 1;
    div = ceil(DIMENSION / (double)numWorkers);
    *ini = div * (processId -1);
    *end = div * processId < DIMENSION ? div * processId : DIMENSION;
}

// Funcao que retorna o numero de celulas
// vivas no tabuleiro.
int getAliveCells(float **grid)
{
  int sum = 0;
  int i, j;
  for (i = 0; i < DIMENSION; i++)
  {
    for (j = 0; j < DIMENSION; j++)
    {
      if (grid[i][j] > 0.0)
        sum++;
    }
  }
  return sum;
}

void masterPlay(float **readingGrid, float **writingGrid)
{
  int aux = 0;
  int i, j;

  while (aux < GENERATIONS)
  {
    sendGridToWorkers(readingGrid, numProcesses);
    receiveGridFromWorkers(writingGrid, numProcesses);
    swap(&readingGrid, &writingGrid);
    aux++;
  }
}

void workerPlay(float **readingGrid, float **writingGrid, int processId, int numProcesses)
{
  int aux = 0;
  int i, j, ini, end;
  calculateOffset(processId, numProcesses, &ini, &end);

  while (aux < GENERATIONS)
  {
    receiveGridFromMaster(readingGrid);
    for (i = ini; i < end; i++)
    {
      for (j = 0; j < DIMENSION; j++)
      {
        assignCellValue(readingGrid, writingGrid, i, j);
      }
    }
    sendGridToMaster(writingGrid, processId, ini, end);
    aux++;
  }
}

void workerProcess(int processId, int numProcesses) {
    float **readingGrid = (float **)malloc(DIMENSION * sizeof(float *));
    float **writingGrid = (float **)malloc(DIMENSION * sizeof(float *));

    int i;
    for (i = 0; i < DIMENSION; i++) {
        readingGrid[i] = (float *)malloc(DIMENSION * sizeof(float));
    }

    for (i = 0; i < DIMENSION; i++) {
        writingGrid[i] = (float *)malloc(DIMENSION * sizeof(float));
    }

    receiveGridFromMaster(readingGrid);
    workerPlay(readingGrid, writingGrid, processId, numProcesses);
}

void masterProcess(int numProcesses) {
    float **readingGrid = (float **)malloc(DIMENSION * sizeof(float *));
    float **writingGrid = (float **)malloc(DIMENSION * sizeof(float *));

    int i;
    for (i = 0; i < DIMENSION; i++) {
        readingGrid[i] = (float *)malloc(DIMENSION * sizeof(float));
    }

    for (i = 0; i < DIMENSION; i++) {
        writingGrid[i] = (float *)malloc(DIMENSION * sizeof(float));
    }

    fillZeros(readingGrid);
    initializeGrid(readingGrid);
    masterPlay(readingGrid, writingGrid);
}

int main(void) {
    int processId, numProcesses;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    if(processId == 0) {
        masterProcess(numProcesses);
    } else {
        workerProcess(processId, numProcesses);
    }

    MPI_Finalize();
    return 0;
}
