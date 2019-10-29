#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *content;
  int length;
  int heapSize;
} heap;

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void printHeap(heap *A) {
  printf("Heap(");
  for (int i = 0; i < A->length; i++) {
    if (i < A->length - 1) {
      printf("%d, ", A->content[i]);
    } else {
      printf("%d)\n", A->content[i]);
    }
  }
}

// Zamienia indeks tablicowy z konwencji [1 ... n] na [0 ... n - 1]
int get(int index) { return index - 1; }

void Heapify(heap *A, int i) {
  int left = 2 * i;
  int rigth = 2 * i + 1;
  int smallest;

  if (left <= A->heapSize && 
      A->content[get(left)] < A->content[get(i)]) {
    smallest = left;
  } else {
    smallest = i;
  }

  if (rigth <= A->heapSize &&
      A->content[get(rigth)] < A->content[get(smallest)]) {
    smallest = rigth;
  }

  if (smallest != i) {
    swap(&(A->content[get(i)]), &(A->content[get(smallest)]));
    Heapify(A, smallest);
  }
}

void BuildHeap(heap *A) {
  A->heapSize = A->length;
  for (int i = (A->length / 2); i >= 1; i--) {
    Heapify(A, i);
  }
}

void HeapSort(heap *A) {
  BuildHeap(A);
  for (int i = A->length; i >= 2; i--) {
    swap(&(A->content[get(A->heapSize)]), &(A->content[get(1)]));
    A->heapSize--;
    Heapify(A, 1);
  }
}

int main(int argc, char **argv) {
  // Otwarcie pliku
  FILE *fp = (argc == 2) ? fopen(argv[1], "r") : NULL;
  if (fp == NULL) {
    printf("Błąd wczytywania pliku, podaj ścieżkę jako argument programu.\n");
    return 1;
  }

  // Liczenie długości pliku
  int fileLength = 1; // Ostatnia linia jest zakończona EOF zamiast '\n'
  for (char c = getc(fp); c != EOF; c = getc(fp)) {
    if (c == '\n') fileLength++;
  }

  if (fileLength < 3) { // 2 liczby zakresu i minimum jedna do tablicy
    printf("Podany plik ma zły format.\n");
    return 2;
  }

  heap A;
  int lowerBound;
  int upperBound;
  int arrayLength = fileLength - 2;

  // Długość tablicy na kopiec, bez dwóch pierwszych elementów
  A.content = calloc(arrayLength, sizeof(int));

  // Wczytywanie wyznaczników zakresu
  rewind(fp);
  fscanf(fp, "%d\n", &lowerBound);
  fscanf(fp, "%d\n", &upperBound);

  // Wczytywanie danych
  for (int i = 0; i < arrayLength; i++) {
    fscanf(fp, "%d\n", &(A.content[i]));
  }

  // Ustawienie granic dla sortowania
  A.length = upperBound - lowerBound + 1; // ± 1 - wynika z konwencji 
  A.content = A.content + lowerBound - 1; //       numerowania tablicy od 1

  if (lowerBound < 1 ||
      lowerBound > upperBound ||
      upperBound > arrayLength) {
    printf("Podany zakres ma zły format.\n");
    return 3;
  }

  HeapSort(&A);

  // Przywrócenie oryginalnej dłogości tablicy
  A.length = arrayLength;
  A.content = A.content - lowerBound + 1;
  printHeap(&A);

  // Zapis do pliku
  fp = fopen("HeapSort.txt", "w");
  for (int i = 0; i < A.length; i++) {
    fprintf(fp, "%d\n", A.content[i]);
  }

  fclose(fp);
  return 0;
}
