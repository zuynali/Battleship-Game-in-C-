#include "Ship.h"

Ship::Ship(int _size, ShipType _type, Color _color) :
    size(_size), hits(0), type(_type), color(_color) {
}

Ship::~Ship() {}

ShipType Ship::getType() const { return type; }
Color Ship::getColor() const { return color; }
int Ship::getSize() const { return size; }

void Ship::setCoordinates(const Vector<std::pair<int, int>>& coords) {
    coordinates = coords;
}

bool Ship::isSunk() const {
    return hits >= size;
}

bool Ship::occupies(int x, int y) const {
    for (int i = 0; i < coordinates.size(); i++) {
        if (coordinates[i].first == x && coordinates[i].second == y) {
            return true;
        }
    }
    return false;
}

bool Ship::registerHit(int x, int y) {
    if (occupies(x, y)) {
        hits++;
        return true;
    }
    return false;
}

const Vector<std::pair<int, int>>& Ship::getCoordinates() const {
    return coordinates;
}
