module;

#include <string>

export module gamestateio;

import board;
import builder;

using namespace std;

export class GameStateIO {
  private:
    Board &board;
    Builder **builders;
    int &currentTurn;

  public:
    GameStateIO(
        Board &board,
        Builder **builders,
        int &currentTurn
    );

    bool save(const string &filename) const;
    bool load(const string &filename);
};