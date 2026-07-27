module;

#include <memory>

export module builder;

import types;
import dice;

export class Builder {
  private:
    Colour colour;

    int brick;
    int energy;
    int glass;
    int heat;
    int wifi;

    int points;

    Dice *currentDice;

  public:
    Builder(Colour colour, Dice &initialDice);

    Colour getColour() const;
    int getResource(ResourceType type) const;

    void addResource(ResourceType type, int amount);
    bool removeResource(ResourceType type, int amount);
    int totalResources() const;
    ResourceType removeRandomResource();
    void reset();

    bool canAffordRoad() const;
    bool canAffordResidence(ResidenceType type) const;

    void setDice(Dice &newDice);
    int rollDice();

    int buildingPoints() const;
    void addBuildingPoints(int amount);
};
