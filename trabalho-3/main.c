// Gabriel Schrader Vilas Boas, RA: 150981
// João Pedro Assumpção Evaristo, RA: 147887
// Ramon da Silva Passos, RA: 148011

// mpicc -o main main.c
// mpiexec -np 4 ./main
#include "mpi_utils.h"
#include <time.h>
#include <stdlib.h>
#define GENERATIONS 2000

void sendZeros(float **grid, int ini, int end)
{
  int i, j;

  for (i = ini; i < end; i++)
  {
    for (j = 0; j < DIMENSION; j++)
      grid[i][j] = 0.0;

    MPI_Send(grid[i], DIMENSION, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
  }
}

void receiveZeros(float **grid, int numProcesses)
{
  receiveGridFromWorkers(grid, numProcesses);
}

// Funcao que retorna o numero de celulas
// vivas no tabuleiro.
int getAliveCells(float **grid, int ini, int end)
{
  int sum = 0;
  int i, j;
  for (i = ini; i < end; i++)
  {
    for (j = 0; j < DIMENSION; j++)
    {
      if (grid[i][j] > 0.0)
        sum++;
    }
  }
  return sum;
}

void workerProcess(int processId, int numProcesses)
{
  float **readingGrid = mallocGrid();
  float **writingGrid = mallocGrid();

  int aux = 0;
  int i, j, ini, end;
  calculateOffset(processId, numProcesses, &ini, &end);
  sendZeros(readingGrid, ini, end);

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

  int sum = getAliveCells(writingGrid, ini, end);
  sendAliveCellsToMaster(sum);
}

void masterProcess(int numProcesses)
{
  int sum = 0, aux = 0;
  float **readingGrid = mallocGrid();
  float **writingGrid = mallocGrid();

  receiveZeros(readingGrid, numProcesses);
  initializeGrid(readingGrid);

  while (aux < GENERATIONS)
  {
    sendGridToWorkers(readingGrid, numProcesses);
    receiveGridFromWorkers(writingGrid, numProcesses);
    swap(&readingGrid, &writingGrid);
    aux++;
  }

  getAliveCellsFromWorkers(&sum);
  printf("Numero de celulas vivas: %d\n", sum);
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
