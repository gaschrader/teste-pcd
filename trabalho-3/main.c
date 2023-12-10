// Gabriel Schrader Vilas Boas, RA: 150981
// João Pedro Assumpção Evaristo, RA: 147887
// Ramon da Silva Passos, RA: 148011

// mpicc -o main main.c
// mpiexec -np 4 ./main
#include "mpi_utils.h"
#include <time.h>
#include <stdlib.h>
#define GENERATIONS 5

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

void workerProcess(int processId, int numProcesses)
{
  float **readingGrid = (float **)malloc(DIMENSION * sizeof(float *));
  float **writingGrid = (float **)malloc(DIMENSION * sizeof(float *));

  int i;
  for (i = 0; i < DIMENSION; i++)
  {
    readingGrid[i] = (float *)malloc(DIMENSION * sizeof(float));
  }

  for (i = 0; i < DIMENSION; i++)
  {
    writingGrid[i] = (float *)malloc(DIMENSION * sizeof(float));
  }

  workerPlay(readingGrid, writingGrid, processId, numProcesses);
}

void masterProcess(int numProcesses)
{
  float **readingGrid = (float **)malloc(DIMENSION * sizeof(float *));
  float **writingGrid = (float **)malloc(DIMENSION * sizeof(float *));

  int i, aux = 0;
  for (i = 0; i < DIMENSION; i++)
  {
    readingGrid[i] = (float *)malloc(DIMENSION * sizeof(float));
  }

  for (i = 0; i < DIMENSION; i++)
  {
    writingGrid[i] = (float *)malloc(DIMENSION * sizeof(float));
  }

  fillZeros(readingGrid);
  initializeGrid(readingGrid);
  
  while (aux < GENERATIONS)
  {
    sendGridToWorkers(readingGrid, numProcesses);
    receiveGridFromWorkers(writingGrid, numProcesses);
    swap(&readingGrid, &writingGrid);
    printf("Numero de celulas vivas: %d\n", getAliveCells(readingGrid));
    aux++;
  }
}

int main(void)
{
  int processId, numProcesses;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &processId);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

  if (processId == 0)
  {
    masterProcess(numProcesses);
  }
  else
  {
    workerProcess(processId, numProcesses);
  }

  MPI_Finalize();
  return 0;
}
