#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Cell {
  int value;
  char symbol;
} Cell;

typedef struct LCSMatrix {
  Cell ***content;  // Dwuwymiarowa macierz wskaźników na struktury typu Cell
  int sizeX;
  int sizeY;
} LCSMatrix;

#pragma region printing
void printHR(int size) {
  printf("+----+");
  for (int i = 0; i < size; i++) printf("----");
  printf("+\n");
}

void printMatrix(LCSMatrix *matrix, char *stringA, char *stringB) {
  printHR(matrix->sizeX);
  printf("| y\\x|    ");
  for (int x = 0; x < matrix->sizeX - 1; x++) printf(" %2c ", stringA[x]);
  printf("|\n");
  printHR(matrix->sizeX);
  // Wnętrze macierzy
  for (int y = 0; y < matrix->sizeY; y++) {
    if (y == 0)
      printf("|    |");
    else
      printf("| %2c |", stringB[y - 1]);
    for (int x = 0; x < matrix->sizeX; x++) {
      printf(" %1c%1d ", matrix->content[y][x]->symbol,
             matrix->content[y][x]->value);
    }
    printf("|\n");
  }
  printHR(matrix->sizeX);
}
#pragma endregion printing

void printLCSHelper(LCSMatrix *matrix, char *string, int x, int y) {
  if (x == 0 || y == 0) {
    printf("\n - ");
    return;
  }
  if (matrix->content[y][x]->symbol == '\\') {
    printLCSHelper(matrix, string, x - 1, y - 1);
    printf("%c", string[x - 1]);
  } else if (matrix->content[y][x]->symbol == '|') {
    printLCSHelper(matrix, string, x, y - 1);
  } else {
    printLCSHelper(matrix, string, x - 1, y);
  }
}

void printLCS(LCSMatrix *matrix, char *stringA, char *stringB) {
  int x = strlen(stringA);
  int y = strlen(stringB);
  printf("Najdłuższe wspólne podciągi \"%s\" i \"%s\" to:", stringA, stringB);
  printLCSHelper(matrix, stringA, x, y);
  printf("\n");
}

LCSMatrix *makeMatrix(int sizeX, int sizeY) {
  LCSMatrix *matrix = (LCSMatrix *)calloc(1, sizeof(LCSMatrix));
  matrix->content = (Cell ***)calloc(sizeY, sizeof(Cell));
  for (int y = 0; y < sizeY; y++) {
    matrix->content[y] = (Cell **)calloc(sizeX, sizeof(Cell));
    for (int x = 0; x < sizeX; x++) {
      matrix->content[y][x] = (Cell *)calloc(1, sizeof(Cell));
      matrix->content[y][x]->value = 0;
    }
  }
  matrix->sizeX = sizeX;
  matrix->sizeY = sizeY;
  return matrix;
}

LCSMatrix *getLCSMatrix(char *stringA, char *stringB) {
  int lengthA = strlen(stringA);
  int lengthB = strlen(stringB);
  LCSMatrix *matrix = makeMatrix(lengthA + 1, lengthB + 1);

  // y - indeks poruszania się pionowo w tabeli == j
  // x - indeks poruszania się poziomo w tabeli == i
  for (int y = 1; y <= lengthB; y++) {
    for (int x = 1; x <= lengthA; x++) {
      if (stringA[x - 1] == stringB[y - 1]) {
        matrix->content[y][x]->value = matrix->content[y - 1][x - 1]->value + 1;
        matrix->content[y][x]->symbol = '\\';
      } else if (matrix->content[y - 1][x]->value >=
                 matrix->content[y][x - 1]->value) {
        matrix->content[y][x]->value = matrix->content[y - 1][x]->value;
        matrix->content[y][x]->symbol = '|';
      } else {
        matrix->content[y][x]->value = matrix->content[y][x - 1]->value;
        matrix->content[y][x]->symbol = '-';
      }
    }
  }

  return matrix;
}

int main() {
  char stringA[] = "SPAWACZ";
  char stringB[] = "SPRAWA";
  LCSMatrix *matrix = getLCSMatrix(stringA, stringB);
  printMatrix(matrix, stringA, stringB);
  printLCS(matrix, stringA, stringB);
  return 0;
}