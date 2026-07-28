module board;

import residence;

Board::Board() {}

Board::~Board() {
    clear();
}

void Board::clear() {
    for (Residence *residence : residences) {
        delete residence;
    }

    residences.clear();
    tiles.clear();
    vertices.clear();
    edges.clear();
    edgeVertices.clear();
    vertexEdges.clear();
    vertexNeighbours.clear();
    tileVertices.clear();
    vertexTiles.clear();
}

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

void Board::buildRoad(int edgeId, Colour colour) {
    edges[edgeId].build(colour);
}

bool Board::canBuildResidence(int vertexId, Colour colour) const {
    if (vertices[vertexId].hasResidence()) {
        return false;
    }

    for (int neighbourId : vertexNeighbours[vertexId]) {
        if (vertices[neighbourId].hasResidence()) {
            return false;
        }
    }

    for (int edgeId : vertexEdges[vertexId]) {
        const Edge &edge = edges[edgeId];

        if (edge.isBuilt() && edge.getOwner() == colour) {
            return true;
        }
    }

    return false;
}

void Board::buildResidence(int vertexId, Colour colour) {
    Residence *residence = new Residence{colour};

    residences.push_back(residence);
    vertices[vertexId].buildResidence(residence);
}

bool Board::canUpgradeResidence(int vertexId, Colour colour) const {
    const Vertex &vertex = vertices[vertexId];
    if (!vertex.hasResidence()) {
        return false;
    }

    const Residence *residence = vertex.getResidence();

    if (residence->getOwner() != colour) {
        return false;
    }

    if (residence->getType() == ResidenceType::TOWER) {
        return false;
    }

    return true;
}

void Board::upgradeResidence(int vertexId) {
    vertices[vertexId].getResidence()->upgrade();
}

bool Board::canPlaceInitialResidence(int vertexId) const {
    if (vertices[vertexId].hasResidence()) {
        return false;
    }

    for (int neighbourId : vertexNeighbours[vertexId]) {
        if (vertices[neighbourId].hasResidence()) {
            return false;
        }
    }

    return true;
}

void Board::placeInitialResidence(int vertexId, Colour colour) {
    buildResidence(vertexId, colour);
}

bool Board::canPlaceInitialRoad(
    int edgeId,
    int residenceVertexId
) const {
    if (edges[edgeId].isBuilt()) {
        return false;
    }

    for (int vertexId : edgeVertices[edgeId]) {
        if (vertexId == residenceVertexId) {
            return true;
        }
    }

    return false;
}

void Board::placeInitialRoad(int edgeId, Colour colour) {
    buildRoad(edgeId, colour);
}

std::vector<int> Board::getProducingTiles(int roll) const {
    std::vector<int> result;

    for (const Tile &tile : tiles) {
        if (
            tile.getType() != ResourceType::PARK &&
            tile.getNumber() == roll &&
            !tile.hasGeeseOnTile()
        ) {
            result.push_back(tile.getId());
        }
    }

    return result;
}

void Board::setupDefaultBoard() {
    clear();

    ResourceType resourceTypes[19] = {
        ResourceType::BRICK,
        ResourceType::BRICK,
        ResourceType::BRICK,
        ResourceType::BRICK,

        ResourceType::ENERGY,
        ResourceType::ENERGY,
        ResourceType::ENERGY,
        ResourceType::ENERGY,

        ResourceType::GLASS,
        ResourceType::GLASS,
        ResourceType::GLASS,
        ResourceType::GLASS,

        ResourceType::HEAT,
        ResourceType::HEAT,
        ResourceType::HEAT,

        ResourceType::WIFI,
        ResourceType::WIFI,
        ResourceType::WIFI,

        ResourceType::PARK
    };

    int values[19] = {
        2, 12,
        3, 3,
        4, 4,
        5, 5,
        6, 6,
        8, 8,
        9, 9,
        10, 10,
        11, 11,
        0
    };

    for (int i = 0; i < 19; ++i) {
        addTile(resourceTypes[i], values[i]);
    }

    setupTopology();
}

void Board::setupTopology() {
    for (Residence *residence : residences) {
        delete residence;
    }
    residences.clear();

    initializeVertices(54);
    initializeEdges(72);

    tileVertices.clear();
    tileVertices.resize(19);

    connectTileToVertices(0, {0, 1, 3, 4, 8, 9});

    connectTileToVertices(1, {2, 3, 7, 8, 13, 14});
    connectTileToVertices(2, {4, 5, 9, 10, 15, 16});

    connectTileToVertices(3, {6, 7, 12, 13, 18, 19});
    connectTileToVertices(4, {8, 9, 14, 15, 20, 21});
    connectTileToVertices(5, {10, 11, 16, 17, 22, 23});

    connectTileToVertices(6, {13, 14, 19, 20, 25, 26});
    connectTileToVertices(7, {15, 16, 21, 22, 27, 28});

    connectTileToVertices(8, {18, 19, 24, 25, 30, 31});
    connectTileToVertices(9, {20, 21, 26, 27, 32, 33});
    connectTileToVertices(10, {22, 23, 28, 29, 34, 35});

    connectTileToVertices(11, {25, 26, 31, 32, 37, 38});
    connectTileToVertices(12, {27, 28, 33, 34, 39, 40});

    connectTileToVertices(13, {30, 31, 36, 37, 42, 43});
    connectTileToVertices(14, {32, 33, 38, 39, 44, 45});
    connectTileToVertices(15, {34, 35, 40, 41, 46, 47});

    connectTileToVertices(16, {37, 38, 43, 44, 48, 49});
    connectTileToVertices(17, {39, 40, 45, 46, 50, 51});

    connectTileToVertices(18, {44, 45, 49, 50, 52, 53});

    connectEdgeToVertices(0, 0, 1);
    connectEdgeToVertices(1, 0, 3);
    connectEdgeToVertices(2, 1, 4);

    connectEdgeToVertices(3, 2, 3);
    connectEdgeToVertices(4, 4, 5);

    connectEdgeToVertices(5, 2, 7);
    connectEdgeToVertices(6, 3, 8);
    connectEdgeToVertices(7, 4, 9);
    connectEdgeToVertices(8, 5, 10);

    connectEdgeToVertices(9, 6, 7);
    connectEdgeToVertices(10, 8, 9);
    connectEdgeToVertices(11, 10, 11);

    connectEdgeToVertices(12, 6, 12);
    connectEdgeToVertices(13, 7, 13);
    connectEdgeToVertices(14, 8, 14);
    connectEdgeToVertices(15, 9, 15);
    connectEdgeToVertices(16, 10, 16);
    connectEdgeToVertices(17, 11, 17);

    connectEdgeToVertices(18, 13, 14);
    connectEdgeToVertices(19, 15, 16);

    connectEdgeToVertices(20, 12, 18);
    connectEdgeToVertices(21, 13, 19);
    connectEdgeToVertices(22, 14, 20);
    connectEdgeToVertices(23, 15, 21);
    connectEdgeToVertices(24, 16, 22);
    connectEdgeToVertices(25, 17, 23);

    connectEdgeToVertices(26, 18, 19);
    connectEdgeToVertices(27, 20, 21);
    connectEdgeToVertices(28, 22, 23);

    connectEdgeToVertices(29, 18, 24);
    connectEdgeToVertices(30, 19, 25);
    connectEdgeToVertices(31, 20, 26);
    connectEdgeToVertices(32, 21, 27);
    connectEdgeToVertices(33, 22, 28);
    connectEdgeToVertices(34, 23, 29);

    connectEdgeToVertices(35, 25, 26);
    connectEdgeToVertices(36, 27, 28);

    connectEdgeToVertices(37, 24, 30);
    connectEdgeToVertices(38, 25, 31);
    connectEdgeToVertices(39, 26, 32);
    connectEdgeToVertices(40, 27, 33);
    connectEdgeToVertices(41, 28, 34);
    connectEdgeToVertices(42, 29, 35);

    connectEdgeToVertices(43, 30, 31);
    connectEdgeToVertices(44, 32, 33);
    connectEdgeToVertices(45, 34, 35);

    connectEdgeToVertices(46, 30, 36);
    connectEdgeToVertices(47, 31, 37);
    connectEdgeToVertices(48, 32, 38);
    connectEdgeToVertices(49, 33, 39);
    connectEdgeToVertices(50, 34, 40);
    connectEdgeToVertices(51, 35, 41);

    connectEdgeToVertices(52, 37, 38);
    connectEdgeToVertices(53, 39, 40);

    connectEdgeToVertices(54, 36, 42);
    connectEdgeToVertices(55, 37, 43);
    connectEdgeToVertices(56, 38, 44);
    connectEdgeToVertices(57, 39, 45);
    connectEdgeToVertices(58, 40, 46);
    connectEdgeToVertices(59, 41, 47);

    connectEdgeToVertices(60, 42, 43);
    connectEdgeToVertices(61, 44, 45);
    connectEdgeToVertices(62, 46, 47);

    connectEdgeToVertices(63, 43, 48);
    connectEdgeToVertices(64, 44, 49);
    connectEdgeToVertices(65, 45, 50);
    connectEdgeToVertices(66, 46, 51);

    connectEdgeToVertices(67, 48, 49);
    connectEdgeToVertices(68, 50, 51);

    connectEdgeToVertices(69, 49, 52);
    connectEdgeToVertices(70, 50, 53);

    connectEdgeToVertices(71, 52, 53);
}

std::vector<Production> Board::getProduction(int roll) const {
    std::vector<Production> result;

    std::vector<int> producingTiles = getProducingTiles(roll);

    for (int tileId : producingTiles) {
        const Tile &tile = getTile(tileId);
        const std::vector<int> &verticesForTile =
            getVerticesForTile(tileId);

        for (int vertexId : verticesForTile) {
            const Vertex &vertex = getVertex(vertexId);

            if (!vertex.hasResidence()) {
                continue;
            }

            Residence *residence = vertex.getResidence();

            result.push_back(
                Production{
                    residence->getOwner(),
                    tile.getType(),
                    residence->getProductionAmount()
                }
            );
        }
    }

    return result;
}

int Board::getGeeseTile() const {
    for (const Tile &tile : tiles) {
        if (tile.hasGeeseOnTile()) {
            return tile.getId();
        }
    }
    return -1;
}

bool Board::canMoveGeeseTo(int tileId) const {
    if (tileId < 0 || tileId >= static_cast<int>(tiles.size())) {
        return false;
    }

    if (tileId == getGeeseTile()) {
        return false;
    }

    return true;
}

void Board::moveGeeseTo(int tileId) {
    if (!canMoveGeeseTo(tileId)) {
        return;
    }

    int currentGeeseTile = getGeeseTile();

    if (currentGeeseTile != -1) {
        tiles[currentGeeseTile].removeGeese();
    }

    tiles[tileId].placeGeese();
}

void Board::setTile(
    int id,
    ResourceType type,
    int number
) {
    if (id < static_cast<int>(tiles.size())) {
        tiles[id] = Tile{id, type, number};
    } else if (id == static_cast<int>(tiles.size())) {
        addTile(type, number);
    }
}
