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

// Wyświetlanie poziomej podziałki
void printHR(int size) {
  printf("+----+");
  for (int i = 0; i < size; i++) printf("----");
  printf("+\n");
}

void printMatrix(LCSMatrix *matrix, const char *stringA, const char *stringB) {
  // Nagłówek
  printHR(matrix->sizeX);
  printf("| y\\x|    ");
  for (int x = 0; x < matrix->sizeX - 1; x++) printf(" %2c ", stringA[x]);
  printf("|\n");
  printHR(matrix->sizeX);
  // Wnętrze macierzy
  for (int y = 0; y < matrix->sizeY; y++) {
    // Pierwsza kolumna
    if (y == 0) {
      printf("|    |");
    } else {
      printf("| %2c |", stringB[y - 1]);
    }

    for (int x = 0; x < matrix->sizeX; x++) {
      printf(" %1c%1d ", matrix->content[y][x]->symbol,
             matrix->content[y][x]->value);
    }
    printf("|\n");
  }

  printHR(matrix->sizeX);
}

// letter to indeks litery do zmodyfikowania (konwencja 1..n) w tablicy output.
// Domyślnie zaczyna jako wartość w prawym dolnym rogu LCSMatrix, i wskazuje
// malejąco kolejne litery do przepisania do tablicy output
void printLCSHelper(LCSMatrix *matrix, const char *string, int letter, char *output,
                    int x, int y) {
  if (x == 0 || y == 0) {
    // Wyświetlamy tablice znaków output[] tylko jeżeli została wypełniona
    // do końca (letter w poprzednim wywołaniu == 1, czyli została zmieniona
    // pierwsza litera)
    if (letter == 0) printf(" - \"%s\"\n", output);
    // Nie trzeba dbać o zerowanie output, bo jeżeli ma być wyświetlona, to
    // na pewno zostanie nadpisana w całości
    return;
  }
  if (matrix->content[y][x]->symbol == '\\') {
    output[letter - 1] = string[x - 1];
    printLCSHelper(matrix, string, letter - 1, output, x - 1, y - 1);
  } else {
    printLCSHelper(matrix, string, letter, output, x, y - 1);
    printLCSHelper(matrix, string, letter, output, x - 1, y);
  }
}

void printLCS(LCSMatrix *matrix, const char *stringA, const char *stringB) {
  int x = strlen(stringA);
  int y = strlen(stringB);
  // Długość najdłuższego ciągu znaków - wyświetlamy tylko przypadki których
  // długość jest równa tej wartości
  int LCSLength = matrix->content[y][x]->value;
  // String przechowujący kandydata do wyświetlenia ( + ostatni znak null)
  char *output = (char *)calloc(LCSLength + 1, sizeof(char));
  printf("Warianty najdłuższych wspólnych podciągów \"%s\" i \"%s\" to:\n",
         stringA, stringB);
  printLCSHelper(matrix, stringA, LCSLength, output, x, y);
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

LCSMatrix *getLCSMatrix(const char *stringA, const char *stringB) {
  int lengthA = strlen(stringA);
  int lengthB = strlen(stringB);
  LCSMatrix *matrix = makeMatrix(lengthA + 1, lengthB + 1);

  // y - indeks poruszania się pionowo w tabeli
  // x - indeks poruszania się poziomo w tabeli
  // Dostęp do liter łańcucha znaków według konwencji 1..n
  // Dostęp do komórek w macierzy według konwencji 0..n - 1
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


int main(int argc, char const *argv[]) {
  const char *stringA;
  const char *stringB;
  if (argc != 3) {
    printf("Nie podano napisów do porównania\n");
    printf("Dane testowe: \"abbaac\", \"bacbacba\"\n\n");
    stringA = "abbaac";
    stringB = "bacbacba";
  } else {
    stringA = argv[1];
    stringB = argv[2];
  }
  LCSMatrix *matrix = getLCSMatrix(stringA, stringB);
  // printMatrix(matrix, stringA, stringB);
  printLCS(matrix, stringA, stringB);
  return 0;
}