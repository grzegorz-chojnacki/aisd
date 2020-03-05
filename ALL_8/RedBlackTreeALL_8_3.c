#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool isPrintingAllowed = true;

// Kolor 'empty' jest specjalnym kolorem tylko dla pustego korzenia
typedef enum Color { red, black, empty } Color;

typedef struct Node {
  int value;
  struct Node *parent;
  struct Node *left;
  struct Node *right;
  Color color;
} Node;

Node nilNode = {0, NULL, NULL, NULL, black};
#define NIL (&nilNode)

void printCaseHeader(Node *node, int n);
void fixInsertion(Node *node);
void fixDeletion(Node *node);
void print(Node *node);
void printNode(Node *node);

Node *newRedBlackTree() {
  Node *rootNode = (Node *)calloc(1, sizeof(Node));
  rootNode->value = 0;
  rootNode->parent = NIL;
  rootNode->left = NIL;
  rootNode->right = NIL;
  rootNode->color = empty;

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

// Pobiera węzeł najdalej od korzenia (w przypadku)
Node *get(Node *node, int value) {
  if (node == NIL) return NIL;

  Node *leftChild = get(node->left, value);
  if (leftChild->value == value) return leftChild;

  Node *rightChild = get(node->right, value);
  if (rightChild->value == value) return rightChild;

  if (node->value == value)
    return node;
  else
    return NIL;
}

// NIL returns NIL
Node *getBrother(Node *node) {
  if (node->parent->left == node)
    return node->parent->right;
  else
    return node->parent->left;
}

Node *getUncle(Node *node) { return getBrother(node->parent); }

bool areParallel(Node *nodeA, Node *nodeB) {
  if (nodeA->parent->left == nodeA) {
    if (nodeB->parent->left == nodeB)
      return true;
    else
      return false;
  } else if (nodeB->parent->right == nodeB)
    return true;
  else
    return false;
}

Node *getBrotherRedParallelSon(Node *node) {
  Node *brother = getBrother(node);
  if (brother->left->color == red && areParallel(node, brother->left))
    return brother->left;
  if (brother->right->color == red && areParallel(node, brother->right))
    return brother->right;
  else
    return NIL;
}

Node *getBrotherRedTurningSon(Node *node) {
  Node *brother = getBrother(node);
  if (brother->left->color == red && !areParallel(node, brother->left))
    return brother->left;
  if (brother->right->color == red && !areParallel(node, brother->right))
    return brother->right;
  else
    return NIL;
}

bool hasSonOfColor(Node *node, Color color) {
  if (node->left->color == color) return true;
  if (node->right->color == color)
    return true;
  else
    return false;
}

// Użyte tylko w przypadku drzewa z którego usuwamy węzeł z dwoma synami.
// Węzeł taki zawsze będzie miał prawe poddrzewo, w którym znajdziemy jego
// następnika (węzeł maksymalnie na lewo)
Node *inorderSuccessor(Node *node) {
  node = node->right;
  while (node->left != NIL) node = node->left;
  return node;
}

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

int minDepth(Node *node) {
  if (node == NIL) return 0;
  if (node->color == empty) return -1;
  int leftDepth = minDepth(node->left) + 1;
  int rightDepth = minDepth(node->right) + 1;
  return (leftDepth < rightDepth ? leftDepth : rightDepth);
}

int maxDepth(Node *node) {
  if (node == NIL) return 0;
  if (node->color == empty) return -1;
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

void rotateLeft(Node *node) {
  Node *nodeLeftSubTree = node->left;
  Node *nodeParent = node->parent;
  Node *nodeGrandfather = node->parent->parent;

  // Wydostanie *node poziom wyżej i naprawa nowego rodzica
  node->parent = nodeGrandfather;
  if (node->parent != NIL) {
    // Podłączenie nowego ojca z *node
    if (nodeGrandfather->left == nodeParent)
      nodeGrandfather->left = node;
    else
      nodeGrandfather->right = node;
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
    if (nodeGrandfather->left == nodeParent)
      nodeGrandfather->left = node;
    else
      nodeGrandfather->right = node;
  }

  // Naprawa starego ojca *node
  nodeParent->parent = node;
  nodeParent->left = nodeRightSubTree;
  nodeRightSubTree->parent = nodeParent;

  // Naprawa wskaźnika na lewe dziecko *node
  node->right = nodeParent;
}

// Automatyczny obrót węzła tak, by stał się rodzicem swojego rodzica
void rotate(Node *node) {
  if (node->parent->left == node)
    rotateRight(node);
  else
    rotateLeft(node);
}

// *nextNode - wskaźnik na węzeł, który jest kolejnym kandydatem na naprawę:
void insertCase1(Node *node) {
  printCaseHeader(node, 1);
  Node *nextNode = node->parent->parent;
  nextNode->color = red;
  node->parent->color = black;
  getUncle(node)->color = black;
  fixInsertion(nextNode);
}

void insertCase3(Node *node) {
  printCaseHeader(node, 3);
  node->parent->color = black;
  node->parent->parent->color = red;
  rotate(node->parent);
}

void insertCase2(Node *node) {
  printCaseHeader(node, 2);
  Node *nextNode = node->parent;
  rotate(node);
  insertCase3(nextNode);
}

void fixInsertion(Node *node) {
  // Napotkano korzeń:
  if (node->parent == NIL) return;
  if (node->parent->color == black) return;

  // Narysuj krok pośredni
  Node *tmp = node;
  while (tmp->parent != NIL) tmp = tmp->parent;
  print(tmp);

  // Naprawianie
  if (getUncle(node)->color == red)
    insertCase1(node);
  else if (isTurning(node))
    insertCase2(node);
  else
    insertCase3(node);
}

void insertNode(Node **root, int value) {
  // Jeżeli korzeń jest pusty
  if ((*root)->color == empty) {
    (*root)->value = value;
    (*root)->color = black;
    return;
  }
  // Informacja o rodzicu jest tracona przy wejściu w NIL
  Node *parent;
  Node *node = (*root);
  do {
    parent = node;
    // Przeszukiwanie binarne
    node = (value <= node->value) ? node->left : node->right;
  } while (node != NIL);

  // node wskazuje na miejsce (NIL),
  // gdzie powinna zostać umieszczona nowa wartość
  node = makeNode(parent, value);
  fixInsertion(node);

  // Korzeń może zostać zmieniony rotacją
  while ((*root)->parent != NIL) {
    (*root) = (*root)->parent;
  }
  (*root)->color = black;
  return;
}

void deleteCase0(Node *node) {
  printCaseHeader(node, 0);
  node->color = black;
}

void deleteCase1(Node *node) {
  printCaseHeader(node, 1);
  Node *nextNode = node;
  Node *brother = getBrother(node);
  node->parent->color = red;
  brother->color = black;
  rotate(brother);
  fixDeletion(nextNode);
}

void deleteCase2(Node *node) {
  printCaseHeader(node, 2);
  Node *nextNode = node->parent;
  getBrother(node)->color = red;
  fixDeletion(nextNode);
}

void deleteCase4(Node *node) {
  printCaseHeader(node, 4);
  Node *brother = getBrother(node);
  brother->color = node->parent->color;
  node->parent->color = black;
  getBrotherRedTurningSon(node)->color = black;
  rotate(brother);
}

void deleteCase3(Node *node) {
  printCaseHeader(node, 3);
  Node *brother = getBrother(node);
  Node *brotherRedSon = getBrotherRedParallelSon(node);
  brother->color = red;
  brotherRedSon->color = black;
  rotate(brotherRedSon);
  deleteCase4(node);
}

void fixDeletion(Node *node) {
  if (node->color == red) return deleteCase0(node);
  Node *brother = getBrother(node);
  if (brother->color == red)
    deleteCase1(node);
  else if (!hasSonOfColor(brother, red))
    deleteCase2(node);
  else {
    if (hasSonOfColor(brother, black) && getBrotherRedParallelSon(node) != NIL)
      deleteCase3(node);
    else
      deleteCase4(node);
  }
}

// Zwalnianie pamięci
void destroyNode(Node *node) {
  if (node->parent->left == node)
    node->parent->left = NIL;
  else
    node->parent->right = NIL;
  free(node);
}

void deleteNode(Node **root, int value) {
  // *node jest wskaźnikiem na węzeł przeznaczony do usunięcia
  Node *node = get(*root, value);
  if (node == NIL) {
    printf("Wartość %d nie znajduje się w drzewie!\n", value);
    return;
  }

  // Usuwany węzeł jest liściem
  if (node->left == NIL && node->right == NIL) {
    node = node;
  } else if (node->left != NIL && node->right != NIL) {
    // Usuwany węzeł ma dwoje synów
    Node *successor = inorderSuccessor(node);
    node->value = successor->value;
    node = successor;
  } else if (node->left != NIL) {
    // Usuwany węzeł ma jedno dziecko (lewe)
    node->value = node->left->value;
    node = node->left;
  } else {
    // Usuwany węzeł ma jedno dziecko (prawe)
    node->value = node->right->value;
    node = node->right;
  }
  // Usuwana wartość jest w korzeniu
  // Ten fragment musi być umieszczony tutaj, by uniknąć błędów przy usuwaniu
  // węzłów z wartościami które powtrzają się w drzewie
  if (node == *root) {
    (*root)->value = 0;
    (*root)->color = empty;
    return;
  }

  fixDeletion(node);
  destroyNode(node);

  // Korzeń może zostać zmieniony
  while ((*root)->parent != NIL) (*root) = (*root)->parent;

  (*root)->color = black;
  return;
}

void printCaseHeader(Node *node, int n) {
  if (!isPrintingAllowed) return;
  printf("[Przypadek #%d ", n);
  printNode(node);
  printf("]\n");
}

void printNode(Node *node) {
  if (node == NIL)
    printf("     ");
  else if (node->color == red)
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
    if (node->left == NIL) {
      if (node->right == NIL)
        printf(" ");
      else
        printf("└");
    } else {
      if (node->right == NIL)
        printf("┘");
      else
        printf("┴");
    }

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
  if (node->color == empty) {
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
  printf("Najmniejsza głębokość: %d\n", minDepth(*tree));
  printf("Największa głębokość: %d\n", maxDepth(*tree));
  printf("Zakończono wstawianie: ");
  getchar();
}

void testDeletion(Node **tree) {
  // Usuwanie elementów
  int numbers[] = {5, 38, 8, 10, 22, 20, 29, 31, 27, 21, 9, 25, 28};
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
  printf("Ilość czerwonych węzłów: %d\n", redNodesCount(*tree));
  printf("Najmniejsza głębokość: %d\n", minDepth(*tree));
  printf("Największa głębokość: %d\n", maxDepth(*tree));
  printf("Zakończono usuwanie: ");
  getchar();
}

void testInsertionRandom(Node **tree, int n) {
  isPrintingAllowed = false;
  for (int i = 1; i <= n; i++) {
    insertNode(tree, n);
  }
  isPrintingAllowed = true;
  printf("\nIlość czerwonych węzłów: %d\n", redNodesCount(*tree));
  printf("Najmniejsza głębokość: %d\n", minDepth(*tree));
  printf("Największa głębokość: %d\n", maxDepth(*tree));
  printf("Zakończono wstawianie: ");
  getchar();
}

void testDeletionRandom(Node **tree, int n) {
  isPrintingAllowed = false;
  for (int i = 1; i <= n; i++) {
    deleteNode(tree, n);
  }
  isPrintingAllowed = true;
  printf("\nIlość czerwonych węzłów: %d\n", redNodesCount(*tree));
  printf("Najmniejsza głębokość: %d\n", minDepth(*tree));
  printf("Największa głębokość: %d\n", maxDepth(*tree));
  printf("Zakończono usuwanie: ");
  getchar();
}

int main() {
  nilNode.parent = NIL;
  nilNode.left = NIL;
  nilNode.right = NIL;
  Node *tree = newRedBlackTree();
  int testSize = 1000;

  testInsertion(&tree);
  testDeletion(&tree);
  printf("\n\n### Test na dużych danych (%d węzłów) ###\n\n", testSize);
  testInsertionRandom(&tree, testSize);
  testDeletionRandom(&tree, testSize);

  return 0;
}