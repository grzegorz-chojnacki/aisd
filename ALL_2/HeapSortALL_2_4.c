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

int get(int index) { return index - 1; }

void Heapify(heap *A, int i) {
  /**
   * Wersja Rekurencyjna:
   */
  // int left = 2 * i;
  // int rigth = 2 * i + 1;
  // int smallest;

  // if (left <= A->heapSize && A->content[get(left)] < A->content[get(i)]) {
  //   smallest = left;
  // } else {
  //   smallest = i;
  // }

  // if (rigth <= A->heapSize &&
  //     A->content[get(rigth)] < A->content[get(smallest)]) {
  //   smallest = rigth;
  // }

  // if (smallest != i) {
  //   swap(&(A->content[get(i)]), &(A->content[get(smallest)]));
  //   Heapify(A, smallest);
  // }

  /**
   * Wersja Iteracyjna:
   */
  int left;
  int rigth;
  int smallest;

  while (1) {
    left = 2 * i;
    rigth = 2 * i + 1;

    if (left <= A->heapSize && A->content[get(left)] < A->content[get(i)]) {
      smallest = left;
    } else {
      smallest = i;
    }

    if (rigth <= A->heapSize &&
        A->content[get(rigth)] < A->content[get(smallest)]) {
      smallest = rigth;
    }

    if (smallest == i) return;
      swap(&(A->content[get(i)]), &(A->content[get(smallest)]));
      i = smallest;
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
  // Przygotowanie pliku
  char fileName[64];

  printf("Podaj nazwę pliku z liczbami: ");
  scanf("%s", &fileName);

  FILE *fp = fopen(fileName, "r");
  if (fp == NULL) {
    printf("Błąd wczytywania pliku\n");
    return 1;
  }

  // Przygotowanie kopca
  heap A;
  A.length = 1;

  for (char c = getc(fp); c != EOF; c = getc(fp)) {
    if (c == '\n') A.length++;
  }

  A.content = calloc(A.length, sizeof(int));

  // Wczytywanie danych
  rewind(fp);
  for (int i = 0; i < A.length; i++) {
    fscanf(fp, "%d\n", &(A.content[i]));
  }

  HeapSort(&A);
  // printHeap(&A);

  // Zapis do pliku
  fp = fopen("HeapSort.txt", "w");

  for (int i = 0; i < A.length; i++) {
    fprintf(fp, "%d\n", A.content[i]);
  }

  fclose(fp);
  return 0;
}
