module;

#include <string>
#include <vector>

export module gamecontroller;

import board;
import builder;
import dice;
import types;
import gamestateio;
import boardsetupstrategy;

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
    GameStateIO gameStateIO;
    Builder &getCurrentBuilder();
    void distributeResources(int roll);
    void printBuilderStatus(const Builder &builder) const;
    void printTurnStart() const;
    bool performInitialPlacement();
    void grantInitialResources(
        Builder &builder,
        int vertexId
    );
    bool handleGeese();
    void discardResourcesForGeese();
    std::vector<Builder *> getStealableBuilders(
        int tileId
    );

  public:
    GameController();

    void run();
    void startNewGame();
    void processCommand(const std::string &command);
    void nextTurn();
    bool loadGame(const std::string &filename);
    bool setupBoard(BoardSetupStrategy &strategy);

    bool handleTrade(
        Builder &other,
        ResourceType give,
        ResourceType take
    );

    bool hasWinner() const;
};
