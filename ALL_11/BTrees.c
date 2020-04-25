#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define T 3

FILE *fp;

typedef struct Node {
  int index;
  int numberOfKeys;
  bool isLeaf;
  int keys[T * 2 - 1];
  int children[T * 2];
} Node;

int nodeSize = sizeof(Node);

typedef struct BTree {
  int root;
} BTree;

Node *newNode() {
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->isLeaf = true;
  node->index = -1;
  return node;
}

void read(int index, Node *node) {
  fseek(fp, index * nodeSize, SEEK_SET);
  fwrite(node, nodeSize, 1, fp);
}

void write(Node *node) {
  if (node->index == -1) {
    node->index = fseek(fp, 0, SEEK_END) / nodeSize;
  }
  fseek(fp, node->index * nodeSize, SEEK_SET);
  fread(node, nodeSize, 1, fp);
}

BTree *newBTree() {
  BTree *tree = (BTree *)calloc(1, sizeof(BTree));
  Node *root = newNode();
  write(root);
  tree->root = root->index;
  return tree;
}

int search(Node *node, int number) {
  int i = 0;
  while (i < node->numberOfKeys && number > node->keys[i]) {
    i++;
  }

  if (i < node->numberOfKeys && number == node->keys[i]) {
    return node->index;
  } else if (node->isLeaf) {
    return -1;
  } else {
    Node *nextNode = newNode();
    read(node->children[i], nextNode);
    return search(nextNode, number);
  }
}

void splitChild(Node *parent, int childIndex, Node *child) {
  Node *newChild = newNode();
  newChild->isLeaf = child->isLeaf;
  newChild->numberOfKeys = T - 1;
  for (int i = 0; i < T - 1; i++) {
    newChild->keys[i] = child->keys[T + i];
  }
  if (!child->isLeaf) {
    for (int i = 0; i < T; i++) {
      newChild->children[i] = child->children[T + i];
    }
  }
  child->numberOfKeys = T - 1;
  for (int i = parent->numberOfKeys; i >= childIndex; i--) {
    parent->children[i + 1] = parent->children[i];
  }
  parent->children[childIndex] = newChild->index;
  for (int i = parent->numberOfKeys - 1; i >= childIndex; i--) {
    parent->keys[i + 1] = parent->keys[i];
  }
  parent->keys[childIndex] = child->keys[T];
  parent->numberOfKeys++;
  write(parent);
  write(newChild);
  write(child);
}

void insertNonFull(Node *node, int number) {
  int i = node->numberOfKeys - 1;
  if (node->isLeaf) {
    while (i >= 0 && number < node->keys[i]) {
      node->keys[i + 1] = node->keys[i];
      i--;
    }
    node->keys[i + 1] = number;
    node->numberOfKeys++;
    write(node);
  } else {
    while (i >= 0 && number < node->keys[i]) {
      i--;
    }
    i++;
    Node *child = newNode();
    read(node->children[i], child);
    if (child->numberOfKeys == 2 * T - 1) {
      splitChild(node, i, child);
      if (number > node->keys[i]) {
        read(node->children[i++], child);
      }
    }
    insertNonFull(child, number);
  }
}

void insert(BTree *tree, int number) {
  Node *root = newNode();
  read(tree->root, root);
  if (root->numberOfKeys == 2 * T - 1) {
    Node *newRoot = newNode();
    tree->root = newRoot->index;
    newRoot->isLeaf = false;
    newRoot->numberOfKeys = 0;
    newRoot->children[0] = root->index;
    splitChild(newRoot, 0, root);
    insertNonFull(newRoot, number);
  } else {
    insertNonFull(root, number);
  }
}

void print(int index, int padding) {
  Node *node = newNode();
  read(index, node);
  if (node->isLeaf) {
    for (int i = 0; i < padding; i++) printf(" ");
    for (int i = 0; i < node->numberOfKeys; i++) printf("%d ", node->keys[i]);
    printf("\n");
  } else {
    print(node->children[node->numberOfKeys], padding + 4);
    for (int i = node->numberOfKeys - 1; i >= 0; i--) {
      for (int j = 0; j < padding; j++) printf(" ");
      printf("%d\n", node->keys[i]);
      print(node->children[i], padding + 4);
    }
  }
}

void printNode(Node *node) {
  printf("Index: %d\n", node->index);
  printf("Number of keys: %d\n", node->numberOfKeys);
  printf("Is leaf: %s\n", node->isLeaf ? "true" : "false");
  printf("Keys: ");
  for (int i = 0; i < T * 2 - 1; i++) {
    printf("%d, ", node->keys[i]);
  }
  printf("\nChildren: ");
  for (int i = 0; i < T * 2; i++) {
    printf("%d, ", node->children[i]);
  }
  printf("\n\n");
}

int main() {
  fp = fopen("btree", "w+");
  BTree *tree = newBTree();

  Node *node = newNode();

  insert(tree, 1);
  insert(tree, 2);
  insert(tree, 3);
  insert(tree, 4);
  insert(tree, 5);
  print(tree->root, 0);

  fclose(fp);
  return 0;
}