module tile;

// Initialize one tile with its id, resource type, dice value, and default geese state.
// 初始化一个 tile，记录编号、资源类型、骰子点数，以及默认的 geese 状态。
Tile::Tile(int id, ResourceType type, int number)
    : id{id},
      type{type},
      number{number},
      hasGeese{type == ResourceType::PARK} {}

// Return the tile id.
// 返回 tile 的编号。
int Tile::getId() const {
    return id;
}

// Return the resource type stored on this tile.
// 返回这个 tile 的资源类型。
ResourceType Tile::getType() const {
    return type;
}

// Return the production number of this tile.
// 返回这个 tile 的生产点数。
int Tile::getNumber() const {
    return number;
}

// Check whether geese are currently on this tile.
// 检查 geese 当前是否在这个 tile 上。
bool Tile::hasGeeseOnTile() const {
    return hasGeese;
}

// Place geese onto this tile.
// 把 geese 放到这个 tile 上。
void Tile::placeGeese() {
    hasGeese = true;
}

// Remove geese from this tile.
// 把 geese 从这个 tile 上移走。
void Tile::removeGeese() {
    hasGeese = false;
}
