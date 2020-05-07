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

// Funkcja obsługująca wczytywanie pliku w całości do tablicy
// Zwraca ilość wczytanych znaków
int withNewLines(FILE *file, char *data) {
  int i = 0;
  for (char c = fgetc(file); c != EOF; c = fgetc(file)) {
    data[i++] = c;
  }
  return i;
}

// Funkcja obsługująca wczytywanie pliku do tablicy, bez znaków nowej linii
// Zwraca ilość wczytanych znaków
int withoutNewLines(FILE *file, char *data) {
  int i = 0;
  for (char c = fgetc(file); c != EOF; c = fgetc(file)) {
    if (c != '\n') data[i++] = c;
  }
  return i;
}

// Wczytywanie pliku tekstowego do pamięci za pomocą funkcji:
// withNewLines() lub withoutNewLines()
Array *load(FILE *file, int newLinePolicy(FILE *, char *)) {
  fseek(file, 0, SEEK_END);
  size_t fileLenght = ftell(file);
  rewind(file);
  Array *array = (Array *)calloc(1, sizeof(Array));
  char *data = (char *)calloc(fileLenght + 1, sizeof(char));

  if (array == NULL || data == NULL) ThrowMemoryError();

  int patternLenght = newLinePolicy(file, data);

  array->data = data;
  array->length = patternLenght;
  return array;
}

// Zwraca długość kawałka tekstu jaki dopasował się do wzorca, z uwzględnieniem
// tego, że znaki nowej linii są pomijane przy dopasowywaniu
int patternOffset(Array *text, Array *pattern, int textIndex) {
  int offset = textIndex - 1;
  int patternIndex = pattern->length - 1;
  while (patternIndex >= 0) {
    if (text->data[offset] == pattern->data[patternIndex])
      patternIndex--;
    offset--;
  }
  return textIndex - offset - 1;
}

// Funkcja generująca tablice indeksów prefiksów dla wzorca
int *prefixFunction(Array *pattern) {
  int *prefixTable = (int *)calloc(pattern->length, sizeof(int));
  if (prefixTable == NULL) ThrowMemoryError();
  int k = 0;
  for (int q = 2; q <= pattern->length; q++) {
    while ((k > 0) && (pattern->data[k] != pattern->data[q - 1])) {
      k = prefixTable[k - 1];
    }
    if (pattern->data[k] == pattern->data[q - 1]) {
      k++;
    }
    prefixTable[q - 1] = k;
  }

  return prefixTable;
}

// Wyszukiwanie wzorca w tekście przy pomocy algorytmu Knutha-Morrisa-Pratta
// Zwraca tablice pozycji na których dopasowano wzorzec
int *KMP(Array *text, Array *pattern) {
  int *prefixTable = prefixFunction(pattern);
  int *listOfMatches = (int *)calloc(text->length, sizeof(int));
  int matchIndex = 0;

  if (listOfMatches == NULL) ThrowMemoryError();

  int q = 0;
  for (int i = 1; i <= text->length; i++) {
    // Pomiń znaki nowego wiersza przy dopasowywaniu
    while (text->data[i - 1] == '\n') {
      i++;
    }
    while ((q > 0) && (pattern->data[q] != text->data[i - 1])) {
      q = prefixTable[q - 1];
    }
    if (pattern->data[q] == text->data[i - 1]) {
      q++;
    }
    if (q == pattern->length) {
      // i jest indeksem końca znalezionego wzorca w tekście
      // Należy odjąć od niego tyle znaków ile występuje we wzorcu, ale
      // z dodatkowym uwzględnieniem znaków nowych linii
      listOfMatches[matchIndex++] = i - patternOffset(text, pattern, i) + 1;
      q = prefixTable[q - 1];
    }
  }
  return listOfMatches;
}

bool matches(Array *text, int index, Array *pattern) {
  for (int i = 0; i < pattern->length; i++)
    if (pattern->data[i] != text->data[index + i]) return false;
  return true;
}

// Wyszukiwanie wzorca w tekście przy pomocy algorytmu Rabina-Karpa
// Zwraca tablice pozycji na których dopasowano wzorzec
int *RB(Array *text, Array *pattern, int alphabetSize, int prime) {
  int *listOfMatches = (int *)calloc(text->length, sizeof(int));
  int matchIndex = 0;
  if (listOfMatches == NULL) ThrowMemoryError();

  int h = 1;
  for (int i = 0; i < pattern->length; i++) h = (h * alphabetSize) % prime;

  int p = 0;
  int t = 0;
  for (int i = 0; i < pattern->length; i++) {
    p = (alphabetSize * p + pattern->data[i]) % prime;
    t = (alphabetSize * t + text->data[i])    % prime;
  }
  for (int s = 0; s < (text->length - pattern->length); s++) {
    if (p == t) {
      // Porównywanie tekstu ze wzorcem
      if (matches(text, s, pattern))
        listOfMatches[matchIndex++] = s;
    }
    if (s < (text->length - pattern->length)) {
      int t1 = (text->data[s] * h) % prime;
      if (t < t1) t = t + prime;
      t = (alphabetSize * (t - t1) + text->data[s + pattern->length]) % prime;
    }
  }
  return listOfMatches;
}

void printResults(Array *text, int *listOfMatches) {
  if (listOfMatches[0] == 0) {
    printf("Nie znaleziono żadnych dopasowań\n");
    return;
  }
  printf("Znaleziono dopasowania na następujących pozycjach:\n"
         "(Numer wiersza i znaku numerowane od 1)\n");
  int matchIndex = 0;
  int lineNumber = 1;
  int characterInLine = 1;
  for (int i = 0; i < text->length; i++) {
    if (text->data[i] == '\n') {
      lineNumber++;
      characterInLine = 0;
    }
    if (i == listOfMatches[matchIndex] - 1) {
      printf("- %d : %d\n", lineNumber, characterInLine);
      matchIndex++;
      if (listOfMatches[matchIndex] == 0) break;
    }
    characterInLine++;
  }
  printf("---------\nZnaleziono łącznie %d dopasowań\n", matchIndex);
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
  Array *pattern = load(fileP, withoutNewLines);
  Array *text = load(fileT, withNewLines);

  // int *listOfMatches = KMP(text, pattern);
  // printResults(text, listOfMatches);
  int *listOfMatches = RB(text, pattern, 128, 27077);
  for (int i = 0; listOfMatches[i] != 0; i++) {
    printf("- %d\n", listOfMatches[i]);
  }
  // printResults(text, listOfMatches);

  return 0;
}
