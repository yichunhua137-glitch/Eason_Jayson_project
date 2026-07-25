module;

#include <string>

export module boardsetupstrategy;

import board;

export class BoardSetupStrategy {
  public:
    virtual bool configure(Board &board) = 0;
    virtual ~BoardSetupStrategy() = default;
};

export class FileBoardSetup : public BoardSetupStrategy {
  private:
    std::string filename;

  public:
    explicit FileBoardSetup(const std::string &filename);
    bool configure(Board &board) override;
};

export class RandomBoardSetup : public BoardSetupStrategy {
  public:
    bool configure(Board &board) override;
};
