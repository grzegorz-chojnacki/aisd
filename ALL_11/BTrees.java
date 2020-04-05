import java.util.List;

public class BTrees {

  private static String defaultFilePath = ".btree";

  public static String getDefaultFilePath() {
    return defaultFilePath;
  }

  private static int order = 3;

  public static int getOrder() {
    return order;
  }

  public static void main(String[] args) {
    Node node = new Node(false, List.of(1, 2, 3, 4), List.of(4, 5, 6, 7, 8));
    System.out.println(node);
    node.save(0);

    Node anotherNode = new Node();
    anotherNode.load(0);
    System.out.println(anotherNode);

  }
}
