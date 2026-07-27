module;

#include <cstdlib>

module builder;

Builder::Builder(Colour colour, Dice &initialDice)
    : colour(colour),
      brick(0),
      energy(0),
      glass(0),
      heat(0),
      wifi(0),
      points(0),
      currentDice(&initialDice) {}

Colour Builder::getColour() const{
    return colour;
}

int Builder::getResource(ResourceType type) const {
    if (type == ResourceType::BRICK) {
        return brick;
    } else if (type == ResourceType::ENERGY) {
        return energy;
    } else if (type == ResourceType::GLASS) {
        return glass;
    } else if (type == ResourceType::HEAT) {
        return heat;
    } else if (type == ResourceType::WIFI) {
        return wifi;
    }

    return 0;
}

void Builder::addResource(ResourceType type, int amount){
    if (amount <= 0) {
        return;
    }
    if (type == ResourceType::BRICK) {
         brick += amount;
    } else if (type == ResourceType::ENERGY) {
        energy += amount;
    } else if (type == ResourceType::GLASS) {
         glass += amount;
    } else if (type == ResourceType::HEAT) {
         heat += amount;
    } else if (type == ResourceType::WIFI) {
         wifi += amount;
    }
}

bool Builder::removeResource(ResourceType type, int amount) {
    if (amount < 0 || getResource(type) < amount) {
        return false;
    }

    if (type == ResourceType::BRICK) {
        brick -= amount;
    } else if (type == ResourceType::ENERGY) {
        energy -= amount;
    } else if (type == ResourceType::GLASS) {
        glass -= amount;
    } else if (type == ResourceType::HEAT) {
        heat -= amount;
    } else if (type == ResourceType::WIFI) {
        wifi -= amount;
    } else {
        return false;
    }

    return true;
}

int Builder::totalResources() const {
    return brick + energy + glass + heat + wifi;
}

ResourceType Builder::removeRandomResource() {
    int total = totalResources();

    if (total == 0) {
        return ResourceType::PARK;
    }

    int chosen = rand() % total;

    for (int resourceIndex = 0;
         resourceIndex < 5;
         ++resourceIndex) {
        ResourceType type =
            static_cast<ResourceType>(resourceIndex);
        int amount = getResource(type);

        if (chosen < amount) {
            removeResource(type, 1);
            return type;
        }

        chosen -= amount;
    }

    return ResourceType::PARK;
}

void Builder::reset() {
    brick = 0;
    energy = 0;
    glass = 0;
    heat = 0;
    wifi = 0;
    points = 0;
}

bool Builder::canAffordRoad() const {
    return heat >= 1 && wifi >= 1;
}

bool Builder::canAffordResidence(ResidenceType type) const {
    if (type == ResidenceType::BASEMENT) {
        return brick >= 1 &&
               energy >= 1 &&
               glass >= 1 &&
               wifi >= 1;
    } else if (type == ResidenceType::HOUSE) {
        return glass >= 2 &&
               heat >= 3;
    } else if (type == ResidenceType::TOWER) {
        return brick >= 3 &&
               energy >= 2 &&
               glass >= 2 &&
               wifi >= 1 &&
               heat >= 2;
    }

    return false;
}

void Builder::setDice(Dice &newDice) {
    currentDice = &newDice;
}

int Builder::rollDice() {
    return currentDice->roll();
}

int Builder::buildingPoints() const {
    return points;
}

void Builder::addBuildingPoints(int amount) {
    if (amount > 0) {
        points += amount;
    }
}





