module tile;

Tile::Tile(int id, ResourceType type, int number)
    : id{id},
      type{type},
      number{number},
      hasGeese{type == ResourceType::PARK} {}

int Tile::getId() const {
    return id;
}

ResourceType Tile::getType() const {
    return type;
}

int Tile::getNumber() const {
    return number;
}

bool Tile::hasGeeseOnTile() const {
    return hasGeese;
}

void Tile::placeGeese() {
    hasGeese = true;
}

void Tile::removeGeese() {
    hasGeese = false;
}
