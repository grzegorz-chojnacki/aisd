#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RANGE 128

typedef struct {
  char *content;
  int length;
} Word;

typedef struct {
  Word *items;
  int length;
  int longestWordLength;
} Array;

int getSmallCase(char c) {
  if ('A' <= c && c <= 'Z')
    return c + 32;
  else
    return c;
}

// letter (0 <-> wordLength - 1)
void countingSort(Array *A, Word *B, int letter) {
  int *C = (int *)calloc(RANGE, sizeof(int));

  for (int i = 0; i < A->length; i++) {
    // Jeśli nazwisko będzie tu krótsze niż sprawdzane miejsce litery,
    // to przyjmujemy, że ma ono wagę 0
    if (letter < A->items[i].length) {
      C[getSmallCase(A->items[i].content[letter])]++;
    } else {
      C[0]++;
    }
  }

  for (int i = 1; i < RANGE; i++) {
    C[i] += C[i - 1];
  }

  // Kolejność linii w pętli jest odwrotna, dzięki czemu algorytm działa na
  // tablicach numerowanych od 0 (offset - 1)
  for (int i = A->length - 1; i >= 0; i--) {
    if (letter < A->items[i].length) {
      C[getSmallCase(A->items[i].content[letter])]--;
      B[C[getSmallCase(A->items[i].content[letter])]] = A->items[i];
    } else {
      C[0]--;
      B[C[0]] = A->items[i];
    }
  }
  free(C);
}

void RadixSort(Array *A) {
  Word *B = (Word *)calloc(A->length, sizeof(Word));
  for (int i = A->longestWordLength - 1; i >= 0; i--) {
    countingSort(A, B, i);
    // Zapisywanie pośrednio posortowanych danych do pierwotnej tablicy
    for (int i = 0; i < A->length; i++) {
      A->items[i] = B[i];
    }
  }
}

int main() {
  FILE *fp = fopen("nazwiskaASCII.txt", "r");
  if (fp == NULL) {
    printf("Nie znaleziono pliku z nazwiskami");
    return 1;
  }

  // Tablice na nazwiska
  Array names;

  names.length = 0;
  names.longestWordLength = 0;
  int wordLength = 0;

  // Zliczanie ilości nazwisk w pliku i ich najdłuższej możliwej długości
  for (char c = getc(fp); c != EOF; c = getc(fp)) {
    if (c == '\n') {
      names.length++;
      if (wordLength > names.longestWordLength) {
        names.longestWordLength = wordLength;
      }
      wordLength = 0;
    } else {
      wordLength++;
    }
  }
  rewind(fp);

  names.items = (Word *)calloc(names.length, sizeof(Word));

  // Wczytywanie nazwisk
  for (int i = 0; i < names.length; i++) {
    char *word = (char *)calloc(names.longestWordLength + 1, sizeof(char));
    fscanf(fp, "%s\n", word);
    int wordLength = strlen(word);
    names.items[i].content = (char *)calloc(wordLength + 1, sizeof(char));
    strcpy(names.items[i].content, word);
    names.items[i].length = wordLength;
  }

  // Sortowanie
  RadixSort(&names);
  fclose(fp);

  fp = fopen("RadixSort.txt", "w");
  for (int i = 0; i < names.length; i++) {
    fprintf(fp, "%s\n", names.items[i]);
  }
  fclose(fp);

  return 0;
}