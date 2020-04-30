import java.util.*;

class Node {
  private String key;
  private int rank;
  private Node parent;

  // MakeSet(k) - obiekty Node pełnią rolę elementów zbioru, więc konstruktor stanowi
  // implementacje funkcji MakeSet
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
    if (this == that)
      return;
    if (this.getRank() < that.getRank()) {
      this.setParent(that);
    } else {
      that.setParent(this);
      if (this.getRank() == that.getRank()) {
        this.incrementRank();
      }
    }
  }

  // Funkcja tworzy listę węzłów bez powtórzeń
  public static List<Node> nodesFrom(String[] args) {
    List<Node> nodes = new ArrayList<>();
    for (int i = 0; i < args.length; i++) {
      if (i % 3 == 2) continue; // Pomijanie wag z tablicy argumentów
      Node node = Node.find(args[i], nodes);
      // Węzła o podanym kluczy nie ma jeszcze na liście węzłów
      if (node == null) {
        node = new Node(args[i]);
        nodes.add(node);
      }
    }
    return nodes;
  }

  // Funkcja szuka zadanego klucza wśród listy węzłów, lub zwraca null
  public static Node find(String key, List<Node> nodes) {
    for (Node node : nodes) {
      if (node.getKey().equals(key))
        return node;
    }
    return null;
  }

  public String getKey() {
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

class Edge {
  private Node a;
  private Node b;
  private int weight;

  public Edge(Node a, Node b, int weight) {
    this.a = a;
    this.b = b;
    this.weight = weight;
  }

  public static List<Edge> edgesFrom(String[] args) {
    List<Node> nodes = Node.nodesFrom(args);
    List<Edge> edges = new ArrayList<>();
    try {
      for (int i = 0; i < args.length; i = i + 3) {
        Node a = Node.find(args[i], nodes);
        Node b = Node.find(args[i + 1], nodes);
        edges.add(new Edge(a, b, Integer.parseInt(args[i + 2])));
      }
    } catch (NumberFormatException e) {
      Kruskal.printError();
      System.exit(1);
    }
    return edges;
  }

  public String toString() {
    return String.format("E(%s, %s, %d)", a.getKey(), b.getKey(), weight);
  }

  public Node getA() {
    return a;
  }

  public Node getB() {
    return b;
  }

  public int getWeight() {
    return weight;
  }
}

class Kruskal {
  // Odpowiednik funkcji Kruskal(ilW, ilK, E)
  private static void kruskalSpanningTree(List<Edge> edges) {
    for (Edge edge : edges) {
      Node a = edge.getA().findSet();
      Node b = edge.getB().findSet();
      if (a != b) {
        a.union(b);
        System.out.println("  " + edge.toString());
      }
    }
  }

  public static void printError() {
    System.out.println("Argumenty podane w złej formie. Podaj np.:");
    System.out.println("\"java Kruskal a b 1 c d 2\"");
    System.out.println("  - krawędź E(a, b, 1) między punktami a i b o wadze 1");
    System.out.println("  - krawędź E(c, d, 2) między punktami c i d o wadze 2");
    System.out.println("Etykiety punktów są dowolne, waga musi być liczbą całkowitą");
  }

  public static void main(String[] args) {
    if (args.length % 3 != 0) {
      printError();
      System.exit(1);
    }
    if (args.length == 0) {
      System.out.println("Nie podano argumentów, zostaną użyte domyślne");
      // Krawędzie grafu przedstawionego na slajdach z wykładu (kruskalPrz.pdf)
      String[] defaultArgs = {
        "a", "b", "4",
        "a", "h", "9",
        "b", "c", "10",
        "b", "h", "11",
        "c", "d", "9",
        "c", "f", "4",
        "c", "i", "2",
        "d", "e", "7",
        "d", "f", "8",
        "e", "f", "10",
        "f", "g", "2",
        "g", "h", "1",
        "g", "i", "6",
        "h", "i", "7"
      };
      args = defaultArgs;
    }

    List<Edge> edges = Edge.edgesFrom(args);
    // Sortowanie listy krawędzi (niemalejąco) pod względem wagi
    edges.sort((edgeA, edgeB) -> edgeA.getWeight() - edgeB.getWeight());
    System.out.println("Lista krawędzi spinająca drzewo:");
    kruskalSpanningTree(edges);

    /**
     * Prawidłowy zestaw krawędzi dla domyślnej tablicy argumentów:
     * E(g, h, 1)
     * E(c, i, 2)
     * E(f, g, 2)
     * E(a, b, 4)
     * E(c, f, 5)
     * E(d, e, 7)
     * E(d, f, 8)
     * E(a, h, 9)
     */
  }
}
