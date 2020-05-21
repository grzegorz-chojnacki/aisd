import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

class Vertex {
  private String key;
  private Vertex connectedWith;
  private boolean wasVisited = false;
  private List<Vertex> neighbours = new ArrayList<>();

  public Vertex(String key) { this.key = key; }
  public boolean isConnected() { return connectedWith != null; }
  public void addNeighbour(Vertex neighbour) { neighbours.add(neighbour); }
  public boolean wasNotVisited() { return !wasVisited; }
  public String getKey() { return key; }
  public void visit() { wasVisited = true; }
  public void connectWith(Vertex visitor) { connectedWith = visitor; }
  public List<Vertex> getNeighbours() { return neighbours; }
  public Vertex getConnected() { return connectedWith; }

  // Metoda nadająca odpowiedni porządek przy wyświetlaniu krawędzi
  public int pairOrder(Vertex that) {
    return this.getConnected().getKey()
      .compareTo(that.getConnected().getKey());
  }

  // Metoda pomagająca ominąć ponowne wczytywanie wierzchołków z pliku
  public void reset() {
    wasVisited = false;
    connectedWith = null;
  }

  public String edgeToString() {
    if (isConnected()) return String.format("(%s, %s)", connectedWith.getKey(), key);
    else return String.format("(%s)", key);
  }
}

class DFS {
  public static List<Vertex> search(List<Vertex> vertices) {
    vertices.stream()
      .filter(Vertex::wasNotVisited)
      .forEach(u -> search(vertices, u));
    return vertices;
  }

  private static void search(List<Vertex> vertices, Vertex u) {
    u.visit();
    for (Vertex v : u.getNeighbours()) {
      if (v.wasNotVisited()) {
        v.connectWith(u);
        search(vertices, v);
      }
    }
  }
}

class BFS {
  private static Queue<Vertex> queue = new LinkedList<>();

  public static List<Vertex> search(List<Vertex> vertices) {
    vertices.stream()
      .filter(Vertex::wasNotVisited)
      .forEach(u -> search(vertices, u));
    return vertices;
  }

  private static void search(List<Vertex> vertices, Vertex s) {
    s.visit();
    queue.add(s);
    while (!queue.isEmpty()) {
      Vertex u = queue.remove();
      for (Vertex v : u.getNeighbours()) {
        if (v.wasNotVisited()) {
          v.visit();
          v.connectWith(u);
          queue.add(v);
        }
      }
    }
  }

}

class GraphTraversal {
  private static void printNeighbourTable(List<Vertex> vertices) {
    System.out.println("Wczytana tablica sąsiedztw: ");
    for (Vertex vertex : vertices) {
      System.out.print(vertex.getKey() + ": ");
      vertex.getNeighbours().stream()
        .map(Vertex::getKey)
        .forEach(v -> System.out.print(v + " "));
      System.out.println();
    }
    System.out.println();
  }

  private static void printEdges(List<Vertex> vertices) {
    System.out.println("Znalezione krawędzie spinające:");
    vertices.stream()
      .filter(Vertex::isConnected)
      .sorted((u, v) -> u.pairOrder(v))
      .map(Vertex::edgeToString)
      .forEach(System.out::println);
    System.out.println();
  }

  // Wczytywanie informacji o wierzchołkach z podanego pliku
  private static List<Vertex> loadVertices(String fileName) {
    try (Scanner scanner = new Scanner(new File(fileName))) {
      int numberOfVertices = 0;
      if (scanner.hasNext()) numberOfVertices = scanner.nextInt();
      else throw new IllegalArgumentException();

      // Tworzenie pustych wierzchołków
      List<Vertex> vertices = IntStream.range(0, numberOfVertices)
        .mapToObj(Integer::toString)
        .map(Vertex::new)
        .collect(Collectors.toList());

      int vertexCounter = 0;

      ListIterator<Vertex> verticesIterator = vertices.listIterator();
      Vertex vertexA;

      // Przechodzenie macierzy sąsiedztw
      while (scanner.hasNext()) {
        vertexA = verticesIterator.next();
        for (Vertex vertexB : vertices) {
          if (scanner.nextInt() == 1)
            vertexA.addNeighbour(vertexB);
          vertexCounter++;
        }
      }

      if (vertexCounter != Math.pow(numberOfVertices, 2))
        throw new IllegalArgumentException();

      return vertices;
    } catch (FileNotFoundException e) {
      System.out.println("Nie znaleziono podanego pliku.");
      System.exit(2);
    } catch (Exception e) {
      // Wszystkie pozostałe wyjątki wynikają ze złego formatu pliku
      System.out.println("Podany plik ma zły format.");
      System.exit(3);
    }
    return null; // Wymagana wartość zwrotna (program zawsze zakończy działanie w przypadku błędu)
  }

  public static void main(String[] args) {
    if (args.length != 1) {
      System.out.println("Jako argument należy podać ścieżkę do pliku z macierzą sąsiedztw.");
      System.exit(1);
    }

    List<Vertex> vertices = loadVertices(args[0]);
    printNeighbourTable(vertices);

    System.out.println("Algorytm DFS:");
    List<Vertex> searchedDFS = DFS.search(vertices);
    printEdges(searchedDFS);

    // Zresetowanie stanu wierzchołków:
    vertices.forEach(Vertex::reset);

    System.out.println("Algorytm BFS:");
    List<Vertex> searchedBFS = BFS.search(vertices);
    printEdges(searchedBFS);
  }
}
