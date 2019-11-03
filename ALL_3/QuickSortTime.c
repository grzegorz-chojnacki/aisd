#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define MLD 1000000000.0

// Stała decydująca o zmiania algorytmu sortowania na bąbelkowe
int threshold;

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

void bubbleSort(int *A, int leftBound, int rightBound) {
  for (int i = 0; i < rightBound; i++)
    for (int j = leftBound; j < rightBound - i; j++)
      if (A[get(j)] > A[get(j + 1)]) swap(&A[get(j)], &A[get(j + 1)]);
}

int Partition(int *A, int leftBound, int rightBound) {
  int pivot = A[get(rightBound)];
  int i = leftBound - 1;
  for (int j = leftBound; j <= rightBound; j++) {
    if (A[get(j)] <= pivot) {
      i++;
      swap(&A[get(i)], &A[get(j)]);
    }
  }
  if (i < rightBound)
    return i;
  else
    return i - 1;
}

// Quicksort podstawowy
void Quicksort(int *A, int leftBound, int rightBound) {
  if (leftBound < rightBound) {
    int division = Partition(A, leftBound, rightBound);
    Quicksort(A, leftBound, division);
    Quicksort(A, division + 1, rightBound);
  }
}

// Quicksort zmodyfikowany
void QuickBubbleSort(int *A, int leftBound, int rightBound) {
  // threshold - stała która decyduje o zmiania algorytmu sortowania
  if (rightBound - leftBound + 1 < threshold) {
    bubbleSort(A, leftBound, rightBound);
  } else {
    int division = Partition(A, leftBound, rightBound);
    QuickBubbleSort(A, leftBound, division);
    QuickBubbleSort(A, division + 1, rightBound);
  }
}

// Wypełnij tablice array liczbami z przedziału 1 - 100
void fillArrayRandom(int *array, int arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    array[i] = rand() % 100 + 1;
  }
}

// Wypełnij tablice array malejącymi liczbami
void fillArrayDeclining(int *array, int arrayLength) {
  array[arrayLength - 1] = 1;
  for (int i = arrayLength - 2; i >= 0; i--) {
    array[i] = array[i + 1] + (rand() % 4 + 1);
  }
}

int main(int argc, char **argv) {
  struct timespec tp0, tp1;
  int arrayLength = 1000; // Rozmiar testów
  int numberOfTests = arrayLength * 2;   // Liczba testów
  threshold = 20;
  double totalTime1 = 0;
  double totalTime2 = 0;
  int *array1 = calloc(arrayLength, sizeof(int));
  int *array2 = calloc(arrayLength, sizeof(int));
  srand(time(0));

  // Powtarzanie testów, wyliczanie średniego czasu
  for (int i = 0; i < numberOfTests; i++) {
    fillArrayRandom(array1, arrayLength);
    // fillArrayDeclining(array1, arrayLength);

    // Duplikowanie zawartości array1 do array2
    for (int j = 0; j < arrayLength; j++) {
      array2[j] = array1[j];
    }

    // Pomiar czasu algorytmu podstawowego:
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp0);
    Quicksort(array1, 1, arrayLength);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp1);
    totalTime1 +=
      (tp1.tv_sec + tp1.tv_nsec / MLD) - (tp0.tv_sec + tp0.tv_nsec / MLD);

    // Pomiar czasu algorytmu zmodyfikowanego:
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp0);
    QuickBubbleSort(array2, 1, arrayLength);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp1);
    totalTime2 +=
      (tp1.tv_sec + tp1.tv_nsec / MLD) - (tp0.tv_sec + tp0.tv_nsec / MLD);
  }
  printf("Podstawowy:    %lf \n", totalTime1 / numberOfTests);
  printf("Zmodyfikowany: %lf \n", totalTime2 / numberOfTests);

  return 0;
}
