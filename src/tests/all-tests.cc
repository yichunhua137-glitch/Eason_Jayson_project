#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

import board;
import boardsetupstrategy;
import builder;
import dice;
import edge;
import gamecontroller;
import gamestateio;
import residence;
import textdisplay;
import tile;
import types;
import vertex;

namespace {

class TestSuite {
    int checks = 0;
    int failures = 0;

  public:
    void expect(bool condition, const std::string &message) {
        ++checks;
        if (!condition) {
            ++failures;
            std::cerr << "FAIL: " << message << '\n';
        }
    }

    template<typename T, typename U>
    void expectEqual(
        const T &actual,
        const U &expected,
        const std::string &message
    ) {
        ++checks;
        if (!(actual == expected)) {
            ++failures;
            std::cerr << "FAIL: " << message
                      << " (actual=" << actual
                      << ", expected=" << expected << ")\n";
        }
    }

    int finish() const {
        if (failures == 0) {
            std::cout << "all-tests passed: "
                      << checks << " checks\n";
            return 0;
        }

        std::cerr << "all-tests failed: "
                  << failures << " of "
                  << checks << " checks failed\n";
        return 1;
    }
};

class FixedDice final : public Dice {
    int value;

  public:
    explicit FixedDice(int value) : value{value} {}
    int roll() override { return value; }
};

class StreamRedirect {
    std::istream *input;
    std::ostream *output;
    std::streambuf *oldInput = nullptr;
    std::streambuf *oldOutput = nullptr;

  public:
    StreamRedirect(
        std::istream &input,
        std::streambuf *replacement
    ) : input{&input}, output{nullptr},
        oldInput{input.rdbuf(replacement)} {}

    StreamRedirect(
        std::ostream &output,
        std::streambuf *replacement
    ) : input{nullptr}, output{&output},
        oldOutput{output.rdbuf(replacement)} {}

    ~StreamRedirect() {
        if (input != nullptr) {
            input->rdbuf(oldInput);
            input->clear();
        }
        if (output != nullptr) {
            output->rdbuf(oldOutput);
        }
    }
};

void testValueObjects(TestSuite &suite) {
    Tile tile{7, ResourceType::GLASS, 8};
    suite.expectEqual(tile.getId(), 7, "Tile stores its id");
    suite.expect(
        tile.getType() == ResourceType::GLASS,
        "Tile stores its resource type"
    );
    suite.expectEqual(tile.getNumber(), 8, "Tile stores its value");
    suite.expect(!tile.hasGeeseOnTile(), "Tile starts without geese");
    tile.placeGeese();
    suite.expect(tile.hasGeeseOnTile(), "Tile accepts geese");
    tile.removeGeese();
    suite.expect(!tile.hasGeeseOnTile(), "Tile removes geese");

    Residence residence{Colour::RED};
    suite.expect(
        residence.getOwner() == Colour::RED,
        "Residence stores owner"
    );
    suite.expect(
        residence.getType() == ResidenceType::BASEMENT,
        "Residence starts as basement"
    );
    suite.expectEqual(
        residence.getProductionAmount(),
        1,
        "Basement produces one"
    );
    residence.upgrade();
    suite.expect(
        residence.getType() == ResidenceType::HOUSE,
        "Basement upgrades to house"
    );
    suite.expectEqual(
        residence.getProductionAmount(),
        2,
        "House produces two"
    );
    residence.upgrade();
    suite.expect(
        residence.getType() == ResidenceType::TOWER,
        "House upgrades to tower"
    );
    suite.expectEqual(
        residence.getProductionAmount(),
        3,
        "Tower produces three"
    );
    residence.upgrade();
    suite.expect(
        residence.getType() == ResidenceType::TOWER,
        "Tower cannot upgrade further"
    );

    Vertex vertex{4};
    suite.expectEqual(vertex.getId(), 4, "Vertex stores id");
    suite.expect(!vertex.hasResidence(), "Vertex starts empty");
    suite.expect(
        vertex.getResidence() == nullptr,
        "Empty vertex returns null residence"
    );
    vertex.buildResidence(&residence);
    suite.expect(vertex.hasResidence(), "Vertex stores residence");
    suite.expect(
        vertex.getResidence() == &residence,
        "Vertex returns stored residence"
    );

    Edge edge{9};
    suite.expectEqual(edge.getId(), 9, "Edge stores id");
    suite.expect(!edge.isBuilt(), "Edge starts unbuilt");
    suite.expect(
        edge.getOwner() == Colour::NONE,
        "Unbuilt edge has no owner"
    );
    edge.build(Colour::ORANGE);
    suite.expect(edge.isBuilt(), "Edge can be built");
    suite.expect(
        edge.getOwner() == Colour::ORANGE,
        "Built edge stores owner"
    );
}

void testBuilderAndDice(TestSuite &suite) {
    FixedDice two{2};
    FixedDice twelve{12};
    Builder builder{Colour::BLUE, two};

    suite.expect(
        builder.getColour() == Colour::BLUE,
        "Builder stores colour"
    );
    for (int type = 0; type <= 5; ++type) {
        suite.expectEqual(
            builder.getResource(
                static_cast<ResourceType>(type)
            ),
            0,
            "Builder resources start at zero"
        );
    }

    builder.addResource(ResourceType::BRICK, 3);
    builder.addResource(ResourceType::ENERGY, 2);
    builder.addResource(ResourceType::GLASS, 2);
    builder.addResource(ResourceType::HEAT, 3);
    builder.addResource(ResourceType::WIFI, 1);
    builder.addResource(ResourceType::BRICK, 0);
    builder.addResource(ResourceType::BRICK, -2);

    suite.expectEqual(
        builder.getResource(ResourceType::BRICK),
        3,
        "Builder ignores non-positive additions"
    );
    suite.expect(builder.canAffordRoad(), "Builder can afford road");
    suite.expect(
        builder.canAffordResidence(ResidenceType::BASEMENT),
        "Builder can afford basement"
    );
    suite.expect(
        builder.canAffordResidence(ResidenceType::HOUSE),
        "Builder can afford house"
    );
    suite.expect(
        builder.canAffordResidence(ResidenceType::TOWER),
        "Builder can afford tower"
    );

    suite.expect(
        !builder.removeResource(ResourceType::BRICK, 4),
        "Cannot remove more resources than owned"
    );
    suite.expect(
        !builder.removeResource(ResourceType::BRICK, -1),
        "Cannot remove a negative amount"
    );
    suite.expect(
        builder.removeResource(ResourceType::BRICK, 3),
        "Can remove exact resource amount"
    );
    suite.expectEqual(
        builder.getResource(ResourceType::BRICK),
        0,
        "Resource removal mutates count"
    );
    suite.expect(
        !builder.removeResource(ResourceType::PARK, 0),
        "PARK is never an owned resource"
    );

    suite.expectEqual(builder.rollDice(), 2, "Builder uses initial dice");
    builder.setDice(twelve);
    suite.expectEqual(builder.rollDice(), 12, "Builder switches dice");

    suite.expectEqual(
        builder.buildingPoints(),
        0,
        "Builder starts with zero points"
    );
    builder.addBuildingPoints(-1);
    builder.addBuildingPoints(0);
    suite.expectEqual(
        builder.buildingPoints(),
        0,
        "Builder ignores non-positive points"
    );
    builder.addBuildingPoints(3);
    suite.expectEqual(
        builder.buildingPoints(),
        3,
        "Builder adds positive points"
    );

    std::srand(12345);
    FairDice fair;
    for (int i = 0; i < 1000; ++i) {
        int roll = fair.roll();
        suite.expect(
            roll >= 2 && roll <= 12,
            "Fair dice stays in range 2..12"
        );
    }

    std::istringstream loadedInput{"x\n1\n13\n7\n"};
    std::ostringstream loadedOutput;
    {
        StreamRedirect inputRedirect{
            std::cin,
            loadedInput.rdbuf()
        };
        StreamRedirect outputRedirect{
            std::cout,
            loadedOutput.rdbuf()
        };
        LoadedDice loaded;
        suite.expectEqual(
            loaded.roll(),
            7,
            "Loaded dice rejects invalid input and accepts 2..12"
        );
    }
    suite.expect(
        loadedOutput.str().find("Invalid roll.") !=
            std::string::npos,
        "Loaded dice reports invalid input"
    );

    std::istringstream eofInput{""};
    {
        StreamRedirect inputRedirect{std::cin, eofInput.rdbuf()};
        LoadedDice loaded;
        suite.expectEqual(
            loaded.roll(),
            -1,
            "Loaded dice returns -1 on EOF"
        );
    }
}

void testBoard(TestSuite &suite) {
    Board board;
    board.setupDefaultBoard();

    const std::vector<int> tileValues = {
        2, 12, 3, 3, 4, 4, 5, 5, 6, 6,
        8, 8, 9, 9, 10, 10, 11, 11, 0
    };
    const std::vector<ResourceType> tileTypes = {
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

    for (int tileId = 0; tileId < 19; ++tileId) {
        suite.expectEqual(
            board.getTile(tileId).getId(),
            tileId,
            "Default board tile ids are sequential"
        );
        suite.expectEqual(
            board.getTile(tileId).getNumber(),
            tileValues[tileId],
            "Default board tile values match layout"
        );
        suite.expect(
            board.getTile(tileId).getType() ==
                tileTypes[tileId],
            "Default board tile resources match layout"
        );
        suite.expectEqual(
            board.getVerticesForTile(tileId).size(),
            std::size_t{6},
            "Every tile has six vertices"
        );
    }

    for (int edgeId = 0; edgeId < 72; ++edgeId) {
        const std::vector<int> &ends =
            board.getVerticesForEdge(edgeId);
        suite.expectEqual(
            ends.size(),
            std::size_t{2},
            "Every edge has two endpoints"
        );
        suite.expect(
            ends[0] != ends[1],
            "Edge endpoints are distinct"
        );
    }

    for (int vertexId = 0; vertexId < 54; ++vertexId) {
        const std::vector<int> &edges =
            board.getEdgesForVertex(vertexId);
        const std::vector<int> &neighbours =
            board.getNeighboursForVertex(vertexId);

        suite.expect(
            edges.size() == 2 || edges.size() == 3,
            "Each vertex touches two or three edges"
        );
        suite.expectEqual(
            neighbours.size(),
            edges.size(),
            "Vertex neighbour and edge counts agree"
        );

        for (int edgeId : edges) {
            const std::vector<int> &ends =
                board.getVerticesForEdge(edgeId);
            suite.expect(
                ends[0] == vertexId ||
                    ends[1] == vertexId,
                "Vertex-edge reverse mapping is consistent"
            );
        }
    }

    suite.expectEqual(
        board.getGeeseTile(),
        18,
        "Geese start on PARK"
    );
    suite.expect(!board.canMoveGeeseTo(-1), "Reject negative geese tile");
    suite.expect(!board.canMoveGeeseTo(19), "Reject oversized geese tile");
    suite.expect(
        !board.canMoveGeeseTo(18),
        "Reject current geese tile"
    );
    suite.expect(board.canMoveGeeseTo(0), "Allow another geese tile");

    board.placeInitialResidence(0, Colour::BLUE);
    suite.expect(
        !board.canPlaceInitialResidence(0),
        "Cannot place twice on one vertex"
    );
    suite.expect(
        !board.canPlaceInitialResidence(1),
        "Residence distance rule blocks neighbour"
    );
    suite.expect(
        board.canPlaceInitialResidence(2),
        "Non-neighbouring initial vertex remains legal"
    );
    suite.expect(
        board.canPlaceInitialRoad(0, 0),
        "Initial road may touch chosen residence"
    );
    suite.expect(
        !board.canPlaceInitialRoad(3, 0),
        "Initial road must touch chosen residence"
    );

    board.placeInitialRoad(0, Colour::BLUE);
    suite.expect(
        !board.canPlaceInitialRoad(0, 0),
        "Initial road cannot occupy built edge"
    );
    suite.expect(
        board.canBuildRoad(1, Colour::BLUE),
        "Owner can extend from residence"
    );
    suite.expect(
        !board.canBuildRoad(1, Colour::RED),
        "Other builder cannot use opponent residence"
    );
    board.buildRoad(1, Colour::BLUE);
    suite.expect(
        board.canBuildRoad(6, Colour::BLUE),
        "Road chain may continue"
    );

    board.placeInitialRoad(6, Colour::BLUE);

    suite.expect(
        board.canBuildResidence(8, Colour::BLUE),
        "Road-connected legal vertex accepts residence"
    );
    suite.expect(
        !board.canBuildResidence(8, Colour::RED),
        "Opponent cannot build from another colour road"
    );
    board.buildResidence(8, Colour::BLUE);
    suite.expect(
        board.canUpgradeResidence(8, Colour::BLUE),
        "Owner may upgrade basement"
    );
    suite.expect(
        !board.canUpgradeResidence(8, Colour::RED),
        "Non-owner may not upgrade residence"
    );
    board.upgradeResidence(8);
    board.upgradeResidence(8);
    suite.expect(
        !board.canUpgradeResidence(8, Colour::BLUE),
        "Tower may not upgrade"
    );

    std::vector<Production> production =
        board.getProduction(2);
    int blueBrick = 0;
    int redBrick = 0;
    for (const Production &entry : production) {
        suite.expect(
            entry.type == ResourceType::BRICK,
            "Roll 2 produces BRICK"
        );
        if (entry.colour == Colour::BLUE) {
            blueBrick += entry.amount;
        } else if (entry.colour == Colour::RED) {
            redBrick += entry.amount;
        }
    }
    suite.expectEqual(
        blueBrick,
        4,
        "Blue basement and tower production totals four"
    );
    suite.expectEqual(redBrick, 0, "No Red residence produces on roll 2");

    Board blockedBoard;
    blockedBoard.setupDefaultBoard();
    blockedBoard.placeInitialResidence(0, Colour::BLUE);
    blockedBoard.placeInitialRoad(1, Colour::BLUE);
    blockedBoard.placeInitialResidence(3, Colour::RED);
    suite.expect(
        !blockedBoard.canBuildRoad(6, Colour::BLUE),
        "Opponent residence blocks road traversal"
    );

    board.moveGeeseTo(0);
    suite.expect(
        board.getProduction(2).empty(),
        "Geese suppress production"
    );
    board.moveGeeseTo(1);
    suite.expect(
        !board.getProduction(2).empty(),
        "Production resumes after geese leave"
    );

    board.setupDefaultBoard();
    suite.expectEqual(
        board.getGeeseTile(),
        18,
        "Default setup is repeatable and resets geese"
    );
    for (int vertexId = 0; vertexId < 54; ++vertexId) {
        suite.expect(
            !board.getVertex(vertexId).hasResidence(),
            "Default setup clears residences"
        );
    }
    for (int edgeId = 0; edgeId < 72; ++edgeId) {
        suite.expect(
            !board.getEdge(edgeId).isBuilt(),
            "Default setup clears roads"
        );
    }
}

void testBoardSetup(TestSuite &suite) {
    const std::string validFile = "test-layout-valid.tmp";
    const std::string shortFile = "test-layout-short.tmp";
    const std::string invalidFile = "test-layout-invalid.tmp";

    {
        std::ofstream out{validFile};
        for (int i = 0; i < 18; ++i) {
            out << (i % 5) << ' ' << (2 + i % 11) << ' ';
        }
        out << "5 7\n";
    }
    {
        std::ofstream out{shortFile};
        out << "0 2 1 3\n";
    }
    {
        std::ofstream out{invalidFile};
        for (int i = 0; i < 18; ++i) {
            out << "0 2 ";
        }
        out << "99 7\n";
    }

    Board fileBoard;
    fileBoard.initializeVertices(54);
    fileBoard.initializeEdges(72);
    FileBoardSetup valid{validFile};
    suite.expect(valid.configure(fileBoard), "Valid layout file loads");
    for (int i = 0; i < 18; ++i) {
        suite.expect(
            fileBoard.getTile(i).getType() ==
                static_cast<ResourceType>(i % 5),
            "File layout preserves resource order"
        );
        suite.expectEqual(
            fileBoard.getTile(i).getNumber(),
            2 + i % 11,
            "File layout preserves tile values"
        );
    }
    suite.expect(
        fileBoard.getTile(18).getType() == ResourceType::PARK,
        "File layout accepts PARK"
    );
    suite.expectEqual(
        fileBoard.getVerticesForEdge(0).size(),
        std::size_t{2},
        "File layout builds Figure 3 edge topology"
    );
    suite.expectEqual(
        fileBoard.getVerticesForTile(0).size(),
        std::size_t{6},
        "File layout builds Figure 3 tile topology"
    );

    Board missingBoard;
    FileBoardSetup missing{"does-not-exist.layout"};
    suite.expect(
        !missing.configure(missingBoard),
        "Missing layout file is rejected"
    );

    Board shortBoard;
    FileBoardSetup shortSetup{shortFile};
    suite.expect(
        !shortSetup.configure(shortBoard),
        "Short layout file is rejected"
    );

    Board invalidBoard;
    FileBoardSetup invalid{invalidFile};
    suite.expect(
        !invalid.configure(invalidBoard),
        "Invalid resource code is rejected"
    );

    std::srand(7);
    Board randomBoard;
    randomBoard.initializeVertices(54);
    randomBoard.initializeEdges(72);
    RandomBoardSetup random;
    suite.expect(
        random.configure(randomBoard),
        "Random layout configures successfully"
    );

    int resourceCounts[6] = {0, 0, 0, 0, 0, 0};
    int valueCounts[13] = {};
    int parkId = -1;
    for (int i = 0; i < 19; ++i) {
        ResourceType type = randomBoard.getTile(i).getType();
        int value = randomBoard.getTile(i).getNumber();
        ++resourceCounts[static_cast<int>(type)];
        if (value >= 0 && value <= 12) {
            ++valueCounts[value];
        }
        if (type == ResourceType::PARK) {
            parkId = i;
        }
    }

    suite.expectEqual(resourceCounts[0], 4, "Random board has 4 BRICK");
    suite.expectEqual(resourceCounts[1], 4, "Random board has 4 ENERGY");
    suite.expectEqual(resourceCounts[2], 4, "Random board has 4 GLASS");
    suite.expectEqual(resourceCounts[3], 3, "Random board has 3 HEAT");
    suite.expectEqual(resourceCounts[4], 3, "Random board has 3 WIFI");
    suite.expectEqual(resourceCounts[5], 1, "Random board has 1 PARK");
    suite.expectEqual(valueCounts[2], 1, "Random board has one 2");
    suite.expectEqual(valueCounts[12], 1, "Random board has one 12");
    suite.expectEqual(valueCounts[7], 1, "Random board has one 7");
    for (int value = 3; value <= 11; ++value) {
        if (value != 7) {
            suite.expectEqual(
                valueCounts[value],
                2,
                "Random board duplicates standard values"
            );
        }
    }
    suite.expectEqual(
        randomBoard.getTile(parkId).getNumber(),
        7,
        "PARK stays paired with value 7"
    );
    suite.expectEqual(
        randomBoard.getVerticesForEdge(71).size(),
        std::size_t{2},
        "Random layout builds Figure 3 edge topology"
    );
    suite.expectEqual(
        randomBoard.getVerticesForTile(18).size(),
        std::size_t{6},
        "Random layout builds Figure 3 tile topology"
    );

    randomBoard.placeInitialResidence(0, Colour::BLUE);
    randomBoard.placeInitialRoad(0, Colour::BLUE);
    suite.expect(
        random.configure(randomBoard),
        "Random layout can reset an existing game"
    );
    suite.expect(
        !randomBoard.getVertex(0).hasResidence(),
        "Board reconfiguration clears old residences"
    );
    suite.expect(
        !randomBoard.getEdge(0).isBuilt(),
        "Board reconfiguration clears old roads"
    );

    std::remove(validFile.c_str());
    std::remove(shortFile.c_str());
    std::remove(invalidFile.c_str());
}

void testDisplay(TestSuite &suite) {
    Board board;
    board.setupDefaultBoard();
    board.placeInitialResidence(0, Colour::BLUE);
    board.placeInitialRoad(0, Colour::BLUE);
    board.moveGeeseTo(0);

    TextDisplay display{board};
    std::string formal = display.renderFormal();
    std::string debug = display.renderDebug();
    std::string normal = display.render();

    suite.expect(!formal.empty(), "Formal display is non-empty");
    suite.expect(!debug.empty(), "Debug display is non-empty");
    suite.expect(
        normal == formal,
        "Default rendering uses formal display"
    );
    suite.expect(
        formal.find("BRICK") != std::string::npos,
        "Formal display names resources"
    );
    suite.expect(
        formal.find("GEESE") != std::string::npos,
        "Formal display shows geese"
    );
    suite.expect(
        formal.find("BR") != std::string::npos,
        "Formal display shows Blue road"
    );
    suite.expect(
        formal.find("BB") != std::string::npos,
        "Formal display shows Blue basement"
    );
    suite.expect(
        debug.find("Tiles") != std::string::npos &&
            debug.find("Vertices") != std::string::npos &&
            debug.find("Edges") != std::string::npos,
        "Debug display contains all sections"
    );
    suite.expect(
        debug.find("geese=yes") != std::string::npos,
        "Debug display reports geese state"
    );

    std::ostringstream output;
    output << display;
    suite.expect(
        output.str() == formal,
        "Stream operator uses rendered board"
    );

    for (int tileId = 0; tileId < 19; ++tileId) {
        Board geeseBoard;
        geeseBoard.setupDefaultBoard();
        geeseBoard.moveGeeseTo(tileId);

        std::string geeseDisplay =
            TextDisplay{geeseBoard}.renderFormal();
        std::size_t first = geeseDisplay.find("GEESE");

        suite.expect(
            first != std::string::npos,
            "Formal display shows Geese on every tile"
        );
        suite.expect(
            first != std::string::npos &&
                geeseDisplay.find("GEESE", first + 1) ==
                    std::string::npos,
            "Formal display shows Geese exactly once"
        );
    }

    Board pristineBoard;
    pristineBoard.setupDefaultBoard();
    std::string pristineFormal =
        TextDisplay{pristineBoard}.renderFormal();

    suite.expect(
        pristineFormal.find("| 0|-- 0--| 1|") !=
            std::string::npos,
        "Formal display matches Figure 3 top edge"
    );
    suite.expect(
        pristineFormal.find("|52|--71--|53|") !=
            std::string::npos,
        "Formal display matches Figure 3 bottom edge"
    );

    Board dynamicBoard;
    dynamicBoard.initializeVertices(54);
    dynamicBoard.initializeEdges(72);
    for (int tileId = 0; tileId < 19; ++tileId) {
        ResourceType type = ResourceType::GLASS;

        if (tileId == 0) {
            type = ResourceType::WIFI;
        } else if (tileId == 4) {
            type = ResourceType::BRICK;
        } else if (tileId == 5) {
            type = ResourceType::ENERGY;
        } else if (tileId == 18) {
            type = ResourceType::PARK;
        }

        dynamicBoard.addTile(type, tileId + 2);
    }
    dynamicBoard.getTile(5).placeGeese();

    std::istringstream dynamicInput{
        TextDisplay{dynamicBoard}.renderFormal()
    };
    std::vector<std::string> dynamicLines;
    std::string dynamicLine;
    while (std::getline(dynamicInput, dynamicLine)) {
        dynamicLines.push_back(dynamicLine);
    }

    suite.expect(
        dynamicLines.size() == 41,
        "Formal display keeps the Figure 3 height"
    );
    suite.expect(
        dynamicLines[3].find("WIFI") != std::string::npos,
        "Formal display reads Tile 0 resource dynamically"
    );
    suite.expect(
        dynamicLines[11].find("BRICK") != std::string::npos &&
            dynamicLines[11].find("ENERGY") != std::string::npos,
        "Formal display reads Tiles 4 and 5 dynamically"
    );
    suite.expect(
        dynamicLines[13].find("GEESE") != std::string::npos,
        "Formal display reads Tile 5 Geese dynamically"
    );
}

void testGameStateIO(TestSuite &suite) {
    FixedDice dice{6};
    Builder blue{Colour::BLUE, dice};
    Builder red{Colour::RED, dice};
    Builder orange{Colour::ORANGE, dice};
    Builder yellow{Colour::YELLOW, dice};
    Builder *builders[4] = {&blue, &red, &orange, &yellow};

    Board board;
    board.setupDefaultBoard();
    board.placeInitialResidence(0, Colour::BLUE);
    board.placeInitialRoad(0, Colour::BLUE);
    board.placeInitialResidence(2, Colour::RED);
    board.placeInitialRoad(3, Colour::RED);
    board.upgradeResidence(2);
    board.moveGeeseTo(5);

    blue.addResource(ResourceType::BRICK, 1);
    blue.addResource(ResourceType::ENERGY, 2);
    red.addResource(ResourceType::GLASS, 3);
    orange.addResource(ResourceType::HEAT, 4);
    yellow.addResource(ResourceType::WIFI, 5);
    blue.addBuildingPoints(1);
    red.addBuildingPoints(2);

    int currentTurn = 2;
    GameStateIO state{board, builders, currentTurn};
    const std::string saveFile = "test-state.sv";
    suite.expect(state.save(saveFile), "Game state saves");

    std::ifstream saved{saveFile};
    std::string firstLine;
    std::getline(saved, firstLine);
    saved.close();
    suite.expectEqual(
        firstLine,
        std::string{"3"},
        "Save records the next builder"
    );

    FixedDice loadedDice{8};
    Builder loadedBlue{Colour::BLUE, loadedDice};
    Builder loadedRed{Colour::RED, loadedDice};
    Builder loadedOrange{Colour::ORANGE, loadedDice};
    Builder loadedYellow{Colour::YELLOW, loadedDice};
    Builder *loadedBuilders[4] = {
        &loadedBlue, &loadedRed, &loadedOrange, &loadedYellow
    };
    Board loadedBoard;
    int loadedTurn = -1;
    GameStateIO loaded{
        loadedBoard,
        loadedBuilders,
        loadedTurn
    };

    suite.expect(loaded.load(saveFile), "Game state loads");
    suite.expectEqual(loadedTurn, 3, "Load restores saved turn");
    suite.expectEqual(
        loadedBlue.getResource(ResourceType::BRICK),
        1,
        "Load restores Blue resources"
    );
    suite.expectEqual(
        loadedRed.getResource(ResourceType::GLASS),
        3,
        "Load restores Red resources"
    );
    suite.expectEqual(
        loadedOrange.getResource(ResourceType::HEAT),
        4,
        "Load restores Orange resources"
    );
    suite.expectEqual(
        loadedYellow.getResource(ResourceType::WIFI),
        5,
        "Load restores Yellow resources"
    );
    suite.expect(
        loadedBoard.getEdge(0).isBuilt() &&
            loadedBoard.getEdge(0).getOwner() == Colour::BLUE,
        "Load restores Blue road"
    );
    suite.expect(
        loadedBoard.getVertex(2).hasResidence() &&
            loadedBoard.getVertex(2).getResidence()->getType() ==
                ResidenceType::HOUSE,
        "Load restores upgraded residence"
    );
    suite.expectEqual(
        loadedBoard.getGeeseTile(),
        5,
        "Load restores geese"
    );
    suite.expectEqual(
        loadedBlue.buildingPoints(),
        1,
        "Load reconstructs basement points"
    );
    suite.expectEqual(
        loadedRed.buildingPoints(),
        2,
        "Load reconstructs house points"
    );

    suite.expectEqual(
        loadedBoard.getVerticesForEdge(0).size(),
        std::size_t{2},
        "Load restores board edge topology"
    );
    suite.expectEqual(
        loadedBoard.getVerticesForTile(0).size(),
        std::size_t{6},
        "Load restores board tile topology"
    );

    Board badBoard;
    Builder badBlue{Colour::BLUE, dice};
    Builder badRed{Colour::RED, dice};
    Builder badOrange{Colour::ORANGE, dice};
    Builder badYellow{Colour::YELLOW, dice};
    Builder *badBuilders[4] = {
        &badBlue, &badRed, &badOrange, &badYellow
    };
    int badTurn = 0;
    GameStateIO badState{badBoard, badBuilders, badTurn};
    suite.expect(
        !badState.load("missing-state.sv"),
        "Missing save file is rejected"
    );

    const std::string malformedFile = "test-state-malformed.sv";
    {
        std::ofstream malformed{malformedFile};
        malformed << "not a game state\n";
    }
    suite.expect(
        !badState.load(malformedFile),
        "Malformed save file is rejected"
    );

    std::remove(saveFile.c_str());
    std::remove(malformedFile.c_str());
}

void testGameController(TestSuite &suite) {
    GameController controller;
    suite.expect(!controller.hasWinner(), "New controller has no winner");

    FixedDice dice{6};
    Builder other{Colour::RED, dice};
    other.addResource(ResourceType::ENERGY, 1);
    suite.expect(
        !controller.handleTrade(
            other,
            ResourceType::BRICK,
            ResourceType::ENERGY
        ),
        "Trade fails when current builder lacks offered resource"
    );

    std::ostringstream beforeRoll;
    {
        StreamRedirect outputRedirect{
            std::cout,
            beforeRoll.rdbuf()
        };
        controller.processCommand("status");
    }
    suite.expect(
        beforeRoll.str().find("Invalid command.") !=
            std::string::npos,
        "During-turn commands are rejected before rolling"
    );

    std::istringstream rollInput{"6\n"};
    std::ostringstream rollOutput;
    {
        StreamRedirect inputRedirect{
            std::cin,
            rollInput.rdbuf()
        };
        StreamRedirect outputRedirect{
            std::cout,
            rollOutput.rdbuf()
        };
        controller.processCommand("roll");
    }
    suite.expect(
        rollOutput.str().find("You rolled 6.") !=
            std::string::npos,
        "Controller rolls loaded dice"
    );

    std::ostringstream statusOutput;
    {
        StreamRedirect outputRedirect{
            std::cout,
            statusOutput.rdbuf()
        };
        controller.processCommand("status");
    }
    suite.expect(
        statusOutput.str().find("Blue has 0 building points") !=
            std::string::npos,
        "Status prints Blue"
    );
    suite.expect(
        statusOutput.str().find("Yellow has 0 building points") !=
            std::string::npos,
        "Status prints Yellow"
    );

    std::ostringstream invalidOutput;
    {
        StreamRedirect outputRedirect{
            std::cout,
            invalidOutput.rdbuf()
        };
        controller.processCommand("unknown");
        controller.processCommand("build-road");
        controller.processCommand("build-road 72");
        controller.processCommand("trade Purple BRICK WIFI");
    }
    suite.expect(
        invalidOutput.str().find("Invalid command.") !=
            std::string::npos,
        "Controller reports malformed commands"
    );
    suite.expect(
        invalidOutput.str().find("You cannot build here.") !=
            std::string::npos,
        "Controller rejects out-of-range building location"
    );

    std::ostringstream helpOutput;
    {
        StreamRedirect outputRedirect{
            std::cout,
            helpOutput.rdbuf()
        };
        controller.processCommand("help");
    }
    suite.expect(
        helpOutput.str().find("build-road <edge#>") !=
            std::string::npos &&
            helpOutput.str().find("save <file>") !=
                std::string::npos,
        "Help lists commands"
    );

    controller.nextTurn();
    std::ostringstream nextPhase;
    {
        StreamRedirect outputRedirect{
            std::cout,
            nextPhase.rdbuf()
        };
        controller.processCommand("status");
    }
    suite.expect(
        nextPhase.str().find("Invalid command.") !=
            std::string::npos,
        "Next turn returns to pre-roll phase"
    );
}

} // namespace

int main() {
    TestSuite suite;

    testValueObjects(suite);
    testBuilderAndDice(suite);
    testBoard(suite);
    testBoardSetup(suite);
    testDisplay(suite);
    testGameStateIO(suite);
    testGameController(suite);

    return suite.finish();
}
