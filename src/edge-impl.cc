module edge;

Edge::Edge(int id)
    : id{id}, built{false}, owner{Colour::NONE} {}

int Edge::getId() const {
    return id;
}

bool Edge::isBuilt() const {
    return built;
}

Colour Edge::getOwner() const {
    return owner;
}

void Edge::build(Colour owner) {
    built = true;
    this->owner = owner;
}