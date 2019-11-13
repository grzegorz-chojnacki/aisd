#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *content;
  int length;
} Word;



// void countingSort(Array *A, Array *B, int range) {
//   Array C;
//   C.length = range;
//   C.data = callolc
//   for (int i = 0; i < range; i++) {

//   }
// }


int main() {
  FILE *fp = fopen("nazwiskaASCII.txt", "r");
  if (fp == NULL) {
    printf("Nie znaleziono pliku z nazwiskami");
    return 1;
  }

  int numberOfLines = 1;
  int wordLength = 0;
  int longestWordLength = 0;

  // Zliczanie ilości nazwisk w pliku i ich najdłuższej możliwej długości
  for (char c = getc(fp); c != EOF; c = getc(fp)) {
    if (c == '\n') {
      numberOfLines++;
      if (wordLength > longestWordLength) {
        longestWordLength = wordLength;
      }
      wordLength = 0;
    } else {
      wordLength++;
    }
  }
  rewind(fp);

  // Tablica nazwisk
  Word *names = (Word *) calloc(numberOfLines, sizeof(Word));

  // Wczytywanie nazwisk
  for (int i = 0; i < numberOfLines; i++) {
    char *word = (char *) calloc(longestWordLength + 1, sizeof(char));
    fscanf(fp, "%s\n", word);
    int wordLength = strlen(word);

    names[i].content = (char *) calloc(wordLength + 1, sizeof(char));
    strcpy(names[i].content, word);
    names[i].length = wordLength;
  }

  for (int i = 0; i < 100; i++) {
    printf("%s %d\n", names[i].content, names[i].length);
  }

  return 0;
}