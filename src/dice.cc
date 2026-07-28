export module dice;

export class Dice {
  public:
    virtual int roll() = 0;
    virtual ~Dice() = default;
};

export class LoadedDice final : public Dice {
  public:
    int roll() override;
};

export class FairDice final : public Dice {
  public:
    int roll() override;
};
