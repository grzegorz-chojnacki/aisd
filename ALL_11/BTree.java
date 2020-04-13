import java.util.List;
import java.io.File;

public class BTree {
  private static int rootIndex = 0;
  private int order;

  public BTree(int order) {
    File file = new File(BTrees.defaultFilePath);
    if (file.exists()) {
      file.delete();
    }

    if (order < 2) throw new IllegalArgumentException("Tree order is too small");
    this.order = order;
    Node root = new Node(order);
    root.save(rootIndex);
  }

  public static void setRootIndex(int index) {
    rootIndex = index;
  }

  public void add(int number) {
    Node root = new Node(order);
    root.load(rootIndex);
    root.add(number);
  }

  public void print() {
    Node root = new Node(order);
    root.load(rootIndex);
    root.print();
  }

  public void add(List<Integer> numbers) {
    for (int number : numbers) {
      add(number);
    }
  }
}