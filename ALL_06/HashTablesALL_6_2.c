#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Zmienna globalna dla operacji modulo funkcji hashujących
int SIZE = 0;

enum State { EMPTY, OCCUPIED, DELETED };

typedef struct Key {
  char *content;
  int n;  // Liczba służąca do testów
  enum State state;
} Key;

typedef struct HashTable {
  Key **keys;
  int length;
} HashTable;

char *toString(enum State value) {
  switch (value) {
    case EMPTY:
      return "-----";
    case OCCUPIED:
      return "(Occupied)";
    case DELETED:
      return "-xxx-";
    default:
      return "Error";
  }
}

// abcdef... -> ((256 · a + b) XOR (256 · c + d)) XOR (256 · e + f)...
unsigned long int getStringCode(char *s) {
  int code = 0;
  int temp = 0;
  for (int i = 0; i <= strlen(s); i += 2) {
    code ^= temp;
    temp = (s[i] * 111);
    // Sprawdź czy skok 'i += 2' nie przeskoczył ostatniej litery
    // Jeżeli nie, to kontynuuj algorytm, jeżeli tak, to zwróć ostatni XOR
    if (i + 1 < strlen(s)) temp += s[i + 1];
    else return code ^ temp;
  }
  return code;
}

HashTable *init(int size) {
  HashTable *T = calloc(1, sizeof(HashTable));
  T->length = size;
  T->keys = (Key **)calloc(size, sizeof(Key));
  for (int i = 0; i < T->length; i++) {
    Key *newKey = (Key *)calloc(1, sizeof(Key));
    newKey->n = 0;
    newKey->state = EMPTY;
    T->keys[i] = newKey;
  }

  return T;
}

void clear(HashTable *T) {
  for (int i = 0; i < T->length; i++) {
    free(T->keys[i]->content);
    free(T->keys[i]);
  }
  free(T->keys);
  T->length = 0;
}

// Wyświetl wszystkie komórki tablicy i krotność prób ich zapisań (n)
void printAll(HashTable *T) {
  printf("HashTable:\n");
  if (T->length == 0) {
    printf("    (Pusta)\n\n");
    return;
  }

  for (int i = 0; i < T->length; i++) {
    switch (T->keys[i]->state) {
      case OCCUPIED:
        printf("  %4d: %-20s - n: %d\n", i, T->keys[i]->content, T->keys[i]->n);
        break;
      case DELETED:
        printf("  %4d: %s\n", i, toString(DELETED));
        break;
      case EMPTY:
        printf("  %4d: %s\n", i, toString(EMPTY));
        break;
    }
  }
  printf("\n");
}

// Oblicz i wyświetl statystyki dla danej tablicy
void printStats(HashTable *T) {
  printf("Statystyki HashTable:\n");
  if (T->length == 0) {
    printf("    (Pusta)\n\n");
    return;
  }

  int zeroCounter = 0;
  int max = T->keys[0]->n;
  int sum = 0;
  int nonZeroCounter = 0;
  for (int i = 0; i < T->length; i++) {
    if (T->keys[i]->n == 0)
      zeroCounter++;
    else {
      sum += T->keys[i]->n;
      nonZeroCounter++;
    }
    if (T->keys[i]->n > max) max = T->keys[i]->n;
  }
  double avg = (nonZeroCounter != 0) ? (double)sum / (double)nonZeroCounter : 0;
  printf(
      "- Ilosc zerowych pozycji w tablicy T: %d\n"
      "- Maksymalna wartosc w T: %d\n"
      "- Srednia wartosc pozycji niezerowych: %f\n",
      zeroCounter, max, avg);
  printf("\n");
}

void insert(HashTable *T, char *key) {
  int index = getStringCode(key) % T->length;
  T->keys[index]->state = OCCUPIED;
  T->keys[index]->n++;
}

void test(HashTable *T, FILE *fp, int currentSize) {
  SIZE = currentSize;
  char *string;
  printf("SIZE = %d\n", SIZE);
  T = init(SIZE);
  // Wczytywanie 2 * SIZE danych do tablicy
  for (int i = 0; i < SIZE * 2; i++) {
    string = calloc(20, sizeof(char));
    fscanf(fp, "%s\n", string);
    insert(T, string);
  }
  rewind(fp);

  printStats(T);
  clear(T);
}

int main() {
  FILE *fp = fopen("3700.txt", "r");
  if (fp == NULL) {
    printf("Nie znaleziono pliku z danymi");
    return 1;
  }

  int favorableSizes[] = {1021, 1931, 2521};
  int unfavorableSizes[] = {1024, 1930, 2520};
  HashTable *T;

  // Testy dla rozmiarów sprzyjających
  printf("#### Rozmiar tablicy sprzyjajacy (liczby pierwsze):\n");
  for (int i = 0; i < 3; i++) {
    test(T, fp, favorableSizes[i]);
  }
  printf("\n");

  // Testy dla rozmiarów niesprzyjających
  printf("#### Rozmiar tablicy niesprzyjajacy (liczby zlozone):\n");
  for (int i = 0; i < 3; i++) {
    test(T, fp, unfavorableSizes[i]);
  }

  fclose(fp);
  return 0;
}