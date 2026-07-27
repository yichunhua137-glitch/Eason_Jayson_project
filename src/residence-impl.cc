module residence;

// Initialize a residence owned by one builder as a basement.
// 初始化一个属于某个 builder 的 residence，默认类型是 basement。
Residence::Residence(Colour owner)
    : owner{owner}, type{ResidenceType::BASEMENT} {}

// Return the owner colour of this residence.
// 返回这个 residence 的 owner 颜色。
Colour Residence::getOwner() const {
    return owner;
}

// Return the current residence type.
// 返回当前 residence 的类型。
ResidenceType Residence::getType() const {
    return type;
}

// Upgrade the residence by one level: Basement -> House -> Tower.
// 把 residence 升一级：Basement -> House -> Tower。
void Residence::upgrade() {
    if (type == ResidenceType::BASEMENT) {
        type = ResidenceType::HOUSE;
    } else if (type == ResidenceType::HOUSE) {
        type = ResidenceType::TOWER;
    }
}

// Return how many resources this residence produces when an adjacent tile activates.
// 返回这个 residence 在相邻 tile 生产时能提供多少资源。
int Residence::getProductionAmount() const{
    if (type == ResidenceType::BASEMENT) {
        return 1;
    } else if (type == ResidenceType::HOUSE) {
        return 2;
    } else if (type == ResidenceType::TOWER) {
        return 3;
    }
    return 0;
}
