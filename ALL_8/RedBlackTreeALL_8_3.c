#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma region declarations
bool isPrintingAllowed = true;

enum Color { red, black };

typedef struct Node {
  int value;
  struct Node *parent;
  struct Node *left;
  struct Node *right;
  enum Color color;
} Node;

Node nilNode = {0, NULL, NULL, NULL, black};
#define NIL (&nilNode)

// Creating nodes
Node *newRedBlackTree();
Node *makeNode(Node *parent, int value);
// Getters
Node *get(Node *node, int value);
Node *getBrother(Node *node);
Node *getUncle(Node *node);
Node *inorderSuccessor(Node *node);
// Conditions
bool isValid(Node *node);
bool isTurning(Node *node);
// Utils
int minDepth(Node *node);
int maxDepth(Node *node);
int power(int a, int b);
int redNodesCount(Node *node);
int *getRandomSequence(int n);
// Transformations
void rotateLeft(Node *node);
void rotateRight(Node *node);
// Inserting
void fixCase1(Node *node);
void fixCase2(Node *node);
void fixCase3(Node *node);
void fixInsertion(Node *node);
void insertNode(Node **root, int value);
// Deleting
void deleteCase0(Node *node);
void deleteCase1(Node *node);
void deleteCase2(Node *node);
void deleteCase3(Node *node);
void deleteCase4(Node *node);
void fixDeletion(Node *node);
void destroyNode(Node *node);
void deleteNode(Node **root, int value);
// Printing
void printNode(Node *node);
void printCaseHeader(Node *node, int n);
void printNodesAtLevel(Node *node, int treeDepth, int level, int currentLevel);
void printLinesAtLevel(Node *node, int treeDepth, int level, int currentLevel);
void printLevel(Node *node, int treeDepth, int level);
void print(Node *node);
// Tests
void testInsertion(Node **tree);
void testInsertionHiddenOutput(Node **tree);
void testDeletion(Node **tree);
#pragma endregion declarations
#pragma region creatingNodes
Node *newRedBlackTree() {
  Node *rootNode = (Node *)calloc(1, sizeof(Node));
  rootNode->value = -1;
  rootNode->parent = NIL;
  rootNode->left = NIL;
  rootNode->right = NIL;
  rootNode->color = black;

  return rootNode;
}

Node *makeNode(Node *parent, int value) {
  Node *newNode = (Node *)calloc(1, sizeof(Node));
  newNode->value = value;
  newNode->parent = parent;
  newNode->left = NIL;
  newNode->right = NIL;
  newNode->color = red;

  if (value <= parent->value)
    parent->left = newNode;
  else
    parent->right = newNode;

  return newNode;
}
#pragma endregion creatingNodes
#pragma region getters
Node *get(Node *node, int value) {
  if (node == NIL) return NIL;
  if (node->value == value) return node;
  node = (value <= node->value) ? node->left : node->right;
  return get(node, value);
}

// NIL returns NIL
Node *getBrother(Node *node) {
  if (node->parent->left == node)
    return node->parent->right;
  else
    return node->parent->left;
}

Node *getUncle(Node *node) { return getBrother(node->parent); }

// Użyte tylko w przypadku drzewa z którego usuwamy węzeł z dwoma synami
Node *inorderSuccessor(Node *node) {
  for (node = node->right; node->left != NIL; node = node->left) {
  }
  return node;
}
#pragma endregion getters
#pragma region conditions
bool isValid(Node *node) {
  if (node == NIL || node->parent == NIL) return true;
  // *node jest poprawnie przyłączonym dzieckiem
  // (Tylko po jednej stronie rodzica i przyłączonym dobrze w obie strony)
  if ((node->parent->left == node) != (node->parent->right == node))
    return true && isValid(node->left) && isValid(node->right);
  else
    return false;
}

// Czy węzeł jest dzieckiem z przeciwnej strony niż jego rodzic (przypadek #2)
bool isTurning(Node *node) {
  if (node->parent->parent->left == node->parent) {
    if (node->parent->right == node)
      return true;
    else
      return false;
  } else {
    if (node->parent->left == node)
      return true;
    else
      return false;
  }
}
#pragma endregion conditions
#pragma region utils
int minDepth(Node *node) {
  if (node == NIL) return 0;
  int leftDepth = minDepth(node->left) + 1;
  int rightDepth = minDepth(node->right) + 1;
  return (leftDepth < rightDepth ? leftDepth : rightDepth);
}

int maxDepth(Node *node) {
  if (node == NIL) return 0;
  int leftDepth = maxDepth(node->left) + 1;
  int rightDepth = maxDepth(node->right) + 1;
  return (leftDepth > rightDepth ? leftDepth : rightDepth);
}

int power(int a, int b) {
  int result = 1;
  while (b-- > 0) result *= a;
  return result;
}

int redNodesCount(Node *node) {
  if (node == NIL) return 0;
  return ((node->color == red) ? 1 : 0) + redNodesCount(node->left) +
         redNodesCount(node->right);
}

int *getRandomSequence(int n) {
  srand(time(NULL));
  int *array = (int *)calloc(n, sizeof(int));
  // Generate sequence 1...n
  for (int i = 0; i < n; i++) {
    array[i] = i + 1;
  }
  // Shuffle sequence
  for (int i = 0, j = 0; i < n; i++) {
    j = rand() % (n - i) + i;
    int tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
  }
  return array;
}
#pragma endregion utils
#pragma region utils
void rotateLeft(Node *node) {
  Node *nodeLeftSubTree = node->left;
  Node *nodeParent = node->parent;
  Node *nodeGrandfather = node->parent->parent;

  // Wydostanie *node poziom wyżej i naprawa nowego rodzica
  node->parent = nodeGrandfather;
  if (node->parent != NIL) {
    // Podłączenie nowego ojca z *node
    if (nodeGrandfather->left == nodeParent) {
      nodeGrandfather->left = node;
    } else {
      nodeGrandfather->right = node;
    }
  }

  // Naprawa starego ojca *node
  nodeParent->parent = node;
  nodeParent->right = nodeLeftSubTree;
  nodeLeftSubTree->parent = nodeParent;
  // Naprawa wskaźnika na lewe dziecko *node
  node->left = nodeParent;
}

void rotateRight(Node *node) {
  Node *nodeRightSubTree = node->right;
  Node *nodeParent = node->parent;
  Node *nodeGrandfather = node->parent->parent;

  // Wydostanie *node poziom wyżej i naprawa nowego rodzica
  node->parent = nodeGrandfather;
  if (node->parent != NIL) {
    // Podłączenie nowego ojca z *node
    if (nodeGrandfather->left == nodeParent) {
      nodeGrandfather->left = node;
    } else {
      nodeGrandfather->right = node;
    }
  }

  // Naprawa starego ojca *node
  nodeParent->parent = node;
  nodeParent->left = nodeRightSubTree;
  nodeRightSubTree->parent = nodeParent;

  // Naprawa wskaźnika na lewe dziecko *node
  node->right = nodeParent;
}
#pragma endregion utils
#pragma region inserting
void fixCase1(Node *node) {
  printCaseHeader(node, 1);
  node->parent->color = black;
  getUncle(node)->color = black;
  Node *grandfather = node->parent->parent;
  grandfather->color = red;
  fixInsertion(grandfather);
}

void fixCase2(Node *node) {
  printCaseHeader(node, 2);
  if (node->parent->right == node) {
    rotateLeft(node);
    fixInsertion(node->left);
  } else {
    rotateRight(node);
    fixInsertion(node->right);
  }
}

void fixCase3(Node *node) {
  printCaseHeader(node, 3);
  if (node->parent->left == node)
    rotateRight(node->parent);
  else
    rotateLeft(node->parent);
  getBrother(node)->color = red;
  node->parent->color = black;
  fixInsertion(node);
}

void fixInsertion(Node *node) {
  // Korzeń:
  if (node->parent == NIL) return;
  if (node->parent->color == black) return;

  // Narysuj krok pośredni
  Node *tmp = node;
  while (tmp->parent != NIL) tmp = tmp->parent;
  print(tmp);

  // Naprawianie
  if (getUncle(node)->color == red) {
    fixCase1(node);
  } else {
    if (isTurning(node)) {
      fixCase2(node);
    } else {
      fixCase3(node);
    }
  }
}

void insertNode(Node **root, int value) {
  if (value < 0) {
    printf("Niepoprawna wartość\n");
    exit(1);
  }

  // Jeżeli korzeń jest pusty
  if ((*root)->value == -1) {
    (*root)->value = value;
    return;
  }

  // Informacja o rodzicu jest tracona przy wejściu w NIL
  Node *parent;
  Node *node = (*root);
  do {
    parent = node;
    // Przejście w lewe lub prawe dziecko
    node = (value <= node->value) ? node->left : node->right;
  } while (node != NIL);

  // node wskazuje na miejsce (NIL),
  // gdzie powinna zostać umieszczona nowa wartość
  node = makeNode(parent, value);
  fixInsertion(node);

  // Korzeń może zostać zmieniony
  while ((*root)->parent != NIL) (*root) = (*root)->parent;

  (*root)->color = black;
  return;
}
#pragma endregion inserting
#pragma region deletion
void deleteCase0(Node *node) {
  node->color = black;
  fixDeletion(node);
}

void deleteCase1(Node *node) {
  if (node->parent->left == node) {
    rotateLeft(getBrother(node));
  } else {
    rotateRight(getBrother(node));
  }
  node->parent->color = red;
  node->parent->parent->color = black;
  fixDeletion(node);
}

void deleteCase2(Node *node) {
  getBrother(node)->color = red;
  fixDeletion(node->parent);
}

void deleteCase3(Node *node) {
  if (node->parent->left == node) {
    Node *nephew = getBrother(node)->left;
    rotateRight(nephew);
    nephew->right->color = red;
    nephew->color = black;
  } else {
    Node *nephew = getBrother(node)->right;
    rotateRight(nephew);
    nephew->left->color = red;
    nephew->color = black;
  }
  deleteCase4(node);
}

void deleteCase4(Node *node) {
  Node *brother = getBrother(node);
  if (node->parent->left == node) {
    rotateLeft(brother);
    node->parent->parent->right->color = black;
  } else {
    rotateRight(brother);
    node->parent->parent->left->color = black;
  }
  node->parent->parent->color = node->parent->color;
  node->parent->color = black;
}

void fixDeletion(Node *node) {
  if (node->color == red) return deleteCase0(node);

  Node *brother = getBrother(node);
  if (brother->color == red) return deleteCase1(brother);

  if (brother->left->color == black && brother->right->color == black)
    return deleteCase2(node);

  if (node->parent->left == node) {
    if (brother->left->color == red && brother->right->color == black)
      return deleteCase3(node);
    else
      return deleteCase4(node);
  } else {
    if (brother->right->color == red && brother->left->color == black)
      return deleteCase3(node);
    else
      return deleteCase4(node);
  }
}

// Free memory
void destroyNode(Node *node) {
  if (node->parent->left == node)
    node->parent->left = NIL;
  else
    node->parent->right = NIL;
  free(node);
}

void deleteNode(Node **root, int value) {
  Node *node = get(*root, value);
  // Leaf case
  if (node->left == NIL && NIL == node->right) destroyNode(node);
  // Two child case
  if (node->left != NIL && NIL != node->right) {
    Node *successor = inorderSuccessor(node);
    node->value = successor->value;
    node->parent = successor->parent;
    node->left = successor->left;
    node->right = successor->right;
    node->color = successor->color;
    destroyNode(successor);
  }
  // One child case
  if (node->left != NIL) {
    node->value = node->left->value;
    destroyNode(node->left);
  } else {
    node->value = node->right->value;
    destroyNode(node->right);
  }
  // TODO: node ma być węzłem od którego zaczyna brakować czarnych
  fixDeletion(node);
  // Korzeń może zostać zmieniony
  while ((*root)->parent != NIL) (*root) = (*root)->parent;

  (*root)->color = black;
  return;
}
#pragma endregion deletion
#pragma region printing
void printCaseHeader(Node *node, int n) {
  if (!isPrintingAllowed) return;
  printf("[Przypadek #%d ", n);
  printNode(node);
  printf("]:\n");
}

void printNode(Node *node) {
  if (node == NIL) {
    printf("     ");
    return;
  }
  if (node->color == red)
    printf("(%03d)", node->value);
  else
    printf("[%03d]", node->value);
}

void printNodesAtLevel(Node *node, int treeDepth, int level, int currentLevel) {
  if (currentLevel > level) return;
  if (currentLevel == level) {
    // Padding przed liczbą
    int spaces = 3 * power(2, treeDepth - level) - 3;
    for (int i = 0; i < spaces; i++) printf(" ");

    printNode(node);

    // Padding za liczbą
    for (int i = 0; i < spaces + 1; i++) printf(" ");
    return;
  }
  printNodesAtLevel(node->left, treeDepth, level, currentLevel + 1);
  printNodesAtLevel(node->right, treeDepth, level, currentLevel + 1);
}

void printLinesAtLevel(Node *node, int treeDepth, int level, int currentLevel) {
  if (currentLevel > level) return;
  if (currentLevel == level) {
    // Padding przed linią
    int spaces = 3 * power(2, treeDepth - level) - 3;
    spaces = (spaces + 1) / 2;
    for (int i = 0; i < spaces; i++) printf(" ");

    // Rysowanie linii (pierwszy segment)
    int lineLength = spaces;
    if (node->left != NIL) {
      printf("┌");
      for (int i = 0; i < lineLength; i++) printf("─");
    } else {
      printf(" ");
      for (int i = 0; i < lineLength; i++) printf(" ");
    }

    // Rysowanie linii (łączenie)
    if (node->left == NIL)
      if (node->right == NIL)
        printf(" ");
      else
        printf("└");
    else if (node->right == NIL)
      printf("┘");
    else
      printf("┴");

    // Rysowanie linii (drugi segment)
    if (node->right != NIL) {
      for (int i = 0; i < lineLength; i++) printf("─");
      printf("┐");
    } else {
      for (int i = 0; i < lineLength; i++) printf(" ");
      printf(" ");
    }

    // Padding za linią
    for (int i = 0; i < spaces + 1; i++) printf(" ");
    return;
  }
  printLinesAtLevel(node->left, treeDepth, level, currentLevel + 1);
  printLinesAtLevel(node->right, treeDepth, level, currentLevel + 1);
}

void printLevel(Node *node, int treeDepth, int level) {
  printNodesAtLevel(node, treeDepth, level, 1);
  printf("\n");
  // Rysuj linie tylko do przedostatniej (zawsze pusta)
  if (level < treeDepth) {
    printLinesAtLevel(node, treeDepth, level, 1);
    printf("\n");
  }
}

void print(Node *node) {
  if (!isPrintingAllowed) return;
  if (node->value == -1) {
    printf("\n(Drzewo puste)\n\n");
    return;
  }

  int treeDepth = maxDepth(node);
  if (treeDepth > 5) {
    printf("Drzewo za duże! (Ilość poziomów > 5)\n");
    return;
  }
  printf("\n");
  for (int level = 1; level <= treeDepth; level++) {
    printLevel(node, treeDepth, level);
  }
  printf("\n");
}
#pragma endregion printing
#pragma region tests
void testInsertion(Node **tree) {
  printf("(xxx) - Węzeł czerwony, [xxx] - Węzeł czarny\n");
  print(*tree);

  // Wstawianie elementów
  int numbers[] = {38, 31, 22, 8, 20, 5, 10, 9, 21, 27, 29, 25, 28};
  int numbersLenght = sizeof(numbers) / sizeof(numbers[0]);
  for (int i = 0; i < numbersLenght; i++) {
    printf("Wstaw %03d [Enter]: ", numbers[i]);
    getchar();
    insertNode(tree, numbers[i]);
    print(*tree);
    printf("Wstawiono: ");
    printNode(get(*tree, numbers[i]));
    printf(" ###\n");
  }

  printf("Ilość czerwonych węzłów: %d\n", redNodesCount(*tree));
  printf("Najmniejsza głębokość: %d\n", minDepth(*tree) - 1);
  printf("Największa głębokość: %d\n", maxDepth(*tree) - 1);
  printf("Zakończono wstawianie: ");
  getchar();
}

void testInsertionHiddenOutput(Node **tree) {
  isPrintingAllowed = false;
  // Wstawianie elementów
  int numbers[] = {38, 31, 22, 8, 20, 5, 10, 9, 21, 27, 29, 25, 28};
  int numbersLenght = sizeof(numbers) / sizeof(numbers[0]);
  for (int i = 0; i < numbersLenght; i++) {
    insertNode(tree, numbers[i]);
  }
  isPrintingAllowed = true;
  printf("(xxx) - Węzeł czerwony, [xxx] - Węzeł czarny\n");
  print(*tree);
  printf("Ilość czerwonych węzłów: %d\n", redNodesCount(*tree));
  printf("Najmniejsza głębokość: %d\n", minDepth(*tree) - 1);
  printf("Największa głębokość: %d\n", maxDepth(*tree) - 1);
}

void testDeletion(Node **tree) {
  // Usuwanie elementów
  int numbers[] = {5, 38, 8, 10, 22, 20, 29, 31};
  int numbersLenght = sizeof(numbers) / sizeof(numbers[0]);
  for (int i = 0; i < numbersLenght; i++) {
    printf("Usuń ");
    printNode(get(*tree, numbers[i]));
    printf(" [Enter]: ");
    getchar();
    deleteNode(tree, numbers[i]);
    print(*tree);
    printf("Usunięto: %03d ###\n", numbers[i]);
  }
  printf("Zakończono usuwanie: ");
  printf("Ilość czerwonych węzłów: %d\n", redNodesCount(*tree));
  printf("Najmniejsza głębokość: %d\n", minDepth(*tree) - 1);
  printf("Największa głębokość: %d\n", maxDepth(*tree) - 1);
  getchar();
}
#pragma endregion tests

int main() {
  nilNode.parent = NIL;
  nilNode.left = NIL;
  nilNode.right = NIL;
  Node *tree = newRedBlackTree();

  // testInsertion(&tree); // Wstawianie krok po kroku
  testInsertionHiddenOutput(&tree);
  // testDeletion(&tree);
  return 0;
}

// Ostatnia zmiana: fixDeletion
//
// Zrobić algorytm który wyszuka pierwszy węzeł od którego zaczyna brakować
// czarnych.
