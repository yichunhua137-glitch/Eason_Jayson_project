module vertex;

// Initialize one vertex with no residence on it.
// 初始化一个 vertex，默认上面没有 residence。
Vertex::Vertex(int id)
    : id{id}, residence{nullptr} {}

// Return the vertex id.
// 返回 vertex 的编号。
int Vertex::getId() const {
    return id;
}

// Check whether this vertex currently has a residence.
// 检查这个 vertex 当前是否有 residence。
bool Vertex::hasResidence() const {
    return residence != nullptr;
}

// Return the residence pointer stored on this vertex.
// 返回这个 vertex 上保存的 residence 指针。
Residence *Vertex::getResidence() const {
    return residence;
}

// Place the given residence pointer onto this vertex.
// 把给定的 residence 指针放到这个 vertex 上。
void Vertex::buildResidence(Residence *residence) {
    this->residence = residence;
}
