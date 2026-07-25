export module residence;

import types;

export class Residence{
    private:
    Colour owner;
    ResidenceType type;

    public:
    Residence(Colour owner);
    Colour getOwner() const;
    ResidenceType getType() const;
    void upgrade();
    int getProductionAmount() const;
};
