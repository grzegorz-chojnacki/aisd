#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

bool isValid(Node *node) {
  if (node == NIL || node->parent == NIL) return true;
  // *node jest poprawnie przyłączonym dzieckiem
  // (Tylko po jednej stronie rodzica i przyłączonym dobrze w obie strony)
  if ((node->parent->left == node) != (node->parent->right == node))
    return true && isValid(node->left) && isValid(node->right);
  else
    return false;
}

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

void print(Node *node);
void printNode(Node *node);
void fixTree(Node *node);

void printCaseHeader(Node *node, int n) {
  printf("[Przypadek #%d ", n);
  printNode(node);
  printf("]:\n");
}

void fixCase1(Node *node) {
  printCaseHeader(node, 1);
  node->parent->color = black;
  getUncle(node)->color = black;
  Node *grandfather = node->parent->parent;
  grandfather->color = red;
  fixTree(grandfather);
}

void fixCase2(Node *node) {
  printCaseHeader(node, 2);
  if (node->parent->right == node) {
    rotateLeft(node);
    fixTree(node->left);
  } else {
    rotateRight(node);
    fixTree(node->right);
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
  fixTree(node);
}

void fixTree(Node *node) {
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

Node *insert(Node **root, int value) {
  if (value < 0) {
    printf("Niepoprawna wartość\n");
    exit(1);
  }

  // Jeżeli korzeń jest pusty
  if ((*root)->value == -1) {
    (*root)->value = value;
    return *root;
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
  fixTree(node);

  // Korzeń może zostać zmieniony
  while ((*root)->parent != NIL)
    (*root) = (*root)->parent;

  (*root)->color = black;
  return *root;
}

void delete(Node *node, int value) {}

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

int redNodesCount(Node *node) {
  if (node == NIL) return 0;
  return ((node->color == red) ? 1 : 0)
    + redNodesCount(node->left) + redNodesCount(node->right);
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

int power(int a, int b) {
  int result = 1;
  while (b-- > 0) result *= a;
  return result;
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

int main() {
  int numbers[] = {38, 31, 22, 8, 20, 5, 10, 9, 21, 27, 29, 25, 28};
  nilNode.parent = NIL;
  nilNode.left = NIL;
  nilNode.right = NIL;
  int numbersLenght = sizeof(numbers) / sizeof(numbers[0]);
  Node *tree = newRedBlackTree();
  printf("(xxx) - Węzeł czerwony, [xxx] - Węzeł czarny\n");
  print(tree);

  for (int i = 0; i < numbersLenght; i++) {
    printf("Wstaw (%03d) [Enter]: ", numbers[i]);
    getchar();
    tree = insert(&tree, numbers[i]);
    print(tree);
    printf("Wstawiono: ");
    printNode(get(tree, numbers[i]));
    printf(" ###\n");
  }
  printf("isValid: %d\n", isValid(tree));
  printf("Ilość czerwonych węzłów: %d\n", redNodesCount(tree));
  return 0;
}
