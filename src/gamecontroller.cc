module;

#include <string>

export module gamecontroller;

import board;
import builder;
import dice;
import types;

export class GameController {
  private:
    Board board;

    FairDice fairDice;
    LoadedDice loadedDice;

    Builder blueBuilder;
    Builder redBuilder;
    Builder orangeBuilder;
    Builder yellowBuilder;

    Builder *builders[4];

    int currentTurn;
    bool gameRunning;
    bool hasRolled;
    
    Builder &getCurrentBuilder();

  public:
    GameController();

    void run();
    void startNewGame();
    void processCommand(const std::string &command);
    void nextTurn();

    bool handleTrade(
        Builder &other,
        ResourceType give,
        ResourceType take
    );

    bool hasWinner() const;
};
