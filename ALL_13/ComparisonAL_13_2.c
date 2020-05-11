#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Stałe wartości dla funkcji haszujących
#define PRIME 27077
#define BASE 128

// Struktura upraszczająca działanie na tablicach znaków
typedef struct Array {
  char *data;
  int length;
} Array;

// Obsługa błędu podczas alokowania pamięci
void ThrowMemoryError() {
  printf("Wystąpił problem z alokacją pamięci\n");
  exit(4);
}

// Funkcja obsługująca wczytywanie pliku w całości do tablicy.
// Zwraca ilość wczytanych znaków
int withNewLines(FILE *file, char *data) {
  int i = 0;
  for (char c = fgetc(file); c != EOF; c = fgetc(file)) {
    data[i++] = c;
  }
  return i;
}

// Funkcja obsługująca wczytywanie pliku do tablicy, bez znaków nowej linii.
// Zwraca ilość wczytanych znaków
int withoutNewLines(FILE *file, char *data) {
  int i = 0;
  for (char c = fgetc(file); c != EOF; c = fgetc(file)) {
    if (c != '\n') data[i++] = c;
  }
  return i;
}

// Wczytywanie pliku tekstowego do pamięci za pomocą funkcji:
// withNewLines() lub withoutNewLines()
Array *load(FILE *file, int newLinePolicy(FILE *, char *)) {
  fseek(file, 0, SEEK_END);
  size_t fileLenght = ftell(file);
  rewind(file);
  Array *array = (Array *)calloc(1, sizeof(Array));
  char *data = (char *)calloc(fileLenght + 1, sizeof(char));

  if (array == NULL || data == NULL) ThrowMemoryError();

  // Wczytanie znaków do tablicy data
  int patternLenght = newLinePolicy(file, data);

  array->data = data;
  array->length = patternLenght;
  return array;
}

// Zwraca długość kawałka tekstu jaki dopasował się do wzorca, z uwzględnieniem
// tego, że znaki nowej linii są pomijane przy dopasowywaniu
int patternOffset(Array *text, Array *pattern, int textIndex) {
  int offset = textIndex - 1;
  int i = pattern->length - 1;
  while (i >= 0) {
    if (text->data[offset] == pattern->data[i])
      i--;
    offset--;
  }
  return textIndex - offset - 1;
}

// Funkcja generująca tablice indeksów prefiksów dla wzorca
int *prefixFunction(Array *pattern) {
  int *prefixTable = (int *)calloc(pattern->length, sizeof(int));
  if (prefixTable == NULL) ThrowMemoryError();
  int k = 0;
  for (int q = 2; q <= pattern->length; q++) {
    while ((k > 0) && (pattern->data[k] != pattern->data[q - 1])) {
      k = prefixTable[k - 1];
    }
    if (pattern->data[k] == pattern->data[q - 1]) {
      k++;
    }
    prefixTable[q - 1] = k;
  }

  return prefixTable;
}

// Wyszukiwanie wzorca w tekście przy pomocy algorytmu Knutha-Morrisa-Pratta.
// Zwraca tablice pozycji na których dopasowano wzorzec
int *KMP(Array *text, Array *pattern) {
  int *prefixTable = prefixFunction(pattern);
  int *listOfMatches = (int *)calloc(text->length, sizeof(int));
  int matchIndex = 0;

  if (listOfMatches == NULL) ThrowMemoryError();

  int q = 0;
  for (int i = 1; i <= text->length; i++) {
    // Pomiń znaki nowego wiersza przy dopasowywaniu
    while (text->data[i - 1] == '\n') {
      i++;
    }
    while ((q > 0) && (pattern->data[q] != text->data[i - 1])) {
      q = prefixTable[q - 1];
    }
    if (pattern->data[q] == text->data[i - 1]) {
      q++;
    }
    if (q == pattern->length) {
      // i jest indeksem końca znalezionego wzorca w tekście
      // Należy odjąć od niego tyle znaków ile występuje we wzorcu, ale
      // z dodatkowym uwzględnieniem znaków nowych linii
      listOfMatches[matchIndex++] = i - patternOffset(text, pattern, i) + 1;
      q = prefixTable[q - 1];
    }
  }
  return listOfMatches;
}

// Dopasowanie wzorca do tekstu (zaczynając od danego indeksu),
// z pomijaniem znaków nowego wiersza
bool isMatching(Array *text, int index, Array *pattern) {
  int charactersChecked = 0;
  for (int i = 0; charactersChecked < pattern->length; i++) {
    if (text->data[index + i] == '\n') continue;
    if (pattern->data[charactersChecked] != text->data[index + i]) return false;
    else charactersChecked++;
  }
  return true;
}

// Funkcja dodająca do hasza kolejny znak
int hashAdd(int oldHash, char newCharacter) {
  return (oldHash * BASE + newCharacter) % PRIME;
}

// Funkcja odejmująca od hasza dany znak, zgodnie z jego wagą określoną przez
// power - w funkcji RB usuwa najstarszy znak z obecnego hasza
int hashRemove(int oldHash, int power, char oldCharacter) {
  oldHash = (oldHash - power * oldCharacter) % PRIME;
  if (oldHash < 0) return oldHash + PRIME;
  else return oldHash;
}

// Funkcja haszująca, dodająca kolejne znaki do hasza
int hash(Array *array) {
  int code = 0;
  for (int i = 0; i < array->length; i++)
    code = hashAdd(code, array->data[i]);
  return code;
}

// Wyszukiwanie wzorca w tekście przy pomocy algorytmu Rabina-Karpa.
// Zwraca tablice pozycji na których dopasowano wzorzec
int *RB(Array *text, Array *pattern) {
  int *listOfMatches = (int *)calloc(text->length, sizeof(int));
  int matchIndex = 0;
  if (listOfMatches == NULL) ThrowMemoryError();

  int power = 1;
  for (int i = 0; i < pattern->length; i++)
    power = (power * BASE) % PRIME;

  int patternHash = hash(pattern);
  int textHash = 0;
  // Licznik pominiętych znaków nowej linii
  int skippedCharacters = 0;
  // Iterator który zawsze wskazuje na pierwszą literę, która jest jeszcze
  // w haszu (litera przed początkiem sprawdzanego wycinka tekstu)
  int j = 0;
  // Pominięcie ewentualnych pierwszych znaków nowej linii
  while (text->data[j] == '\n') {j++;}

  for (int i = 0; i < text->length; i++) {
    if (text->data[i] == '\n') {
      skippedCharacters++;
      continue;
    }
    // Haszowanie typu "Rolling hash"
    textHash = hashAdd(textHash, text->data[i]);
    if (i - skippedCharacters >= pattern->length) {
      textHash = hashRemove(textHash, power, text->data[j++]);
      while (text->data[j] == '\n') {j++;}
    }
    // Leniwe sprawdzanie dokładnego dopasowania
    if (textHash == patternHash && isMatching(text, j, pattern)) {
      listOfMatches[matchIndex++] = j + 1;
    }
  }

  return listOfMatches;
}

// Wyszukiwanie wzorca w tekście przy pomocy algorytmu naiwnego.
// Zwraca tablice pozycji na których dopasowano wzorzec
int *N(Array *text, Array *pattern) {
  int *listOfMatches = (int *)calloc(text->length, sizeof(int));
  int matchIndex = 0;
  if (listOfMatches == NULL) ThrowMemoryError();

  for (int i = 0; i < (text->length - pattern->length); i++) {
    if (text->data[i] == '\n') continue;
    if (isMatching(text, i, pattern)) {
      listOfMatches[matchIndex++] = i + 1;
    }
  }
  return listOfMatches;
}

void printResults(Array *text, int *listOfMatches) {
  if (listOfMatches[0] == 0) {
    printf("Nie znaleziono żadnych dopasowań\n\n");
    return;
  }
  printf("Znaleziono dopasowania na następujących pozycjach:\n"
         "(Numer wiersza i znaku numerowane od 1)\n");
  int matchIndex = 0;
  int lineNumber = 1;
  int charactersInLine = 1;
  for (int i = 0; i < text->length; i++) {
    if (text->data[i] == '\n') {
      lineNumber++;
      charactersInLine = 0;
    }
    if (i == listOfMatches[matchIndex] - 1) {
      printf("- %d : %d\n", lineNumber, charactersInLine);
      matchIndex++;
      if (listOfMatches[matchIndex] == 0) break;
    }
    charactersInLine++;
  }
  printf("------------------\n"
         "Znaleziono łącznie %d dopasowań\n\n", matchIndex);
}

double timeTest(Array *text, Array *pattern, int *searchFunction(Array*, Array*)) {
  clock_t start, end;
  start = clock();
  searchFunction(text, pattern);
  end = clock();
  return ((double) (end - start) / CLOCKS_PER_SEC);
}

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
    printf("Wystąpił błąd podczas otwierania pliku ze wzorcem\n");
    return 2;
  }
  if (fileT == NULL) {
    printf("Wystąpił błąd podczas otwierania pliku z tekstem\n");
    return 3;
  }

  // Wczytywanie plików
  Array *pattern = load(fileP, withoutNewLines);
  fclose(fileP);
  Array *text = load(fileT, withNewLines);
  fclose(fileT);

  printf("### Test poprawności działania algorytmu Knutha-Morrisa-Pratta ###\n");
  printResults(text, KMP(text, pattern));
  printf("### Test poprawności działania algorytmu Rabina-Karpa ###\n");
  printResults(text, RB(text, pattern));
  printf("### Test poprawności działania algorytmu naiwnego ###\n");
  printResults(text, N(text, pattern));

  printf("### Test pomiaru czasu działania algorytmów ###\n");
  printf("1) Test algorytmu Knutha-Morrisa-Pratta:\n");
  printf(" ~ %fs\n", timeTest(text, pattern, KMP));
  printf("2) Test algorytmu Rabina-Karpa:\n");
  printf(" ~ %fs\n", timeTest(text, pattern, RB));
  printf("3) Test algorytmu naiwnego:\n");
  printf(" ~ %fs\n", timeTest(text, pattern, N));
  printf("\n");
  return 0;
}
