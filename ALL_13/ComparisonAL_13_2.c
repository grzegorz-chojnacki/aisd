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

int getPatternLengthWithoutNewLines(Array *pattern) {
  int length = pattern->length;
  for (int i = 0; i < pattern->length; i++) {
    if (pattern->data[i] == '\n') length--;
  }
  return length;
}

// Funkcja porównująca znaki, definicja maski ('?') w jednym miejscu
bool isMatching(char patternChar, char textChar) {
  return patternChar == textChar;
}

// Funkcja generująca tablice indeksów prefiksów, domyślnie wyzerowana
int *prefixFunction(Array *pattern) {
  int *prefixTable = (int *)calloc(pattern->length, sizeof(int));
  if (prefixTable == NULL) ThrowMemoryError();

  int k = 0;
  for (int q = 2; q <= pattern->length; q++) {
    while ((k > 0) && (pattern->data[k] != pattern->data[q - 1])) {
      k = prefixTable[k - 1];
    }
    if (isMatching(pattern->data[k], pattern->data[q - 1])) {
      k++;
    }
    prefixTable[q - 1] = k;
  }

  return prefixTable;
}

// Wyszukiwanie wzorca w tekście przy pomocy algorytmu Knutha-Morrisa-Pratta
int KMP(Array *text, Array *pattern) {
  int matchesFound = 0;
  int *prefixTable = prefixFunction(pattern);
  int lineNumber = 1;
  int characterNumber = 1;
  int patternLengthWithoutNewLines = getPatternLengthWithoutNewLines(pattern);
  int q = 0;
  for (int i = 1; i <= text->length; i++) {
    // Pomin znaki nowego wiersza
    while (pattern->data[q] == '\n') q++;
    while (text->data[i - 1] == '\n') {
      i++;
      lineNumber++;
      characterNumber = 1;
    }

    while ((q > 0) && !isMatching(pattern->data[q], text->data[i - 1])) {
      q = prefixTable[q - 1];
    }
    if (isMatching(pattern->data[q], text->data[i - 1])) {
      q++;
    }
    if (q == pattern->length) {
      if (matchesFound == 0) {
        printf(
            "Znaleziono dopasowania na następujących pozycjach:\n"
            "(Numer wiersza : znak w wierszu)\n");
      }
      matchesFound++;
      printf("- %d : %d\n", lineNumber,
             characterNumber - patternLengthWithoutNewLines + 1);
      q = prefixTable[q - 1];
    }
    characterNumber++;
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
    printf("Wystąpił błąd podczas otwierania pliku ze wzorcem\n");
    return 2;
  }
  if (fileT == NULL) {
    printf("Wystąpił błąd podczas otwierania pliku z tekstem\n");
    return 3;
  }

  // Wczytywanie plików
  Array *pattern = load(fileP);
  Array *text = load(fileT);

  int matchesFound = KMP(text, pattern);
  if (matchesFound == 0)
    printf("Nie znaleziono żadnych dopasowań\n");
  else
    printf("---------\nZnaleziono łącznie %d dopasowań\n", matchesFound);

  return 0;
}
