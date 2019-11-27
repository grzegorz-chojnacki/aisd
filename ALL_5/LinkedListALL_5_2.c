#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NIL NULL

typedef struct List {
  struct List *next;
  struct List *prev;
  char *key;
} List;

// Inicjalizacja wartownika listy
List *init() {
  List *L = (List *)calloc(1, sizeof(List));
  L->key = NIL;
  L->next = L;
  L->prev = L;
  return L;
}

// Znalezienie i pobranie wskaźnika na pierwszy element o zadanym kluczu "s"
List *get(List *L, char *s) {
  List *node = L->next;
  while (node != L && strcmp(node->key, s) != 0) {
    node = node->next;
  }
  return node;
}

void reverse(List *L) {
  List *tmp;
  for (List *node = L->next; node != L; node = node->prev) {
    tmp = node->next;
    node->next = node->prev;
    node->prev = tmp;
  }
  tmp = L->next;
  L->next = L->prev;
  L->prev = tmp;
}

// SZUKAJ(s, L) wskaźnik na słowo lub NIL
char *search(List *L, char *s) { return get(L, s)->key; }

// DRUKUJ(L)
void print(List *L) {
  printf("List(");
  for (List *node = L->next; node != L; node = node->next) {
    if (node->next != L) {
      printf("%s, ", node->key);
    } else {
      printf("%s", node->key);
    }
  }
  printf(")\n");
}

// WSTAW(s, L) - na początek listy
void add(List *L, char *s) {
  List *newItem = (List *)calloc(1, sizeof(List));
  newItem->key = s;
  newItem->next = L->next;
  newItem->prev = L;

  // Lista zawierająca tylko wartownika
  if (L->next == L) {
    L->prev = newItem;
  } else {
    L->next->prev = newItem;
  }
  L->next = newItem;
}

// USUŃ(s, L)
void delete (List *L, char *s) {
  List *node = get(L, s);
  if (node == L) return;

  node->prev->next = node->next;
  node->next->prev = node->prev;
}

// BEZPOWTÓRZEŃ(L)
List *cleanRepeats(List *L1) {
  List *L2 = init();
  for (List *node = L1->next; node != L1; node = node->next) {
    // Dodaj element do L2, o ile jeszcze go tam nie ma
    if (search(L2, node->key) == NIL) {
      char *keyCopy = (char *)calloc(strlen(node->key) + 1, sizeof(char));
      keyCopy = strcpy(keyCopy, node->key);
      add(L2, keyCopy);
    }
  }
  reverse(L2);
  return L2;
}

// SCAL(L1, L2) - dla dwóch list *rozłącznych*
List *merge(List *L1, List *L2) {
  List *L3 = init();

  // Powiązanie nowego wartownika z listą L1:
  L3->next = L1->next;

  // Powiązania ostatniego elementu L3 i pierwszego L2:
  L1->prev->next = L2->next;
  L2->next->prev = L3->prev;

  // Powiązania ostatniego elemetnu L2 i nowego wartownika:
  L2->prev->next = L3;
  L3->prev = L2->prev;

  // Usuwanie powiązań z elementami z obydwu list źródłowych:
  L1->next = L1;
  L1->prev = L1;
  L2->next = L2;
  L2->prev = L2;
  return L3;
}

// KASUJ(L)
void clear(List *L) {
  L->next = L;
  L->prev = L;
}

int main() {
  List *list = init();
  // Wypełnienie listy danymi do testów
  {
    add(list, "quam");
    add(list, "Lorem");
    add(list, "ipsum");
    add(list, "dolor");
    add(list, "sit");
    add(list, "Lorem");
    add(list, "Lorem");
    add(list, "quam");
    add(list, "Nulla");
    add(list, "quam");
    add(list, "Lorem");
    add(list, "suscipit");
  }

  // Test wstawiania i wyświetlania:
  printf("Wstawianie:\n  ");
  print(list);
  printf("\n");

  // Test przeszukiwania:
  printf("Przeszukiwanie: - (null) jeśli nie znaleziono\n");
  printf("  Czy wyraz \"dolor\" jest w liście? - %s \n", search(list, "dolor"));
  printf("  Czy wyraz \"TEST\" jest w liście? - %s \n", search(list, "TEST"));
  printf("\n");

  // Test Usuwania
  printf("Usuwanie:\n");
  printf("- wyrazu \"quam\":\n  ");
  delete (list, "quam");
  print(list);
  printf("- wyrazu \"TEST\":\n  ");
  delete (list, "TEST");
  print(list);
  printf("\n");

  // Test usuwania powtórzeń:
  printf("Usuwanie powtórzeń:\n  ");
  List *cleanList = cleanRepeats(list);
  print(cleanList);
  printf("\n");

  // Test scalania dwóch list:
  printf(
      "Scalanie dwóch list: (lista pierwotna i jej kopia bez powtórzeń)\n  ");
  List *mergedList = merge(list, cleanList);
  print(mergedList);
  printf("Listy użyte do scalenia:\n  ");
  print(list);
  printf("  ");
  print(cleanList);
  printf("\n");

  // Test kasowania list:
  printf("Kasowanie listy:\n  ");
  clear(mergedList);
  print(mergedList);
  printf("\n");

  return 0;
}
