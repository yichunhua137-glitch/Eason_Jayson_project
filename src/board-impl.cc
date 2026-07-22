module board;

Board::Board() {}

Tile &Board::getTile(int id) {
    return tiles[id];
}

Vertex &Board::getVertex(int id) {
    return vertices[id];
}

Edge &Board::getEdge(int id) {
    return edges[id];
}

const Tile &Board::getTile(int id) const {
    return tiles[id];
}

const Vertex &Board::getVertex(int id) const {
    return vertices[id];
}

const Edge &Board::getEdge(int id) const {
    return edges[id];
}

void Board::initializeVertices(int count) {
    vertices.clear();
    vertexEdges.clear();
    vertexNeighbours.clear();
    vertexTiles.clear();

    vertexEdges.resize(count);
    vertexNeighbours.resize(count);
    vertexTiles.resize(count);

    for (int i = 0; i < count; ++i) {
        vertices.emplace_back(i);
    }
}

void Board::initializeEdges(int count) {
    edges.clear();
    edgeVertices.clear();

    edgeVertices.resize(count);

    for (int i = 0; i < count; ++i) {
        edges.emplace_back(i);
    }
}

void Board::addTile(ResourceType type, int number) {
    int id = static_cast<int>(tiles.size());

    tiles.emplace_back(id, type, number);
    tileVertices.emplace_back();
}

void Board::connectEdgeToVertices(
    int edgeId,
    int vertex1,
    int vertex2
) {
    edgeVertices[edgeId] = {vertex1, vertex2};

    vertexEdges[vertex1].push_back(edgeId);
    vertexEdges[vertex2].push_back(edgeId);

    vertexNeighbours[vertex1].push_back(vertex2);
    vertexNeighbours[vertex2].push_back(vertex1);
}

const std::vector<int> &Board::getVerticesForEdge(int edgeId) const {
    return edgeVertices[edgeId];
}

const std::vector<int> &Board::getEdgesForVertex(int vertexId) const {
    return vertexEdges[vertexId];
}

const std::vector<int> &Board::getNeighboursForVertex(int vertexId) const {
    return vertexNeighbours[vertexId];
}

void Board::connectTileToVertices(
    int tileId,
    const std::vector<int> &vertexIds
) {
    tileVertices[tileId] = vertexIds;

    for (int vertexId : vertexIds) {
        vertexTiles[vertexId].push_back(tileId);
    }
}

const std::vector<int> &Board::getVerticesForTile(int tileId) const {
    return tileVertices[tileId];
}

const std::vector<int> &Board::getTilesForVertex(int vertexId) const {
    return vertexTiles[vertexId];
}

bool Board::canBuildRoad(int edgeId, Colour colour) const {
    if (edges[edgeId].isBuilt()) {
        return false;
    }

    const std::vector<int> &ends = edgeVertices[edgeId];

    for (int vertexId : ends) {
        const Vertex &vertex = vertices[vertexId];

        if (vertex.hasResidence()) {
            Residence *residence = vertex.getResidence();

            if (residence->getOwner() == colour) {
                return true;
            }

            // Another player's residence blocks connection
            continue;
        }

        for (int connectedEdgeId : vertexEdges[vertexId]) {
            if (connectedEdgeId == edgeId) {
                continue;
            }

            const Edge &connectedEdge = edges[connectedEdgeId];

            if (
                connectedEdge.isBuilt() &&
                connectedEdge.getOwner() == colour
            ) {
                return true;
            }
        }
    }

    return false;
}