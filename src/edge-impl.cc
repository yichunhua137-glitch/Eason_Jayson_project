module edge;

// Initialize one edge as unbuilt and owned by nobody.
// 初始化一条 edge，默认未建造且没有 owner。
Edge::Edge(int id)
    : id{id}, built{false}, owner{Colour::NONE} {}

// Return the edge id.
// 返回 edge 的编号。
int Edge::getId() const {
    return id;
}

// Check whether a road has already been built on this edge.
// 检查这条 edge 上是否已经建了 road。
bool Edge::isBuilt() const {
    return built;
}

// Return the owner of the road on this edge.
// 返回这条 edge 上 road 的拥有者。
Colour Edge::getOwner() const {
    return owner;
}

// Mark this edge as built by the given colour.
// 把这条 edge 标记为由指定颜色建造。
void Edge::build(Colour owner) {
    built = true;
    this->owner = owner;
}
