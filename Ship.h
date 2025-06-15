#pragma once
#include "Header.h"
#include "Vector.h"
using namespace std;

#ifndef SHIP_H
#define SHIP_H

class Ship {
protected:
    int size;
    Vector<pair<int, int>> coordinates;
    int hits;
    ShipType type;
    Color color;
    bool isVertical;
    int abilityUses;

public:
    Ship(int _size, ShipType _type, Color _color) :
        size(_size), hits(0), type(_type), color(_color),
        coordinates(), isVertical(false), abilityUses(1) {
    }

    virtual ~Ship() {}

    ShipType getType() const { return type; }
    Color getColor() const { return color; }
    int getSize() const { return size; }
    bool getIsVertical() const { return isVertical; }
    void setIsVertical(bool vertical) { isVertical = vertical; }
    int getAbilityUses() const { return abilityUses; }
    void useAbility() { if (abilityUses > 0) abilityUses--; }

    void setCoordinates(const Vector<pair<int, int>>& coords) {
        coordinates = coords;
    }

    bool isSunk() const { return hits >= size; }

    bool occupies(int x, int y) const {
        return find(coordinates.begin(), coordinates.end(), make_pair(x, y)) != coordinates.end();
    }

    bool registerHit(int x, int y) {
        if (occupies(x, y)) {
            hits++;
            return true;
        }
        return false;
    }

    const Vector<pair<int, int>>& getCoordinates() const {
        return coordinates;
    }

};


#endif // !SHIP_H