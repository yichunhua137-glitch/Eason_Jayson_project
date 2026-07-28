export module tile;

import types;

export class Tile {
  private:
    int id;
    ResourceType type;
    int number;
    bool hasGeese;

  public:
    Tile(int id, ResourceType type, int number);
    int getId() const;
    ResourceType getType() const;
    int getNumber() const;
    bool hasGeeseOnTile() const;
    void placeGeese();
    void removeGeese();
};
