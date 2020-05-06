#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Array {
  char *data;
  int length;
} Array;

void ThrowMemoryError() {
  printf("Wystąpił problem z alokacją pamięci\n");
  exit(4);
}

// Wczytywanie pliku do pamięci, w celu zmniejszenia czasu dostępu do znaków
Array *load(FILE *file) {
  fseek(file, 0, SEEK_END);
  size_t fileLenght = ftell(file);
  rewind(file);
  Array *array = (Array *)calloc(1, sizeof(Array));
  char *data = (char *)calloc(fileLenght + 1, sizeof(char));

  if (array == NULL || data == NULL) ThrowMemoryError();

  int i = 0;
  for (char c = fgetc(file); c != EOF; c = fgetc(file)) {
    data[i++] = c;
  }

  array->data = data;
  array->length = fileLenght;
  return array;
}

// Funkcja porównująca znaki, definicja maski ('?') w jednym miejscu
bool isMatching(char predicateChar, char textChar) {
  return predicateChar == textChar || predicateChar == '?';
}

// Funkcja generująca tablice indeksów prefiksów, domyślnie wyzerowana
int *prefixFunction(Array *predicate) {
  int *prefixTable = (int *)calloc(predicate->length, sizeof(int));
  if (prefixTable == NULL) ThrowMemoryError();

  int k = 0;
  for (int q = 2; q <= predicate->length; q++) {
    while ((k > 0) && (predicate->data[k] != predicate->data[q - 1])) {
      k = prefixTable[k - 1];
    }
    if (isMatching(predicate->data[k], predicate->data[q - 1])) {
      k++;
    }
    prefixTable[q - 1] = k;
  }

  return prefixTable;
}

// Wyszukiwanie wzorca w tekście przy pomocy algorytmu Knutha-Morrisa-Pratta
int KMP(Array *text, Array *predicate) {
  int matchesFound = 0;
  int *prefixTable = prefixFunction(predicate);
  int q = 0;
  for (int i = 1; i <= text->length; i++) {
    while ((q > 0) && !isMatching(predicate->data[q], text->data[i - 1])) {
      q = prefixTable[q - 1];
    }
    if (isMatching(predicate->data[q], text->data[i - 1])) {
      q++;
    }
    if (q == predicate->length) {
      // printMatch(text, predicate, (i - predicate->length));
      if (matchesFound == 0) {
        printf(
            "Znaleziono dopasowania na następujących pozycjach:\n"
            "(Pierwszy znak jest oznaczony indeksem 1)\n");
      }
      matchesFound++;
      printf("- %d\n", i - predicate->length + 1);
      q = prefixTable[q - 1];
    }
  }
  return matchesFound;
}

int main(int argc, char const *argv[]) {
  if (argc != 3) {
    printf(
        "Należy podać dwa argumenty, będące ścieżkami do plików:\n"
        "- Wzorzec\n"
        "- Tekst do przeszukania\n");
    return 1;
  }

  FILE *fileP = fopen(argv[1], "r");
  FILE *fileT = fopen(argv[2], "r");

  if (fileP == NULL) {
    printf("Wystąpił błąd podczas otwierania pliku z predykatem\n");
    return 2;
  }
  if (fileT == NULL) {
    printf("Wystąpił błąd podczas otwierania pliku z tekstem\n");
    return 3;
  }

  // Wczytywanie plików
  Array *predicate = load(fileP);
  Array *text = load(fileT);

  int matchesFound = KMP(text, predicate);
  if (matchesFound == 0)
    printf("Nie znaleziono żadnych dopasowań\n");
  else
    printf("---------\nZnaleziono łącznie %d dopasowań\n", matchesFound);

  return 0;
}
