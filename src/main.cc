#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

import boardsetupstrategy;
import gamecontroller;

using namespace std;

int main(int argc, char *argv[]) {
    string loadFilename;
    string boardFilename;

    unsigned int seed = 1;
    bool useRandomBoard = false;

    for (int i = 1; i < argc; ++i) {
        string option = argv[i];

        if (option == "-seed") {
            if (i + 1 >= argc) {
                cerr << "Missing seed." << endl;
                return 1;
            }

            istringstream seedInput{argv[++i]};
            string extra;

            if (!(seedInput >> seed) ||
                (seedInput >> extra)) {
                cerr << "Invalid seed." << endl;
                return 1;
            }
        } else if (option == "-load") {
            if (i + 1 >= argc) {
                cerr << "Missing load filename."
                     << endl;
                return 1;
            }

            loadFilename = argv[++i];
        } else if (option == "-board") {
            if (i + 1 >= argc) {
                cerr << "Missing board filename."
                     << endl;
                return 1;
            }

            boardFilename = argv[++i];
        } else if (option == "-random-board") {
            useRandomBoard = true;
        } else {
            cerr << "Invalid command-line option."
                 << endl;
            return 1;
        }
    }

    srand(seed);

    GameController controller;

    if (!loadFilename.empty()) {
        if (!controller.loadGame(loadFilename)) {
            cerr << "Unable to load game."
                 << endl;
            return 1;
        }
    } else if (!boardFilename.empty()) {
        FileBoardSetup setup{boardFilename};

        if (!controller.setupBoard(setup)) {
            cerr << "Unable to load board."
                 << endl;
            return 1;
        }

        controller.startNewGame();
    } else if (useRandomBoard) {
        RandomBoardSetup setup;

        if (!controller.setupBoard(setup)) {
            cerr << "Unable to create board."
                 << endl;
            return 1;
        }

        controller.startNewGame();
    } else {
        FileBoardSetup setup{"layout.txt"};

        if (!controller.setupBoard(setup)) {
            cerr << "Unable to load board."
                 << endl;
            return 1;
        }

        controller.startNewGame();
    }

    controller.run();

    return 0;
}
