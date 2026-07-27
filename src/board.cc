export module board;

import <vector>;
import types;
import tile;
import vertex;
import edge;
import residence;

export struct Production {
  Colour colour;
  ResourceType type;
  int amount;
};

export class Board {
  private:
    std::vector<Tile> tiles;
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;

    // Edge -> Vertices
    std::vector<std::vector<int>> edgeVertices;

    // Vertex -> Edges
    std::vector<std::vector<int>> vertexEdges;

    // Vertex -> adjacent Vertices
    std::vector<std::vector<int>> vertexNeighbours;

    // Tile -> Vertices
    std::vector<std::vector<int>> tileVertices;

    // Vertex -> Tiles
    std::vector<std::vector<int>> vertexTiles;

    std::vector<Residence *> residences;

  public:
    Board();
    ~Board();
    Board(const Board &) = delete;
    Board &operator=(const Board &) = delete;

    Tile &getTile(int id);
    Vertex &getVertex(int id);
    Edge &getEdge(int id);

    const Tile &getTile(int id) const;
    const Vertex &getVertex(int id) const;
    const Edge &getEdge(int id) const;

    void initializeVertices(int count);
    void initializeEdges(int count);
    void addTile(ResourceType type, int number);
    void setTile(int id, ResourceType type, int number);

    void connectEdgeToVertices(
        int edgeId,
        int vertex1,
        int vertex2
    );

    const std::vector<int> &getVerticesForEdge(int edgeId) const;
    const std::vector<int> &getEdgesForVertex(int vertexId) const;
    const std::vector<int> &getNeighboursForVertex(int vertexId) const;

    void connectTileToVertices(
        int tileId,
        const std::vector<int> &vertexIds
    );

    const std::vector<int> &getVerticesForTile(int tileId) const;
    const std::vector<int> &getTilesForVertex(int vertexId) const;
    bool canBuildRoad(int edgeId, Colour colour) const;
    void buildRoad(int edgeId, Colour colour);

    bool canBuildResidence(int vertexId, Colour colour) const;
    void buildResidence(int vertexId, Colour colour);

    bool canUpgradeResidence(int vertexId, Colour colour) const;
    void upgradeResidence(int vertexId);

    bool canPlaceInitialResidence(int vertexId) const;
    void placeInitialResidence(int vertexId, Colour colour);

    bool canPlaceInitialRoad(int edgeId, int residenceVertexId) const;
    void placeInitialRoad(int edgeId, Colour colour);
    std::vector<int> getProducingTiles(int roll) const;

    void setupDefaultBoard();
    std::vector<Production> getProduction(int roll) const;

    int getGeeseTile() const;
    bool canMoveGeeseTo(int tileId) const;
    void moveGeeseTo(int tileId);
};
