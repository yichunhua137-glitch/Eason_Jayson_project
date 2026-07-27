module;

#include <fstream>
#include <sstream>
#include <string>

module gamestateio;

import types;
import tile;
import vertex;
import edge;
import residence;

using namespace std;

GameStateIO::GameStateIO(Board &board, Builder **builders, int &currentTurn)
    : board{board},
      builders{builders},
      currentTurn{currentTurn}
{
}

bool GameStateIO::save(const string &filename) const {
    ofstream output{filename};

    if (!output) {
        return false;
    }

    output << (currentTurn + 1) % 4 << '\n';

    for (int builderIndex = 0;
         builderIndex < 4;
         ++builderIndex) {

        Builder &builder = *builders[builderIndex];

        output
            << builder.getResource(ResourceType::BRICK)
            << ' '
            << builder.getResource(ResourceType::ENERGY)
            << ' '
            << builder.getResource(ResourceType::GLASS)
            << ' '
            << builder.getResource(ResourceType::HEAT)
            << ' '
            << builder.getResource(ResourceType::WIFI)
            << " r";

        for (int edgeId = 0;
             edgeId < 72;
             ++edgeId) {

            const Edge &edge =
                board.getEdge(edgeId);

            if (edge.isBuilt() &&
                edge.getOwner() ==
                    builder.getColour()) {

                output << ' ' << edgeId;
            }
        }

        output << " h";

        for (int vertexId = 0;
             vertexId < 54;
             ++vertexId) {

            const Vertex &vertex =
                board.getVertex(vertexId);

            Residence *residence =
                vertex.getResidence();

            if (residence == nullptr ||
                residence->getOwner() !=
                    builder.getColour()) {

                continue;
            }

            char residenceType;

            if (residence->getType() ==
                ResidenceType::BASEMENT) {

                residenceType = 'B';
            } else if (
                residence->getType() ==
                ResidenceType::HOUSE
            ) {
                residenceType = 'H';
            } else {
                residenceType = 'T';
            }

            output
                << ' '
                << vertexId
                << ' '
                << residenceType;
        }

        output << '\n';
    }

    int parkId = -1;
    int geeseId = -1;

    for (int tileId = 0;
         tileId < 19;
         ++tileId) {

        const Tile &tile =
            board.getTile(tileId);

        if (tileId > 0) {
            output << ' ';
        }

        output
            << static_cast<int>(
                   tile.getType()
               )
            << ' '
            << tile.getNumber();

        if (tile.getType() ==
            ResourceType::PARK) {

            parkId = tileId;
        }

        if (tile.hasGeeseOnTile()) {
            geeseId = tileId;
        }
    }

    output << '\n';

    if (geeseId == -1) {
        geeseId = parkId;
    }

    output << geeseId << '\n';

    return static_cast<bool>(output);
}

bool GameStateIO::load(const string &filename) {
    ifstream input{filename};

    if (!input) {
        return false;
    }

    int loadedTurn;

    if (!(input >> loadedTurn)) {
        return false;
    }

    string line;
    getline(input, line);

    string builderLines[4];

    for (int builderIndex = 0;
         builderIndex < 4;
         ++builderIndex) {

        if (!getline(
                input,
                builderLines[builderIndex]
            )) {

            return false;
        }
    }

    ResourceType tileTypes[19];
    int tileValues[19];

    for (int tileId = 0;
         tileId < 19;
         ++tileId) {

        int resourceCode;

        if (!(input >>
              resourceCode >>
              tileValues[tileId])) {

            return false;
        }

        tileTypes[tileId] =
            static_cast<ResourceType>(
                resourceCode
            );
    }

    int geeseId;

    if (!(input >> geeseId)) {
        return false;
    }

    board.setupDefaultBoard();

    for (int tileId = 0;
         tileId < 19;
         ++tileId) {

        board.setTile(
        tileId,
        tileTypes[tileId],
        tileValues[tileId]
);
    }

    for (int builderIndex = 0;
         builderIndex < 4;
         ++builderIndex) {

        istringstream builderInput{
            builderLines[builderIndex]
        };

        int resources[5];

        for (int resourceIndex = 0;
             resourceIndex < 5;
             ++resourceIndex) {

            builderInput >>
                resources[resourceIndex];

            builders[builderIndex]
                ->addResource(
                    static_cast<ResourceType>(
                        resourceIndex
                    ),
                    resources[resourceIndex]
                );
        }

        char marker;
        builderInput >> marker;

        Colour colour =
            builders[builderIndex]
                ->getColour();

        string roadToken;

        while (builderInput >> roadToken &&
               roadToken != "h") {

            int edgeId = stoi(roadToken);

            board.buildRoad(
                edgeId,
                colour
            );
        }

        int vertexId;
        char residenceType;

        while (builderInput >>
               vertexId >>
               residenceType) {

            board.buildResidence(
                vertexId,
                colour
            );

            int points = 1;

            if (residenceType == 'H') {
                board.upgradeResidence(
                    vertexId
                );

                points = 2;
            } else if (
                residenceType == 'T'
            ) {
                board.upgradeResidence(
                    vertexId
                );

                board.upgradeResidence(
                    vertexId
                );

                points = 3;
            }

            builders[builderIndex]
                ->addBuildingPoints(
                    points
                );
        }
    }

    board.getTile(geeseId)
        .placeGeese();

    currentTurn = loadedTurn;

    return true;
}