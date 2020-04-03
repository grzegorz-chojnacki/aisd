#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Zakres Ascii dla tablicy liczników "C"
#define RANGE 256

typedef struct Word {
  char *content;
  int length;
} Word;

typedef struct Array {
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

// Zmienna letter jest indeksem litery w napisie (0 ... n - 1)
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
  for (int i = A->longestWordLength; i >= 0; i--) {
    countingSort(A, B, i);
    // Zapisywanie pośrednio posortowanych danych do pierwotnej tablicy
    for (int i = 0; i < A->length; i++) {
      A->items[i] = B[i];
    }
  }
}

// Sprawdza też, czy plik zawiera znaki spoza zakresu ASCII
void setAttributes(FILE *fp, Array *A) {
  A->length = 0;
  A->longestWordLength = 0;

  int wordLength = 0;
  // Zliczanie ilości nazwisk w pliku i ich najdłuższej możliwej długości
  for (char c = getc(fp); c != EOF; c = getc(fp)) {
    if (c < 0 || c >= 256) {
      printf("Plik zawiera znaki spoza zestawu ASCII\n");
      exit(2);
    }
    if (c == '\n') {
      A->length++;
      if (wordLength > A->longestWordLength) {
        A->longestWordLength = wordLength;
      }
      wordLength = 0;
    } else {
      wordLength++;
    }
  }
  rewind(fp);
  A->items = (Word *)calloc(A->length, sizeof(Word));
}

void loadNames(FILE *fp, Array *A) {
  int wordLength;
  char *word = (char *)calloc(A->longestWordLength, sizeof(char));
  
  for (int i = 0; i < A->length; i++) {
    fscanf(fp, "%s\n", word);
    wordLength = strlen(word);

    A->items[i].content = (char *)calloc(wordLength, sizeof(char));
    A->items[i].length = wordLength;
    strcpy(A->items[i].content, word);
  }
}

int main() {
  FILE *fp = fopen("nazwiskaASCII.txt", "r");
  if (fp == NULL) {
    printf("Nie znaleziono pliku z nazwiskami");
    return 1;
  }

  // Tablica na nazwiska
  Array names;

  // Wczytywanie
  setAttributes(fp, &names);
  loadNames(fp, &names);
  fclose(fp);

  // Sortowanie
  RadixSort(&names);

  // Zapis do pliku
  fp = fopen("RadixSort.txt", "w");
  for (int i = 0; i < names.length; i++) {
    fprintf(fp, "%s\n", names.items[i].content);
  }

  fclose(fp);
  return 0;
}