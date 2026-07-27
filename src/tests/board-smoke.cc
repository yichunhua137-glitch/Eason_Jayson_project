#include <iostream>
#include <string>
#include <vector>

import board;
import textdisplay;
import types;

namespace {

bool fail(const char *message) {
    std::cerr << message << '\n';
    return false;
}

bool checkVector(
    const std::vector<int> &actual,
    const std::vector<int> &expected,
    const char *label
) {
    if (actual.size() != expected.size()) {
        std::cerr << label << " size mismatch\n";
        return false;
    }

    for (std::size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] != expected[i]) {
            std::cerr << label << " mismatch at index " << i << '\n';
            return false;
        }
    }

    return true;
}

} // namespace

int main() {
    Board board;
    board.setupDefaultBoard();

    const std::vector<std::vector<int>> expectedTileVertices = {
        {0, 1, 3, 4, 8, 9},
        {2, 3, 7, 8, 13, 14},
        {4, 5, 9, 10, 15, 16},
        {6, 7, 12, 13, 18, 19},
        {8, 9, 14, 15, 20, 21},
        {10, 11, 16, 17, 22, 23},
        {13, 14, 19, 20, 25, 26},
        {15, 16, 21, 22, 27, 28},
        {18, 19, 24, 25, 30, 31},
        {20, 21, 26, 27, 32, 33},
        {22, 23, 28, 29, 34, 35},
        {25, 26, 31, 32, 37, 38},
        {27, 28, 33, 34, 39, 40},
        {30, 31, 36, 37, 42, 43},
        {32, 33, 38, 39, 44, 45},
        {34, 35, 40, 41, 46, 47},
        {37, 38, 43, 44, 48, 49},
        {39, 40, 45, 46, 50, 51},
        {44, 45, 49, 50, 52, 53}
    };

    const std::vector<std::vector<int>> expectedEdgeVertices = {
        {0, 1}, {0, 3}, {1, 4}, {2, 3}, {4, 5}, {2, 7}, {3, 8}, {4, 9},
        {5, 10}, {6, 7}, {8, 9}, {10, 11}, {6, 12}, {7, 13}, {8, 14},
        {9, 15}, {10, 16}, {11, 17}, {13, 14}, {15, 16}, {12, 18},
        {13, 19}, {14, 20}, {15, 21}, {16, 22}, {17, 23}, {18, 19},
        {20, 21}, {22, 23}, {18, 24}, {19, 25}, {20, 26}, {21, 27},
        {22, 28}, {23, 29}, {25, 26}, {27, 28}, {24, 30}, {25, 31},
        {26, 32}, {27, 33}, {28, 34}, {29, 35}, {30, 31}, {32, 33},
        {34, 35}, {30, 36}, {31, 37}, {32, 38}, {33, 39}, {34, 40},
        {35, 41}, {37, 38}, {39, 40}, {36, 42}, {37, 43}, {38, 44},
        {39, 45}, {40, 46}, {41, 47}, {42, 43}, {44, 45}, {46, 47},
        {43, 48}, {44, 49}, {45, 50}, {46, 51}, {48, 49}, {50, 51},
        {49, 52}, {50, 53}, {52, 53}
    };

    for (int tileId = 0; tileId < 19; ++tileId) {
        if (!checkVector(
                board.getVerticesForTile(tileId),
                expectedTileVertices[tileId],
                "Tile vertices"
            )) {
            std::cerr << "Failed on tile " << tileId << '\n';
            return 1;
        }
    }

    for (int edgeId = 0; edgeId < 72; ++edgeId) {
        if (!checkVector(
                board.getVerticesForEdge(edgeId),
                expectedEdgeVertices[edgeId],
                "Edge vertices"
            )) {
            std::cerr << "Failed on edge " << edgeId << '\n';
            return 1;
        }
    }

    if (board.getTile(18).getType() != ResourceType::PARK) {
        return fail("Tile 18 should be PARK.");
    }

    if (board.getTile(18).getNumber() != 0) {
        return fail("PARK tile should use number 0.");
    }

    if (board.getGeeseTile() != 18) {
        return fail("Initial geese should be on tile 18.");
    }

    if (board.canPlaceInitialRoad(3, 0)) {
        return fail("Edge 3 should not be a valid initial road for residence 0.");
    }

    if (!board.canPlaceInitialResidence(0)) {
        return fail("Vertex 0 should allow an initial residence.");
    }

    board.placeInitialResidence(0, Colour::BLUE);

    if (board.canPlaceInitialResidence(1)) {
        return fail("Vertex 1 should be blocked by adjacent initial residence at 0.");
    }

    if (!board.canPlaceInitialRoad(0, 0)) {
        return fail("Edge 0 should be a valid initial road for residence 0.");
    }

    board.placeInitialRoad(0, Colour::BLUE);

    if (board.canBuildRoad(0, Colour::RED)) {
        return fail("Built edge 0 should not allow another road.");
    }

    if (!board.canBuildRoad(1, Colour::BLUE)) {
        return fail("Blue should be able to build road 1 from residence 0.");
    }

    board.buildRoad(1, Colour::BLUE);

    if (!board.canBuildRoad(6, Colour::BLUE)) {
        return fail("Blue should be able to continue road chain to edge 6.");
    }

    board.buildRoad(6, Colour::BLUE);

    if (board.canBuildResidence(3, Colour::BLUE)) {
        return fail("Vertex 3 should be blocked by adjacency to residence 0.");
    }

    if (board.canBuildResidence(8, Colour::RED)) {
        return fail("Red should not be able to build at vertex 8 without a red road.");
    }

    if (!board.canBuildResidence(8, Colour::BLUE)) {
        return fail("Blue should be able to build at vertex 8 after connecting roads.");
    }

    board.buildResidence(8, Colour::BLUE);

    if (!board.canUpgradeResidence(8, Colour::BLUE)) {
        return fail("Blue residence at 8 should be upgradable from basement.");
    }

    board.upgradeResidence(8);

    if (board.getVertex(8).getResidence()->getType() != ResidenceType::HOUSE) {
        return fail("Residence at 8 should be a HOUSE after one upgrade.");
    }

    if (board.getVertex(8).getResidence()->getProductionAmount() != 2) {
        return fail("HOUSE at 8 should produce 2 resources.");
    }

    board.upgradeResidence(8);

    if (board.getVertex(8).getResidence()->getType() != ResidenceType::TOWER) {
        return fail("Residence at 8 should be a TOWER after two upgrades.");
    }

    if (board.getVertex(8).getResidence()->getProductionAmount() != 3) {
        return fail("TOWER at 8 should produce 3 resources.");
    }

    if (board.canUpgradeResidence(8, Colour::BLUE)) {
        return fail("TOWER at 8 should not be upgradable again.");
    }

    std::vector<int> rollTwoTiles = board.getProducingTiles(2);
    if (!checkVector(
            rollTwoTiles,
            std::vector<int>{0},
            "Producing tiles for roll 2"
        )) {
        return 1;
    }

    if (!board.getProducingTiles(0).empty()) {
        return fail("PARK should never produce resources.");
    }

    std::vector<Production> production = board.getProduction(2);
    if (production.size() != 2) {
        return fail("Roll 2 should create two production records.");
    }

    int totalBrick = 0;
    for (const Production &entry : production) {
        if (entry.colour != Colour::BLUE) {
            return fail("All production on roll 2 should belong to Blue.");
        }

        if (entry.type != ResourceType::BRICK) {
            return fail("Roll 2 production should be BRICK from tile 0.");
        }

        totalBrick += entry.amount;
    }

    if (totalBrick != 4) {
        return fail("Blue should receive total production amount 4 on roll 2.");
    }

    if (board.canMoveGeeseTo(18)) {
        return fail("Geese should not be movable onto their current tile.");
    }

    if (!board.canMoveGeeseTo(0)) {
        return fail("Geese should be movable from PARK to tile 0.");
    }

    board.moveGeeseTo(0);

    if (board.getGeeseTile() != 0) {
        return fail("Geese should be on tile 0 after moving.");
    }

    if (!board.getProduction(2).empty()) {
        return fail("Tile 0 should not produce while geese are on it.");
    }

    TextDisplay display{board};
    std::string formal = display.renderFormal();
    std::string debug = display.renderDebug();

    if (formal.empty() || debug.empty()) {
        return fail("TextDisplay should render non-empty strings.");
    }

    if (formal.find("BRICK") == std::string::npos) {
        return fail("Formal display should contain tile resource text.");
    }

    if (debug.find("Tiles") == std::string::npos ||
        debug.find("Edges") == std::string::npos) {
        return fail("Debug display should contain section headers.");
    }

    if (debug.find("geese=yes") == std::string::npos) {
        return fail("Debug display should show geese state.");
    }

    std::cout << "board-smoke passed\n";
    return 0;
}
