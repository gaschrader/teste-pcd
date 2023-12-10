#include <stdio.h>
#include "mpi.h"
#define DIMENSION 2048

typedef struct
{
  int number;
  float avg;
} Neighbors;

void fillZeros(float **grid)
{
    int i, j;
    for (i = 0; i < DIMENSION; i++)
    {
        for (j = 0; j < DIMENSION; j++)
        {
            grid[i][j] = 0.0;
        }
    }
}

// Funcao que inicializa o primeiro
// tabuleiro do jogo.
void initializeGrid(float **grid)
{
  int lin = 1, col = 1;
  grid[lin][col + 1] = 1.0;
  grid[lin + 1][col + 2] = 1.0;
  grid[lin + 2][col] = 1.0;
  grid[lin + 2][col + 1] = 1.0;
  grid[lin + 2][col + 2] = 1.0;
  lin = 10;
  col = 30;
  grid[lin][col + 1] = 1.0;
  grid[lin][col + 2] = 1.0;
  grid[lin + 1][col] = 1.0;
  grid[lin + 1][col + 1] = 1.0;
  grid[lin + 2][col + 1] = 1.0;
}

// Funcao para trocar os grids
void swap(float ***readingGrid, float ***writingGrid)
{
  float **temp = *readingGrid;
  *readingGrid = *writingGrid;
  *writingGrid = temp;
}

// Funcao para printar o tabuleiro.
void printGrid(float **grid)
{
  int i, j;
  for(i = 0; i < DIMENSION; i++)
  {
    for(j = 0; j < DIMENSION; j++)
    {
      if (grid[i][j] > 0.0 && grid[i][j] <= 0.25)
        printf("*");
      else if (grid[i][j] > 0.25 && grid[i][j] <= 0.5)
        printf("#");
      else if (grid[i][j] > 0.5 && grid[i][j] <= 0.75)
        printf("%");
      else if (grid[i][j] > 0.75)
        printf("@");
      else
        printf("_");
    }
    printf("\n");
  }
  printf("=======================================================\n");
}

// Funcao que retorna o numero de vizinhos
// de uma celula.

Neighbors getNeighbors(float **grid, int i, int j)
{
  int neighborsNum = 0;
  float neighborsSum = 0;
  int x, y;
  int neighbors[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  Neighbors result;

  int k;
  // verifica horizontal e vertical
  for (k = 0; k < 8; k++)
  {
    x = i + neighbors[k][0];
    y = j + neighbors[k][1];
    if (x < 0)
    {
      x = DIMENSION - 1;
    }
    else if (x >= DIMENSION)
    {
      x = 0;
    }
    if (y < 0)
    {
      y = DIMENSION - 1;
    }
    else if (y >= DIMENSION)
    {
      y = 0;
    }

    if (grid[x][y] > 0)
    {
      neighborsNum++;
    }
    neighborsSum += grid[x][y];
  }

  float avg = neighborsSum / 8.0;

  result.number = neighborsNum;
  result.avg = avg;
  return result;
}

void assignCellValue(float **readingGrid, float **writingGrid, int i, int j)
{
  Neighbors neighbors = getNeighbors(readingGrid, i, j);
  int numNeighbors = neighbors.number;
  float avg = neighbors.avg;
  float currentCell = readingGrid[i][j];
  int alive = (currentCell > 0) ? 1 : 0;

  // Regra 3
  writingGrid[i][j] = 0.0;

  // Regra 1
  if ((numNeighbors == 2 || numNeighbors == 3) && alive)
  {
    writingGrid[i][j] = currentCell;
  }

  // Regra 2
  if (numNeighbors == 3 && !alive)
  {
    writingGrid[i][j] = 1.0;
  }
}

int ceil(double number)
{
  int aux = (int)number;
  if (number > aux)
  {
    return aux + 1;
  }
  return aux;
}
