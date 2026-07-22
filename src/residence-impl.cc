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