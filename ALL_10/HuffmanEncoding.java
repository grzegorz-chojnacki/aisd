import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;


// Tablica kodowa
class Table {
  private List<Node> nodes = new ArrayList<Node>();
  // Używane do ładnego wyświetlania
  private int highestFrequency = 0;

  Table(int[] ASCIITable) {
    for (int i = 0; i < ASCIITable.length; i++) {
      if (ASCIITable[i] > 0) {
        Node node = new Node((char) i, ASCIITable[i]);
        addInOrder(nodes, node);
        highestFrequency = Math.max(highestFrequency, ASCIITable[i]);
      }
    }

    buildTree();
  }

  // Wstaw węzeł tak, aby lista pozostała posortowana
  private void addInOrder(List<Node> nodes, Node newNode) {
    ListIterator<Node> iterator = nodes.listIterator();
    while (iterator.hasNext()) {
      Node node = iterator.next();
      if (node.getFrequency() > newNode.getFrequency()) {
        nodes.add(iterator.nextIndex(), newNode);
        return;
      }
    }
    // Nowy węzeł rozgałęziający ma największą wartość i trafia na koniec listy
    nodes.add(newNode);
  }

  private void buildTree() {
    if (nodes.size() <= 1)
      return;
    Node A = nodes.get(0);
    Node B = nodes.get(1);
    Node C = new Node();
    C.setAsParentOf(A, B);
    nodes.remove(A);
    nodes.remove(B);

    addInOrder(nodes, C);
    buildTree();
  }

  public int getCompressedSize() {
    return getCompressedSizeHelper(nodes.get(0), 0);
  }

  private int getCompressedSizeHelper(Node node, int codeSize) {
    if (node.isLeaf()) {
      return codeSize * node.getFrequency();
    } else {
      return getCompressedSizeHelper(node.getRight(), codeSize + 1)
          + getCompressedSizeHelper(node.getLeft(), codeSize + 1);
    }
  }

  public void print() {
    System.out.println("Tabela kodu:");
    printHelper(nodes.get(0), "");
  }

  private int numberOfDigits(int number) {
    return (int)Math.log10(number) + 1;
  }

  private void printHelper(Node node, String code) {
    if (node.isLeaf()) {
      System.out.printf(" - '%c' [%" + numberOfDigits(highestFrequency) + "d]: %s\n",
        node.getValue(), node.getFrequency(), code);
    } else {
      printHelper(node.getLeft(), code + "0");
      printHelper(node.getRight(), code + "1");
    }
  }
}

enum State {
  Leaf, Parent
}

// Węzły używane do budowy drzewa kodowań
class Node {
  private int frequency;
  private char value;
  private State state;
  private Node left;
  private Node right;

  Node() {
    super();
  }

  Node(char value, int frequency) {
    this.value = value;
    this.frequency = frequency;
    this.state = State.Leaf;
  }

  public int getFrequency() {
    return frequency;
  }

  public int getValue() {
    return value;
  }

  public boolean isLeaf() {
    return (state == State.Leaf);
  }

  public Node getLeft() {
    return left;
  }

  public Node getRight() {
    return right;
  }

  // Ustawianie węzła jako węzła rozgałęzienia (zawsze pusty nowy węzeł)
  public void setAsParentOf(Node A, Node B) {
    if (A.getFrequency() < B.getFrequency()) {
      this.left = A;
      this.right = B;
    } else {
      this.left = B;
      this.right = A;
    }
    this.frequency = A.getFrequency() + B.getFrequency();
    this.state = State.Parent;
  }
}

public class HuffmanEncoding {
  public static void main(String[] args) {
    if (args.length != 1) {
      System.out.println("Podaj ścieżkę do pliku jako argument!");
      return;
    }

    int[] ASCIITable = new int[256];
    int originalFileLength = 0;

    try (FileReader fileReader = new FileReader(new File(args[0]))) {
      byte character;
      while ((character = (byte)fileReader.read()) != -1) {
        originalFileLength++;
        ASCIITable[character]++;
      }
    } catch (IOException e) {
      System.out.println("Błąd pliku");
      return;
    }

    Table table = new Table(ASCIITable);
    table.print();
    System.out.println("Długość oryginalnego pliku (bity): " + originalFileLength * 8);
    System.out.println("Długość skompresowanego pliku (bity): " + table.getCompressedSize());
  }
}
