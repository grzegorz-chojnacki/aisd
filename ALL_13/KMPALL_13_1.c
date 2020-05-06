#include <stdio.h>
#include <stdlib.h>

#include <string.h>

// Zmodyfikuj algorytm Knutha-Morrisa-Pratta tak, aby wyszukiwał w tekście T
// dany wzorzec P przy założeniu, że każdy znak zapytania ’?’ w tekście T może
// odpowiadać dowolnemu znakowi, a następnie zaimplementuj tę modyfikację.

typedef struct Array {
  char *data;
  int length;
} Array;

// Wczytywanie pliku do pamięci, w celu zmniejszenia czasu dostępu do znaków
Array *load(FILE *file) {
  fseek(file, 0, SEEK_END);
  size_t fileLenght = ftell(file);
  rewind(file);
  Array *array = (Array *)calloc(1, sizeof(Array));
  char *data = (char *)calloc(fileLenght + 1, sizeof(char));

  if (array == NULL || data == NULL) {
    printf("Wystąpił problem z alokacją pamięci\n");
    exit(4);
  }

  int i = 0;
  for (char c = fgetc(file); c != EOF; c = fgetc(file)) {
    data[i++] = c;
  }

  array->data = data;
  array->length = fileLenght;
  return array;
}

// Funkcja generująca tablice indeksów prefiksów, domyślnie wyzerowana
int *prefixFunction(Array *predicate) {
  int *indices = (int *)calloc(predicate->length, sizeof(int));

  int k = 0;
  for (int q = 2; q <= predicate->length; q++) {
    while ((k > 0) && (predicate->data[k] != predicate->data[q - 1])) {
      k = indices[k - 1];
    }
    if (predicate->data[k] == predicate->data[q - 1]) k++;
    indices[q - 1] = k;
  }

  return indices;
}

void KMP(Array *text, Array *predicate) {}

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
  // Array *predicate = load(fileP);
  // Array *text = load(fileT);
  char test[] = {'b', 'b', 'b', 'a', 'b', 'b', 0};
  Array *predicate = (Array *)calloc(1, sizeof(Array));
  predicate->data = test;
  predicate->length = strlen(test);

  int *indices = prefixFunction(predicate);
  printf("%s\n", predicate->data);
  for (int i = 0; i < predicate->length; i++) {
    printf("%i", indices[i]);
  }
  printf("\n");

  // KMP(text, predicate);
  return 0;
}
