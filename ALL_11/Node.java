import java.io.IOException;
import java.io.RandomAccessFile;

public class Node {
  private int index = 0; // Miejsce zapisu w pliku
  private int order; // Minimalny stopień drzewa
  private boolean isLeaf = true;
  private int numberOfKeys = 0; // Liczba kluczy zapisanych w tablicy keys
  private int[] keys;
  private int[] childrenIndices;
  private int parentIndex = -1; // Miejsce zapisu rodzica w pliku,
  // jeżeli parentIndex == -1, to węzeł jest korzeniem

  public Node(int order) {
    this.order = order;
    this.keys = new int[order * 2 - 1];
    this.childrenIndices = new int[order * 2];
  }

  private void setParentIndex(int index) {
    this.parentIndex = index;
  }

  // Wstawia liczbę do tablicy kluczy i zwraca indeks na jaki została ona wstawiona
  private int addToKeys(int number) {
    for (int i = numberOfKeys; i > 0; i--) {
      // Wstaw liczbę do tablicy kluczy
      if (keys[i - 1] < number) {
        keys[i] = number;
        numberOfKeys++;
        return i;
      // Przesuń klucz ku końcowi tablicy
      } else {
        keys[i] = keys[i - 1];
      }
    }
    // Wstawiana liczba jest najmniejszym z kluczy
    keys[0] = number;
    numberOfKeys++;
    return 0;
  }

  // Dodaje podany indeks do tablicy indeksów dzieci na wskazane miejsce
  private void insertChildIndexAt(int childIndex, int childrenIndicesPosition) {
    for (int i = numberOfKeys; i > childrenIndicesPosition; i--) {
      childrenIndices[i] = childrenIndices[i - 1];
    }
    childrenIndices[childrenIndicesPosition] = childIndex;
  }

  // Dodaje podany indeks do tablicy indeksów dzieci na puste miejsce
  private void addChildIndex(int childIndex) {
    for (int i = 0; i < childrenIndices.length; i++) {
      if (childrenIndices[i] == 0) {
        childrenIndices[i] = childIndex;
      }
    }
  }

  // Dzieli węzeł na dwa, poprzez podział kluczy i dzieci, zapisuje nowy
  // węzeł w pliku i zwraca jego indeks
  private int splitNode() {
    Node newNode = new Node(order);
    for (int i = keys.length / 2; i < keys.length; i++) {
      newNode.addToKeys(keys[i]);
      keys[i] = 0;
      numberOfKeys--;
    }
    for (int i = childrenIndices.length / 2; i < childrenIndices.length; i++) {
      newNode.addChildIndex(childrenIndices[i]);
      childrenIndices[i] = 0;
    }
    newNode.save(findEmptyPlaceInFile());
    return newNode.getIndex();
  }

  // Obsługuje sytuacje gdy węzeł ma za dużo kluczy. Wczytuje rodzica (lub tworzy
  // nowy korzeń, jeżeli obecny jest już przepełniony) i oddaje mu klucz środkowy
  private void propagadeUp() {
    Node parent;
    // Węzeł jest korzeniem - tworzenie nowego korzenia
    if (parentIndex == -1) {
      parent = new Node(order);
      parent.save(findEmptyPlaceInFile());
      BTree.setRootIndex(parent.getIndex());
      this.parentIndex = parent.getIndex();
      parent.setAsNotLeaf();
    // Zwykły węzeł
    } else {
      parent = loadNode(parentIndex);
    }

    int middleKey = popMiddleKey();
    int addedKeyIndex = parent.addToKeys(middleKey);

    Node newSibling = loadNode(splitNode());
    newSibling.setParentIndex(parent.getIndex());

    parent.insertChildIndexAt(newSibling.getIndex(), addedKeyIndex + 1);
    save();
    // System.out.println(parent);
    // System.out.println(this);
    // System.out.println(newSibling);
    // Naruszenie warunku b-drzewa u rodzica
    if (parent.getNumberOfKeys() == (order * 2 - 1)) {
      parent.propagadeUp();
    }
  }

  public void add(int number) {
    if (isLeaf()) {
      addToKeys(number);
      // Naruszenie warunku b-drzewa
      if (numberOfKeys == (order * 2 - 1)) {
        propagadeUp();
      }
      save();
    } else {
      for (int i = 0; i < numberOfKeys; i++) {
        if (number < keys[i]) {
          Node child = loadNode(childrenIndices[i]);
          child.add(number);
          return;
        }
      }
      // Wstawiana liczba jest większa od wszystkich kluczy
      Node child = loadNode(childrenIndices[numberOfKeys]);
      child.add(number);
    }
  }


  // Zwraca zawartość środkowego klucza z tablicy kluczy,
  // jednocześnie pozbywając się go i przywracając porządek w tablicy
  private int popMiddleKey() {
    int middleIndex = numberOfKeys / 2;
    int middleKey = keys[middleIndex];
    for (int i = middleIndex; i < keys.length - 1; i++) {
      keys[i] = keys[i + 1];
    }
    keys[keys.length - 1] = 0;
    numberOfKeys--;
    return middleKey;
  }

  private void printPadding(int padding) {
    while (padding > 0) {
      System.out.print(" ");
      padding--;
    }
  }

  private Node loadNode(int index) {
    Node tempNode = new Node(order);
    tempNode.load(index);
    return tempNode;
  }

  // Wczytuje dane dziecko z pliku i rekurencyjnie wyświetla jego zawartość
  private void printChild(int index, int padding) {
    Node tempNode = loadNode(childrenIndices[index]);
    tempNode.print(padding + 2);
  }

  // Przechodzi drzewo w porządku odwróconym inorder, wyświetla wszystkie
  // dzieci i klucze
  private void print(int padding) {
    if (isLeaf()) {
      printPadding(padding);
      for (int i = 0; i < numberOfKeys; i++) {
        System.out.print(keys[i] + " ");
      }
      System.out.println();
    } else {
      printChild(numberOfKeys, padding); // Dziecko z prawego brzegu
      for (int i = numberOfKeys - 1; i >= 0; i--) {
        printPadding(padding);
        System.out.println(keys[i]);
        printChild(i, padding);
      }
    }
  }

  public void print() {
    print(0);
  }

  public void load(int index) {
    this.index = index;
    try (RandomAccessFile file = new RandomAccessFile(BTrees.defaultFilePath, "rw")) {
      file.seek(index);
      this.isLeaf = (file.read() == 1);
      this.numberOfKeys = file.read();
      for (int i = 0; i < order * 2 - 1; i++) {
        this.keys[i] = file.read();
      }
      for (int i = 0; i < order * 2; i++) {
        this.childrenIndices[i] = file.read();
      }
      this.parentIndex = file.read();
      System.out.print("Load: ");
      System.out.println(this);
    } catch (IOException e) {
      System.out.println(index);
      e.printStackTrace();
    }
  }

  public void save() {
    save(this.index);
  }

  private int findEmptyPlaceInFile() {
    try (RandomAccessFile file = new RandomAccessFile(BTrees.defaultFilePath, "r")) {
      return (int)file.length();
    } catch (IOException e) {
      e.printStackTrace();
      return -404;
    }
  }

  public void save(int index) {
    System.out.print("Save: ");
    System.out.println(this);

    try (RandomAccessFile file = new RandomAccessFile(BTrees.defaultFilePath, "rw")) {
      file.seek(index);
      file.write((isLeaf()) ? 1 : 0);
      file.write(numberOfKeys);
      for (int key : keys) {
        file.write(key);
      }
      for (int childIndex : childrenIndices) {
        file.write(childIndex);
      }
      file.write(parentIndex);
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  public boolean isLeaf() {
    return isLeaf;
  }

  private void setAsNotLeaf() {
    isLeaf = false;
  }

  public int getNumberOfKeys() {
    return numberOfKeys;
  }

  public int getIndex() {
    return index;
  }

  public String toString() {
    String label = ((isLeaf()) ? "Leaf Node(" : "Node(") + order + ") -> Keys: [";
    for (int i = 0; i < keys.length; i++) {
      label = label + keys[i] + ", ";
    }
    label = label + "] Children: [";
    for (int i = 0; i < childrenIndices.length; i++) {
      label = label + childrenIndices[i] + ", ";
    }
    return label + "]";
  }
}