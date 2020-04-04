import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.List;

class Node {
  private boolean isLeaf;
  private int numberOfKeys;
  private List<Integer> keys;
  private List<Integer> children;

  public Node() {
    this.keys = new ArrayList<Integer>();
    this.children = new ArrayList<Integer>();
  }

  public Node(boolean isLeaf, int numberOfKeys, List<Integer> keys, List<Integer> children) {
    this.isLeaf = isLeaf;
    this.numberOfKeys = numberOfKeys;
    this.keys = new ArrayList<Integer>(keys);
    this.children = new ArrayList<Integer>(children);
  }

  public void load(int index) {
    try (RandomAccessFile file = new RandomAccessFile(BTrees.defaultFilePath, "rw")) {
      file.seek(index);
      this.isLeaf = (file.read() == 1);
      this.numberOfKeys = file.read();
      for (int i = 0; i < numberOfKeys; i++) {
        this.keys.add(file.read());
      }
      for (int i = 0; i <= numberOfKeys; i++) {
        this.children.add(file.read());
      }
    } catch (IOException e) {
      System.out.println(e.getStackTrace());
    }
  }

  public void save(int index) {
    try (RandomAccessFile file = new RandomAccessFile(BTrees.defaultFilePath, "rw")) {
      file.seek(index);
      file.write(isLeaf ? 1 : 0);
      file.write(numberOfKeys);
      for (int key : keys) {
        file.write(key);
      }
      for (int child : children) {
        file.write(child);
      }
    } catch (IOException e) {
      System.out.println(e.getStackTrace());
    }
  }

  public String toString() {
    return ((isLeaf) ? "Leaf Node[" : "Node[") + numberOfKeys + "] -> Keys: " + keys + ", Children: " + children;
  }

}

class BTree {
  private Node root;

  public BTree(List<Integer> numbers) {
    this.root = new Node();
    add(numbers);
  }

  public void add(int number) {
    // TODO: napisać funkcję wstawiającą liczbę do drzewa
  }

  public void add(List<Integer> numbers) {
    // TODO: napisać funkcję wstawiającą liczby do drzewa
  }
}

public class BTrees {

  public static String defaultFilePath = ".btree";

  public static void main(String[] args) {
    Node node = new Node(true, 4, List.of(1, 2, 3, 4), List.of(4, 5, 6, 7, 8));
    System.out.println(node);
    node.save(0);

    Node anotherNode = new Node();
    anotherNode.load(0);
    System.out.println(anotherNode);

  }
}
