import java.util.*;

class Node {
  private String key;
  private int rank;
  private Node parent;

  // MakeSet(k) - klasa Node pełni rolę zbioru, więc jej konstruktor stanowi
  // implementacji funkcji MakeSet
  public Node(String key) {
    this.key = key;
    this.rank = 0;
    this.parent = this;
  }

  // FindSet(x)
  public Node findSet() {
    if (this != parent) {
      parent = parent.findSet();
    }
    return this.parent;
  }

  // Union(x, y) - obiektowość pozwala nam wywołać tą metodę z kontekstu x lub y,
  // za pomocą składni: x.union(y), lub y.union(x)
  public void union(Node that) {
    if (this.getRank() < that.getRank()) {
      setParent(that);
    } else {
      that.setParent(this);
      if (this.getRank() == that.getRank()) {
        that.incrementRank();
      }
    }
  }

  public String getPath() {
    if (this.parent == this) {
      return "/" + key;
    } else {
      return parent.getPath() + "/" + key;
    }
  }

  public String getkey() {
    return key;
  }

  public int getRank() {
    return rank;
  }

  private void setParent(Node newParent) {
    parent = newParent;
  }

  private void incrementRank() {
    rank++;
  }
}

class DisjointSets {
  // Funkcja upodabniająca interfejs na taki jak w zadaniu
  public static void union(Node a, Node b) {
    a.union(b);
  }

  public static void main(String[] args) {
    // Lista wszystkich węzłów w grafie
    List<Node> nodes = new ArrayList<>();

    for (int i = 0; i < 10; i++) {
      nodes.add(new Node(Integer.toString(i)));
    }

    union(nodes.get(0).findSet(), nodes.get(1).findSet());
    union(nodes.get(2).findSet(), nodes.get(3).findSet());
    union(nodes.get(1).findSet(), nodes.get(2).findSet());
    union(nodes.get(5).findSet(), nodes.get(6).findSet());
    union(nodes.get(7).findSet(), nodes.get(8).findSet());
    union(nodes.get(3).findSet(), nodes.get(5).findSet());
    union(nodes.get(0).findSet(), nodes.get(7).findSet());

    System.out.println("Ścieżki do korzenia dla każdego węzła \"/korzeń/węzeł[1]/.../węzeł[n-1]/węzeł[n]\":");
    for (Node node : nodes) {
      System.out.println("  " + node.getPath());
    }

  }
}
