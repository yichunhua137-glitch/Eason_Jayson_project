module;

#include <iostream>
#include <sstream>
#include <string>

module gamecontroller;

import residence;

using namespace std;


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

void GameController::startNewGame() {
    currentTurn = 0;
    gameRunning = true;
    hasRolled = false;

    for (int i = 0; i < 4; ++i) {
        builders[i]->setDice(loadedDice);
    }
}


void GameController::processCommand(const std::string &command) {
    istringstream input{command};

    string action;
    input >> action;

    Builder &current = getCurrentBuilder();

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
        hasRolled = true;

    } else if (action == "save") {
        string filename;

        if (!(input >> filename)) {
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

        if (!(input >> edgeId)) {
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

        if (!(input >> vertexId)) {
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

        if (!(input >> vertexId)) {
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
        string colourNames[4] = {
            "Blue",
            "Red",
            "Orange",
            "Yellow"
        };

        for (int i = 0; i < 4; ++i) {
            Builder &builder = *builders[i];

            cout << colourNames[i]
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

    } else if (action == "residences") {
        string colourName;

        if (current.getColour() == Colour::BLUE) {
            colourName = "Blue";
        } else if (current.getColour() == Colour::RED) {
            colourName = "Red";
        } else if (current.getColour() == Colour::ORANGE) {
            colourName = "Orange";
        } else {
            colourName = "Yellow";
        }

        cout << colourName << " has built:" << endl;

        for (int vertexId = 0;
             vertexId < 54;
             ++vertexId) {

            Residence *residence =
                board.getVertex(vertexId).getResidence();

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

    } else if (action == "trade") {
        string colourText;
        string giveText;
        string takeText;

        if (!(input >> colourText
                    >> giveText
                    >> takeText)) {
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

        string currentColour;

        if (current.getColour() == Colour::BLUE) {
            currentColour = "Blue";
        } else if (current.getColour() == Colour::RED) {
            currentColour = "Red";
        } else if (current.getColour() ==
                   Colour::ORANGE) {
            currentColour = "Orange";
        } else {
            currentColour = "Yellow";
        }

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
