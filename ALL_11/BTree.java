import java.util.ArrayList;
import java.util.List;

class BTree {
  private Node root;

  public BTree(int order) {
    this.root = new Node();
  }

  public void add(int number) {
    Node node = root;
    while (!node.isLeaf()) {
      List<Integer> keys = node.getKeys();
      for (int i = 0; i < keys.size(); i++) {
        if (keys.get(i) > number) {
          node.load(node.getChildren().get(i));
          break;
        }
      }
    }
  }

  public void add(List<Integer> numbers) {
    for (int number : numbers) {
      add(number);
    }
  }
}