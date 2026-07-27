module board;

import residence;

// Initialize an empty board object.
// 初始化一个空的 board 对象。
Board::Board() {}

// Release every dynamically allocated residence owned by the board.
// 释放 board 持有的所有动态分配的 residence。
Board::~Board() {
    for (Residence *residence : residences) {
        delete residence;
    }
}

// Return mutable access to a tile by id.
// 按编号返回一个可修改的 tile。
Tile &Board::getTile(int id) {
    return tiles[id];
}

// Return mutable access to a vertex by id.
// 按编号返回一个可修改的 vertex。
Vertex &Board::getVertex(int id) {
    return vertices[id];
}

// Return mutable access to an edge by id.
// 按编号返回一个可修改的 edge。
Edge &Board::getEdge(int id) {
    return edges[id];
}

// Return read-only access to a tile by id.
// 按编号返回一个只读 tile。
const Tile &Board::getTile(int id) const {
    return tiles[id];
}

// Return read-only access to a vertex by id.
// 按编号返回一个只读 vertex。
const Vertex &Board::getVertex(int id) const {
    return vertices[id];
}

// Return read-only access to an edge by id.
// 按编号返回一个只读 edge。
const Edge &Board::getEdge(int id) const {
    return edges[id];
}

// Rebuild the vertex storage and reset all vertex-based adjacency tables.
// 重新建立 vertex 存储，并清空所有基于 vertex 的邻接表。
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

// Rebuild the edge storage and reset the edge-to-vertices table.
// 重新建立 edge 存储，并清空 edge 到 vertices 的映射。
void Board::initializeEdges(int count) {
    edges.clear();
    edgeVertices.clear();

    edgeVertices.resize(count);

    for (int i = 0; i < count; ++i) {
        edges.emplace_back(i);
    }
}

// Append one tile to the board and reserve its tile-to-vertices entry.
// 向 board 追加一个 tile，并为它准备 tile-to-vertices 的槽位。
void Board::addTile(ResourceType type, int number) {
    int id = static_cast<int>(tiles.size());

    tiles.emplace_back(id, type, number);
    tileVertices.emplace_back();
}

// Connect one edge to its two endpoint vertices and update reverse adjacency tables.
// 把一条 edge 连接到两个端点 vertex，并同步更新反向邻接表。
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

// Return the two endpoint vertices of one edge.
// 返回一条 edge 的两个端点 vertex。
const std::vector<int> &Board::getVerticesForEdge(int edgeId) const {
    return edgeVertices[edgeId];
}

// Return all edges adjacent to one vertex.
// 返回一个 vertex 相邻的所有 edge。
const std::vector<int> &Board::getEdgesForVertex(int vertexId) const {
    return vertexEdges[vertexId];
}

// Return all neighbouring vertices adjacent to one vertex.
// 返回一个 vertex 相邻的所有 vertex。
const std::vector<int> &Board::getNeighboursForVertex(int vertexId) const {
    return vertexNeighbours[vertexId];
}

// Connect one tile to its six surrounding vertices and update vertex-to-tiles.
// 把一个 tile 连接到周围六个 vertex，并更新 vertex-to-tiles 关系。
void Board::connectTileToVertices(
    int tileId,
    const std::vector<int> &vertexIds
) {
    tileVertices[tileId] = vertexIds;

    for (int vertexId : vertexIds) {
        vertexTiles[vertexId].push_back(tileId);
    }
}

// Return the six vertices surrounding one tile.
// 返回一个 tile 周围的六个 vertex。
const std::vector<int> &Board::getVerticesForTile(int tileId) const {
    return tileVertices[tileId];
}

// Return all tiles adjacent to one vertex.
// 返回一个 vertex 相邻的所有 tile。
const std::vector<int> &Board::getTilesForVertex(int vertexId) const {
    return vertexTiles[vertexId];
}

// Check whether a builder can legally build a road on the given edge.
// 检查某个 builder 是否可以合法地在指定 edge 上建 road。
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

// Build a road on the given edge for the given builder colour.
// 在指定 edge 上为指定颜色建一条 road。
void Board::buildRoad(int edgeId, Colour colour) {
    edges[edgeId].build(colour);
}

// Check whether a builder can legally build a residence on the given vertex.
// 检查某个 builder 是否可以合法地在指定 vertex 上建 residence。
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

// Create and place a new residence owned by the given builder.
// 创建并放置一个属于指定 builder 的新 residence。
void Board::buildResidence(int vertexId, Colour colour) {
    Residence *residence = new Residence{colour};

    residences.push_back(residence);
    vertices[vertexId].buildResidence(residence);
}

// Check whether the residence on a vertex can be upgraded by this builder.
// 检查一个 vertex 上的 residence 是否能被该 builder 升级。
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

// Upgrade the residence already stored on the given vertex.
// 升级指定 vertex 上已经存在的 residence。
void Board::upgradeResidence(int vertexId) {
    vertices[vertexId].getResidence()->upgrade();
}

// Check whether a vertex is legal for initial residence placement.
// 检查一个 vertex 是否适合开局放置 residence。
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

// Place an initial residence by reusing the normal residence-building helper.
// 复用普通建房逻辑来放置开局 residence。
void Board::placeInitialResidence(int vertexId, Colour colour) {
    buildResidence(vertexId, colour);
}

// Check whether an initial road touches the residence chosen in setup.
// 检查开局 road 是否与当前选中的 residence 相接。
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

// Place an initial road by reusing the normal road-building helper.
// 复用普通建路逻辑来放置开局 road。
void Board::placeInitialRoad(int edgeId, Colour colour) {
    buildRoad(edgeId, colour);
}

// Return every tile id that should produce for this dice roll.
// 返回这次骰子点数下所有应该生产资源的 tile 编号。
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

// Build the fixed default Constructor board layout and reset old board state.
// 建立固定的默认 Constructor 棋盘布局，并重置旧的 board 状态。
void Board::setupDefaultBoard(){
    for (Residence *residence : residences) {
        delete residence;
    }
    residences.clear();

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

    tiles.clear();
    tileVertices.clear();

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

    initializeVertices(54);
    initializeEdges(72);

    for (int i = 0; i < 19; ++i) {
        addTile(resourceTypes[i], values[i]);
    }

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

// Convert one dice roll into production records without directly mutating builders.
// 把一次掷骰结果转换成 production records，但不直接修改 builder。
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

// Find which tile currently contains the geese.
// 找出当前哪一个 tile 上有 geese。
int Board::getGeeseTile() const {
    for (const Tile &tile : tiles) {
        if (tile.hasGeeseOnTile()) {
            return tile.getId();
        }
    }
    return -1;
}

// Check whether geese are allowed to move onto the target tile.
// 检查 geese 是否允许移动到目标 tile。
bool Board::canMoveGeeseTo(int tileId) const {
    if (tileId < 0 || tileId >= static_cast<int>(tiles.size())) {
        return false;
    }

    if (tileId == getGeeseTile()) {
        return false;
    }
    
    return true;
}

// Move geese from their current tile onto a new target tile.
// 把 geese 从当前 tile 移动到新的目标 tile。
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
    } else if (id ==
               static_cast<int>(tiles.size())) {
        addTile(type, number);
    }
}