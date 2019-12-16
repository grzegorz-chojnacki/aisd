#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************************************/
/*                                   */
/*     Wariant zadania: [U + OK]     */
/*                                   */
/*************************************/

// Zmienna globalna dla operacji modulo funkcji hashujących
int SIZE = 0;

enum State { EMPTY, OCCUPIED, DELETED };

typedef struct Key {
  char *content;
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
    temp = (s[i] * 256);
    // Sprawdź czy skok 'i += 2' nie przeskoczył ostatniej litery
    // Jeżeli nie, to kontynuuj algorytm, jeżeli tak, to zwróć ostatni XOR
    if (i + 1 < strlen(s))
      temp += s[i + 1];
    else
      return code ^ temp;
  }
  return code;
}

int h1(unsigned long int k) { return k % SIZE; }

int h2(unsigned long int k) { return 1 + (k % (SIZE - 2)); }

int h_linear(unsigned long int k, int i) { return (h1(k) + i) % SIZE; }

int h_quadratic(unsigned long int k, int i) { return (h1(k) + i * i) % SIZE; }

int h_double(unsigned long int k, int i) { return (h1(k) + i * h2(k)) % SIZE; }

HashTable *init(int size) {
  HashTable *T = calloc(1, sizeof(HashTable));
  T->length = size;
  T->keys = (Key **)calloc(size, sizeof(Key));
  for (int i = 0; i < T->length; i++) {
    Key *newKey = (Key *)calloc(1, sizeof(Key));
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

// Wyświetl tę komórki tablicy, które zaweirają jakieś dane
void print(HashTable *T) {
  printf("HashTable:\n");
  if (T->length == 0) {
    printf("    (Pusta)\n\n");
    return;
  }

  for (int i = 0; i < T->length; i++) {
    if (T->keys[i]->state == OCCUPIED) {
      printf("  %4d: %s\n", i, T->keys[i]->content);
    }
  }
  printf("\n");
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
        printf("  %4d: %s\n", i, T->keys[i]->content);
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

  int numberOfDeleted = 0;
  for (int i = 0; i < T->length; i++) {
    if (T->keys[i]->state == DELETED) numberOfDeleted++;
  }
  printf("    Pierwotnie usunieto %d z %d elementow\n", (int)(T->length * 0.8),
         T->length);
  printf("    Liczba pozostalych usunietych elementow: %d\n", numberOfDeleted);
  printf("\n");
}

void insert(HashTable *T, int hashFunction(unsigned long int, int), char *key) {
  int index;
  for (int i = 0; i < T->length; i++) {
    index = hashFunction(getStringCode(key), i);
    if (T->keys[index]->state != OCCUPIED) {
      T->keys[index]->content = key;
      T->keys[index]->state = OCCUPIED;
      return;
    }
  }
  // printf("Zabrakło wolnych komórek dla funkcji hashującej\n");
  // exit(2);
}

void removeHalf(HashTable *T) {
  for (int i = 0; i < T->length; i++) {
    if (i % 2 == 0) {
      T->keys[i]->content = NULL;
      T->keys[i]->state = DELETED;
    }
  }
}

void test(HashTable *T, FILE *fp, int currentSize) {
  SIZE = currentSize;
  char *string;
  int dummy;
  printf("SIZE = %d\n", SIZE);
  T = init(SIZE);
  // Wczytywanie danych aż do wypełenienia tablicy w 80%
  for (int i = 0; i < (SIZE * 0.8); i++) {
    string = calloc(20, sizeof(char));
    fscanf(fp, "%d %s\n", &dummy, string);
    insert(T, h_quadratic, string);
  }

  // Usuwanie połowy elementów
  removeHalf(T);
  // Ponowne wczytywanie danych (do 80%)
  for (int i = 0; i < (SIZE * 0.4); i++) {
    string = calloc(20, sizeof(char));
    fscanf(fp, "%d %s\n", &dummy, string);
    insert(T, h_quadratic, string);
  }

  printStats(T);

  rewind(fp);
  clear(T);
}

int main() {
  FILE *fp = fopen("nazwiskaASCII.txt", "r");
  if (fp == NULL) {
    printf("Nie znaleziono pliku z nazwiskami");
    return 1;
  }

  int favorableSizes[] = {2521, 4093, 7559};
  int unfavorableSizes[] = {2520, 4096, 7560};
  HashTable *T;

  // Testy dla rozmiarów sprzyjających
  printf("### Rozmiar tablicy sprzyjajacy (liczby pierwsze):\n");
  for (int i = 0; i < 3; i++) {
    test(T, fp, favorableSizes[i]);
  }
  printf("\n");
  printf("\n");

  // Testy dla rozmiarów niesprzyjających
  printf("### Rozmiar tablicy niesprzyjajacy (liczby zlozone):\n");
  for (int i = 0; i < 3; i++) {
    test(T, fp, unfavorableSizes[i]);
  }

  fclose(fp);
  return 0;
}