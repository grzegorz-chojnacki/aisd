import java.util.List;

public class BTrees {
  public static final String defaultFilePath = ".btree";

  public static void main(String[] args) {
    BTree tree = new BTree(3);
    // tree.add(List.of(62, 2, 88, 6, 83, 81, 49, 71, 2, 69, 74, 90, 23));
    // tree.add(List.of(5, 55, 555, 5555, 55555));
    tree.add(List.of(1, 2, 3, 4, 5));
    tree.print();
  }
}
