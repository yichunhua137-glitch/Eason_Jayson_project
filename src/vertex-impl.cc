module vertex;

Vertex::Vertex(int id)
    : id{id}, residence{nullptr} {}

int Vertex::getId() const {
    return id;
}

bool Vertex::hasResidence() const {
    return residence != nullptr;
}

Residence *Vertex::getResidence() const {
    return residence;
}

void Vertex::buildResidence(Residence *residence) {
    this->residence = residence;
}