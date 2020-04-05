import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.List;

public class Node {
  private boolean isLeaf;
  private List<Integer> keys;
  private List<Integer> children;

  public Node() {
    this.keys = new ArrayList<Integer>();
    this.children = new ArrayList<Integer>();
  }

  public Node(boolean isLeaf, List<Integer> keys, List<Integer> children) {
    this.isLeaf = isLeaf;
    this.keys = new ArrayList<Integer>(keys);
    this.children = new ArrayList<Integer>(children);
    validate();
  }

  public void add(int number) {
    if (keys.size() < BTrees.getOrder()) {
      keys.add(number);
    }
  }

  public void load(int index) {
    try (RandomAccessFile file = new RandomAccessFile(BTrees.getDefaultFilePath(), "rw")) {
      file.seek(index);
      this.isLeaf = (file.read() == 1);
      int numberOfKeys = file.read();
      for (int i = 0; i < numberOfKeys; i++) {
        this.keys.add(file.read());
      }
      for (int i = 0; i <= numberOfKeys; i++) {
        this.children.add(file.read());
      }
    } catch (IOException e) {
      System.out.println(e.getStackTrace());
    }
    validate();
  }

  public void save(int index) {
    try (RandomAccessFile file = new RandomAccessFile(BTrees.getDefaultFilePath(), "rw")) {
      file.seek(index);
      file.write(isLeaf ? 1 : 0);
      file.write(keys.size());
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

  public boolean isLeaf() {
    return isLeaf;
  }

  public List<Integer> getKeys() {
    return keys;
  }

  public List<Integer> getChildren() {
    return children;
  }

  public String toString() {
    return ((isLeaf) ? "Leaf Node[" : "Node[") + keys.size() + "] -> Keys: " + keys + ", Children: " + children;
  }

  private void validate() {
    if (isLeaf && !children.isEmpty())
      throw new IllegalArgumentException("Is a leaf but has children");
    if (!isLeaf && children.isEmpty())
      throw new IllegalArgumentException("Is not a leaf but does not have children");
    if (keys.size() != (children.size() - 1))
      throw new IllegalArgumentException("Does not have the right amount of children");
  }

}