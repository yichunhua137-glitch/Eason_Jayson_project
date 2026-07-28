module residence;

Residence::Residence(Colour owner)
    : owner{owner}, type{ResidenceType::BASEMENT} {}

Colour Residence::getOwner() const {
    return owner;
}

ResidenceType Residence::getType() const {
    return type;
}

void Residence::upgrade() {
    if (type == ResidenceType::BASEMENT) {
        type = ResidenceType::HOUSE;
    } else if (type == ResidenceType::HOUSE) {
        type = ResidenceType::TOWER;
    }
}

int Residence::getProductionAmount() const {
    if (type == ResidenceType::BASEMENT) {
        return 1;
    } else if (type == ResidenceType::HOUSE) {
        return 2;
    } else if (type == ResidenceType::TOWER) {
        return 3;
    }
    return 0;
}
