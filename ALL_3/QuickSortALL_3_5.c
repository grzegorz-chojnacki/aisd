#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void printArray(int *array, int arrayLength) {
  printf("Array(");
  for (int i = 0; i < arrayLength; i++) {
    if (i < arrayLength - 1) {
      printf("%d, ", array[i]);
    } else {
      printf("%d)\n", array[i]);
    }
  }
}

// Zamienia indeks tablicowy z konwencji [1 ... n] na [0 ... n - 1]
int get(int index) { return index - 1; }

int main(int argc, char **argv) {
  // Otwarcie pliku
  FILE *fp = (argc == 2) ? fopen(argv[1], "r") : NULL;
  if (fp == NULL) {
    printf("Błąd wczytywania pliku, podaj ścieżkę jako argument programu.\n");
    return 1;
  }

  // Liczenie długości pliku / potrzebnej tablicy
  int arrayLength = 1; // Ostatnia linia jest zakończona EOF zamiast '\n'
  for (char c = getc(fp); c != EOF; c = getc(fp)) {
    if (c == '\n') arrayLength++;
  }

  if (arrayLength < 1) {
    printf("Podany plik ma zły format.\n");
    return 2;
  }
  printf("%d\n", arrayLength);
  // Przygotowanie tablicy na dane
  int *array = calloc(arrayLength, sizeof(int));

  // Wczytywanie danych
  rewind(fp);
  for (int i = 0; i < arrayLength; i++) {
    fscanf(fp, "%d\n", &(array[i]));
  }

  printArray(array, arrayLength);

  // Zapis do pliku
  fp = fopen("QuickSort.txt", "w");
  for (int i = 0; i < arrayLength; i++) {
    fprintf(fp, "%d\n", array[i]);
  }

  fclose(fp);
  return 0;
}
