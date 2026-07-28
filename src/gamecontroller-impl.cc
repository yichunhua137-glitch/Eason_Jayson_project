module;

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

module gamecontroller;

import residence;
import textdisplay;

using namespace std;

namespace {

string colourName(Colour colour) {
    if (colour == Colour::BLUE) {
        return "Blue";
    } else if (colour == Colour::RED) {
        return "Red";
    } else if (colour == Colour::ORANGE) {
        return "Orange";
    }

    return "Yellow";
}

string resourceName(ResourceType type) {
    if (type == ResourceType::BRICK) {
        return "BRICK";
    } else if (type == ResourceType::ENERGY) {
        return "ENERGY";
    } else if (type == ResourceType::GLASS) {
        return "GLASS";
    } else if (type == ResourceType::HEAT) {
        return "HEAT";
    } else if (type == ResourceType::WIFI) {
        return "WIFI";
    }

    return "PARK";
}

bool parseSingleInteger(const string &line, int &value) {
    istringstream input{line};
    string extra;

    return (input >> value) && !(input >> extra);
}

} // namespace


GameController::GameController()
    : board{},
      fairDice{},
      loadedDice{},
      blueBuilder{Colour::BLUE, loadedDice},
      redBuilder{Colour::RED, loadedDice},
      orangeBuilder{Colour::ORANGE, loadedDice},
      yellowBuilder{Colour::YELLOW, loadedDice},
      builders{
          &blueBuilder,
          &redBuilder,
          &orangeBuilder,
          &yellowBuilder
      },
      currentTurn{0},
      gameRunning{false},
      hasRolled{false},
      gameStateIO{
          board,
          builders,
          currentTurn
      }
{
}

Builder &GameController::getCurrentBuilder() {
    return *builders[currentTurn];
}

void GameController::nextTurn() {
    currentTurn = (currentTurn + 1) % 4;
    hasRolled = false;
}

bool GameController::hasWinner() const {
    int i = 0;

    while (i < 4) {
        if (builders[i]->buildingPoints() >= 10) {
            return true;
        }

        ++i;
    }

    return false;
}


bool GameController::handleTrade( Builder &other, ResourceType give, ResourceType take){
    Builder &current = getCurrentBuilder();

    if (current.getResource(give) >= 1 &&
        other.getResource(take) >= 1) {

        current.removeResource(give, 1);
        other.addResource(give, 1);

        other.removeResource(take, 1);
        current.addResource(take, 1);

        return true;
    }

    return false;
}

void GameController::grantInitialResources(
    Builder &builder,
    int vertexId
) {
    const vector<int> &tileIds =
        board.getTilesForVertex(vertexId);

    for (int tileId : tileIds) {
        ResourceType type =
            board.getTile(tileId).getType();

        if (type != ResourceType::PARK) {
            builder.addResource(type, 1);
        }
    }
}

bool GameController::performInitialPlacement() {
    const int placementOrder[8] = {
        0, 1, 2, 3, 3, 2, 1, 0
    };

    for (int placement = 0;
         placement < 8;
         ++placement) {
        int builderIndex = placementOrder[placement];
        Builder &builder = *builders[builderIndex];

        while (true) {
            cout << "Builder "
                 << colourName(builder.getColour())
                 << ", where do you want to build a basement?"
                 << endl;
            cout << "> ";

            string line;
            if (!getline(cin, line)) {
                return false;
            }

            int vertexId;
            if (
                !parseSingleInteger(line, vertexId) ||
                vertexId < 0 ||
                vertexId >= 54 ||
                !board.canPlaceInitialResidence(vertexId)
            ) {
                cout << "You cannot build here." << endl;
                continue;
            }

            board.placeInitialResidence(
                vertexId,
                builder.getColour()
            );
            builder.addBuildingPoints(1);

            if (placement >= 4) {
                grantInitialResources(builder, vertexId);
            }

            break;
        }
    }

    cout << TextDisplay{board};
    return true;
}

void GameController::discardResourcesForGeese() {
    for (int builderIndex = 0;
         builderIndex < 4;
         ++builderIndex) {
        Builder &builder = *builders[builderIndex];
        int total = builder.totalResources();

        if (total < 10) {
            continue;
        }

        int lossCount = total / 2;
        int lost[5] = {};

        for (int i = 0; i < lossCount; ++i) {
            ResourceType lostType =
                builder.removeRandomResource();
            int resourceIndex =
                static_cast<int>(lostType);

            if (
                resourceIndex >= 0 &&
                resourceIndex < 5
            ) {
                ++lost[resourceIndex];
            }
        }

        cout << "Builder "
             << colourName(builder.getColour())
             << " loses "
             << lossCount
             << " resources to the geese. They lose:"
             << endl;

        for (int resourceIndex = 0;
             resourceIndex < 5;
             ++resourceIndex) {
            if (lost[resourceIndex] > 0) {
                cout << lost[resourceIndex]
                     << " "
                     << resourceName(
                            static_cast<ResourceType>(
                                resourceIndex
                            )
                        )
                     << endl;
            }
        }
    }
}

vector<Builder *> GameController::getStealableBuilders(
    int tileId
) {
    bool present[4] = {false, false, false, false};
    Builder &current = getCurrentBuilder();

    for (int vertexId :
         board.getVerticesForTile(tileId)) {
        Residence *residence =
            board.getVertex(vertexId).getResidence();

        if (residence == nullptr) {
            continue;
        }

        for (int builderIndex = 0;
             builderIndex < 4;
             ++builderIndex) {
            Builder *candidate = builders[builderIndex];

            if (
                candidate != &current &&
                candidate->getColour() ==
                    residence->getOwner() &&
                candidate->totalResources() > 0
            ) {
                present[builderIndex] = true;
            }
        }
    }

    vector<Builder *> result;
    for (int builderIndex = 0;
         builderIndex < 4;
         ++builderIndex) {
        if (present[builderIndex]) {
            result.push_back(builders[builderIndex]);
        }
    }

    return result;
}

bool GameController::handleGeese() {
    discardResourcesForGeese();

    int tileId = -1;
    while (true) {
        cout << "Choose where to place the GEESE."
             << endl;
        cout << "> ";

        string line;
        if (!getline(cin, line)) {
            gameStateIO.save("backup.sv");
            gameRunning = false;
            return false;
        }

        if (
            parseSingleInteger(line, tileId) &&
            board.canMoveGeeseTo(tileId)
        ) {
            break;
        }

        cout << "Invalid command." << endl;
    }

    board.moveGeeseTo(tileId);

    vector<Builder *> candidates =
        getStealableBuilders(tileId);
    Builder &current = getCurrentBuilder();
    string currentName =
        colourName(current.getColour());

    if (candidates.empty()) {
        cout << "Builder "
             << currentName
             << " has no builders to steal from."
             << endl;
        return true;
    }

    cout << "Builder "
         << currentName
         << " can choose to steal from ";

    for (int i = 0;
         i < static_cast<int>(candidates.size());
         ++i) {
        if (i > 0) {
            cout << ", ";
        }
        cout << colourName(candidates[i]->getColour());
    }
    cout << "." << endl;

    Builder *victim = nullptr;
    while (victim == nullptr) {
        cout << "Choose a builder to steal from."
             << endl;
        cout << "> ";

        string chosenColour;
        if (!getline(cin, chosenColour)) {
            gameStateIO.save("backup.sv");
            gameRunning = false;
            return false;
        }

        for (Builder *candidate : candidates) {
            if (
                chosenColour ==
                colourName(candidate->getColour())
            ) {
                victim = candidate;
                break;
            }
        }

        if (victim == nullptr) {
            cout << "Invalid command." << endl;
        }
    }

    ResourceType stolen =
        victim->removeRandomResource();
    current.addResource(stolen, 1);

    cout << "Builder "
         << currentName
         << " steals "
         << resourceName(stolen)
         << " from builder "
         << colourName(victim->getColour())
         << "."
         << endl;

    return true;
}

void GameController::startNewGame() {
    currentTurn = 0;
    gameRunning = true;
    hasRolled = false;

    board.setupTopology();

    int parkId = -1;
    for (int tileId = 0; tileId < 19; ++tileId) {
        if (
            board.getTile(tileId).getType() ==
            ResourceType::PARK
        ) {
            parkId = tileId;
            break;
        }
    }

    if (
        parkId != -1 &&
        board.getGeeseTile() != parkId
    ) {
        board.moveGeeseTo(parkId);
    }

    for (int i = 0; i < 4; ++i) {
        builders[i]->reset();
        builders[i]->setDice(loadedDice);
    }

    if (!performInitialPlacement()) {
        gameStateIO.save("backup.sv");
        gameRunning = false;
    }
}


void GameController::processCommand(const std::string &command) {
    istringstream input{command};

    string action;
    input >> action;

    Builder &current = getCurrentBuilder();
    string extra;

    bool takesNoArguments =
        action == "load" ||
        action == "fair" ||
        action == "roll" ||
        action == "next" ||
        action == "board" ||
        action == "status" ||
        action == "residences" ||
        action == "help";

    if (takesNoArguments && (input >> extra)) {
        cout << "Invalid command." << endl;
        return;
    }

    if (!hasRolled &&
    action != "load" &&
    action != "fair" &&
    action != "roll") {
    cout << "Invalid command." << endl;
    return;
}

    if (hasRolled &&
    (action == "load" ||
     action == "fair" ||
     action == "roll")) {
    cout << "Invalid command." << endl;
    return;
}

    if (action == "load") {
        current.setDice(loadedDice);

    } else if (action == "fair") {
        current.setDice(fairDice);

    } else if (action == "next") {
        nextTurn();

    } else if (action == "roll") {
        int result = current.rollDice();

        if (result == -1) {
        gameStateIO.save("backup.sv");
        gameRunning = false;
        return;
    }
        cout << "You rolled " << result << "." << endl;

        if (result == 7) {
            if (!handleGeese()) {
                return;
            }
        } else {
            distributeResources(result);
        }
        hasRolled = true;

    } else if (action == "save") {
        string filename;

        if (!(input >> filename) ||
            (input >> extra)) {
            cout << "Invalid command." << endl;
            return;
        }

        if (!gameStateIO.save(filename)) {
            cerr << "Unable to save game." << endl;
            return;
        }

        nextTurn();
    } else if (action == "help") {
        cout << "Valid commands:\n"
             << "board\n"
             << "status\n"
             << "residences\n"
             << "build-road <edge#>\n"
             << "build-res <housing#>\n"
             << "improve <housing#>\n"
             << "trade <colour> <give> <take>\n"
             << "next\n"
             << "save <file>\n"
             << "help"
             << endl;

    } else if (action == "build-road") {
        int edgeId;

        if (!(input >> edgeId) ||
            (input >> extra)) {
            cout << "Invalid command." << endl;
        } else if (edgeId < 0 || edgeId >= 72) {
            cout << "You cannot build here." << endl;
        } else if (!current.canAffordRoad()) {
            cout << "You do not have enough resources." << endl;
        } else if (!board.canBuildRoad(
                       edgeId,
                       current.getColour()
                   )) {
            cout << "You cannot build here." << endl;
        } else {
            current.removeResource(ResourceType::HEAT, 1);
            current.removeResource(ResourceType::WIFI, 1);
            board.buildRoad(edgeId, current.getColour());
        }

    } else if (action == "build-res") {
        int vertexId;

        if (!(input >> vertexId) ||
            (input >> extra)) {
            cout << "Invalid command." << endl;
        } else if (vertexId < 0 || vertexId >= 54) {
            cout << "You cannot build here." << endl;
        } else if (!current.canAffordResidence(
                       ResidenceType::BASEMENT
                   )) {
            cout << "You do not have enough resources." << endl;
        } else if (!board.canBuildResidence(
                       vertexId,
                       current.getColour()
                   )) {
            cout << "You cannot build here." << endl;
        } else {
            current.removeResource(ResourceType::BRICK, 1);
            current.removeResource(ResourceType::ENERGY, 1);
            current.removeResource(ResourceType::GLASS, 1);
            current.removeResource(ResourceType::WIFI, 1);

            board.buildResidence(
                vertexId,
                current.getColour()
            );

            current.addBuildingPoints(1);
        }

    } else if (action == "improve") {
        int vertexId;

        if (!(input >> vertexId) ||
            (input >> extra)) {
            cout << "Invalid command." << endl;
        } else if (vertexId < 0 || vertexId >= 54) {
            cout << "You cannot build here." << endl;
        } else if (!board.canUpgradeResidence(
                       vertexId,
                       current.getColour()
                   )) {
            cout << "You cannot build here." << endl;
        } else {
            Residence *residence =
                board.getVertex(vertexId).getResidence();

            ResidenceType currentType =
                residence->getType();

            ResidenceType newType;

            if (currentType == ResidenceType::BASEMENT) {
                newType = ResidenceType::HOUSE;
            } else if (currentType == ResidenceType::HOUSE) {
                newType = ResidenceType::TOWER;
            } else {
                cout << "You cannot build here." << endl;
                return;
            }

            if (!current.canAffordResidence(newType)) {
                cout << "You do not have enough resources."
                     << endl;
                return;
            }

            if (newType == ResidenceType::HOUSE) {
                current.removeResource(
                    ResourceType::GLASS,
                    2
                );
                current.removeResource(
                    ResourceType::HEAT,
                    3
                );
            } else {
                current.removeResource(
                    ResourceType::BRICK,
                    3
                );
                current.removeResource(
                    ResourceType::ENERGY,
                    2
                );
                current.removeResource(
                    ResourceType::GLASS,
                    2
                );
                current.removeResource(
                    ResourceType::WIFI,
                    1
                );
                current.removeResource(
                    ResourceType::HEAT,
                    2
                );
            }

            board.upgradeResidence(vertexId);
            current.addBuildingPoints(1);
        }

    } else if (action == "status") {
        for (int i = 0; i < 4; ++i) {
            printBuilderStatus(*builders[i]);
        }

    } else if (action == "residences") {
        cout << colourName(current.getColour())
             << " has built:" << endl;

        for (int vertexId = 0;
            vertexId < 54;
            ++vertexId) {

        Residence *residence = board.getVertex(vertexId).getResidence();

        if (residence != nullptr &&
            residence->getOwner() ==
                current.getColour()) {

            char buildingType;

            if (residence->getType() ==
                ResidenceType::BASEMENT) {
                buildingType = 'B';
            } else if (residence->getType() ==
                       ResidenceType::HOUSE) {
                buildingType = 'H';
            } else {
                buildingType = 'T';
            }

            cout << vertexId
                 << " "
                 << buildingType
                 << endl;
        }
    }
    } else if (action == "board") {
        cout << TextDisplay{board};

    }else if (action == "trade") {
        string colourText;
        string giveText;
        string takeText;

        if (!(input >> colourText
                    >> giveText
                    >> takeText) ||
            (input >> extra)) {
            cout << "Invalid command." << endl;
            return;
        }

        Builder *other = nullptr;

        if (colourText == "Blue") {
            other = builders[0];
        } else if (colourText == "Red") {
            other = builders[1];
        } else if (colourText == "Orange") {
            other = builders[2];
        } else if (colourText == "Yellow") {
            other = builders[3];
        }

        ResourceType give = ResourceType::PARK;
        ResourceType take = ResourceType::PARK;

        if (giveText == "BRICK") {
            give = ResourceType::BRICK;
        } else if (giveText == "ENERGY") {
            give = ResourceType::ENERGY;
        } else if (giveText == "GLASS") {
            give = ResourceType::GLASS;
        } else if (giveText == "HEAT") {
            give = ResourceType::HEAT;
        } else if (giveText == "WIFI") {
            give = ResourceType::WIFI;
        }

        if (takeText == "BRICK") {
            take = ResourceType::BRICK;
        } else if (takeText == "ENERGY") {
            take = ResourceType::ENERGY;
        } else if (takeText == "GLASS") {
            take = ResourceType::GLASS;
        } else if (takeText == "HEAT") {
            take = ResourceType::HEAT;
        } else if (takeText == "WIFI") {
            take = ResourceType::WIFI;
        }

        if (other == nullptr ||
            other == &current ||
            give == ResourceType::PARK ||
            take == ResourceType::PARK) {

            cout << "Invalid command." << endl;
            return;
        }

        if (current.getResource(give) < 1 ||
            other->getResource(take) < 1) {

            cout << "You do not have enough resources."
                 << endl;
            return;
        }

        string currentColour = colourName(current.getColour());

        cout << currentColour
             << " offers "
             << colourText
             << " one "
             << giveText
             << " for one "
             << takeText
             << "."
             << endl;

        cout << "Does "
             << colourText
             << " accept this offer?"
             << endl;

        cout << "> ";

        string response;

        if (!getline(cin, response)) {
            gameStateIO.save("backup.sv");
            gameRunning = false;
            return;
        }

        if (response == "yes") {
            handleTrade(*other, give, take);
        } else if (response != "no") {
            cout << "Invalid command." << endl;
        }

    } else {
        cout << "Invalid command." << endl;
    }
}

void GameController::run() {
    if (!gameRunning) {
        return;
    }

    bool showTurnStart = true;

    while (gameRunning) {
        if (hasWinner()) {
            cout << "Would you like to play again?"
                 << endl;
            cout << "> ";

            string response;

            if (!getline(cin, response)) {
                gameStateIO.save("backup.sv");
                gameRunning = false;
                return;
            }

            if (response == "yes") {
                startNewGame();
                showTurnStart = true;
                continue;
            }

            if (response == "no") {
                gameRunning = false;
                return;
            }

            cout << "Invalid command." << endl;
            continue;
        }

        if (showTurnStart) {
            printTurnStart();
            showTurnStart = false;
        }

        cout << "> ";

        string command;

        if (!getline(cin, command)) {
            gameStateIO.save("backup.sv");
            gameRunning = false;
            return;
        }

        int previousTurn = currentTurn;

        processCommand(command);

        if (currentTurn != previousTurn) {
            showTurnStart = true;
        }
    }
}

bool GameController::setupBoard(BoardSetupStrategy &strategy) {
    return strategy.configure(board);
}

bool GameController::loadGame(const string &filename) {
    if (!gameStateIO.load(filename)) {
        return false;
    }

    gameRunning = true;
    hasRolled = false;

    for (int i = 0; i < 4; ++i) {
        builders[i]->setDice(loadedDice);
    }

    return true;
}

void GameController::distributeResources(int roll) {
    int gained[4][5] = {};

    auto production =
        board.getProduction(roll);

    for (const Production &record :
         production) {

        int builderIndex = -1;

        for (int i = 0; i < 4; ++i) {
            if (builders[i]->getColour() ==
                record.colour) {

                builderIndex = i;
                break;
            }
        }

        int resourceIndex =
            static_cast<int>(record.type);

        if (builderIndex != -1 &&
            resourceIndex >= 0 &&
            resourceIndex < 5) {

            builders[builderIndex]
                ->addResource(
                    record.type,
                    record.amount
                );

            gained[builderIndex]
                  [resourceIndex] +=
                record.amount;
        }
    }


    string resourceNames[5] = {
        "BRICK",
        "ENERGY",
        "GLASS",
        "HEAT",
        "WIFI"
    };

    bool anyoneGained = false;

    for (int builderIndex = 0;
         builderIndex < 4;
         ++builderIndex) {

        bool builderGained = false;

        for (int resourceIndex = 0;
             resourceIndex < 5;
             ++resourceIndex) {

            if (gained[builderIndex]
                      [resourceIndex] > 0) {

                builderGained = true;
                break;
            }
        }

        if (!builderGained) {
            continue;
        }

        anyoneGained = true;

        cout << "Builder "
            << colourName( builders[builderIndex]->getColour())
            << " gained:"
            << endl;

        for (int resourceIndex = 0;
             resourceIndex < 5;
             ++resourceIndex) {

            int amount =
                gained[builderIndex]
                      [resourceIndex];

            if (amount > 0) {
                cout << amount
                     << " "
                     << resourceNames[
                            resourceIndex
                        ]
                     << endl;
            }
        }
    }

    if (!anyoneGained) {
        cout << "No builders gained resources."
             << endl;
    }
}

void GameController::printBuilderStatus(
    const Builder &builder
) const {
    cout << colourName(builder.getColour())
         << " has "
         << builder.buildingPoints()
         << " building points, "
         << builder.getResource(ResourceType::BRICK)
         << " brick, "
         << builder.getResource(ResourceType::ENERGY)
         << " energy, "
         << builder.getResource(ResourceType::GLASS)
         << " glass, "
         << builder.getResource(ResourceType::HEAT)
         << " heat, and "
         << builder.getResource(ResourceType::WIFI)
         << " WiFi."
         << endl;
}

void GameController::printTurnStart() const {
    cout << TextDisplay{board};

    cout << "Builder "
         << colourName(builders[currentTurn]->getColour())
         << "'s turn."
         << endl;

    printBuilderStatus(*builders[currentTurn]);
}
