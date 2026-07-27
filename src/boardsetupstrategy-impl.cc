module;

#include <fstream>
#include <string>

module boardsetupstrategy;

import types;

using namespace std;

FileBoardSetup::FileBoardSetup(const string &filename)
    : filename{filename} {}

bool FileBoardSetup::configure(Board &board) {
    ifstream input{filename};

    if (!input) {
        return false;
    }

    ResourceType resourceTypes[19];
    int values[19];

    for (int i = 0; i < 19; ++i) {
        int resourceCode;
        int value;

        if (!(input >> resourceCode >> value)) {
            return false;
        }

        if (resourceCode == 0) {
            resourceTypes[i] = ResourceType::BRICK;
        } else if (resourceCode == 1) {
            resourceTypes[i] = ResourceType::ENERGY;
        } else if (resourceCode == 2) {
            resourceTypes[i] = ResourceType::GLASS;
        } else if (resourceCode == 3) {
            resourceTypes[i] = ResourceType::HEAT;
        } else if (resourceCode == 4) {
            resourceTypes[i] = ResourceType::WIFI;
        } else if (resourceCode == 5) {
            resourceTypes[i] = ResourceType::PARK;
        } else {
            return false;
        }

        values[i] = value;
    }

    board.clear();

    for (int i = 0; i < 19; ++i) {
        board.addTile(resourceTypes[i], values[i]);
    }

    board.setupTopology();

    return true;
}


bool RandomBoardSetup::configure(Board &board) {
    board.clear();

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
        7
    };


    for (int i = 17; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(resourceTypes[i], resourceTypes[j]);
    }


    for (int i = 17; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(values[i], values[j]);
    }


    for (int i = 18; i > 0; --i) {
        int j = rand() % (i + 1);

        swap(resourceTypes[i], resourceTypes[j]);
        swap(values[i], values[j]);
    }

    for (int i = 0; i < 19; ++i) {
        board.setTile(i, resourceTypes[i], values[i]);
    }

    board.setupTopology();

    return true;
}
