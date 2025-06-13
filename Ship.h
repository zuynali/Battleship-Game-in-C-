#pragma once
#include "Header.h"
#include "Vector.h"
using namespace std;

#ifndef SHIP_H
#define SHIP_H

class Ship {
protected:
    int size;
    vector<pair<int, int>> coordinates;
    int hits;
    ShipType type;
    Color color;
    bool isVertical;
    bool hasSpecialAbility;
    int abilityUses;

public:
    Ship(int _size, ShipType _type, Color _color) :
        size(_size), hits(0), type(_type), color(_color),
        coordinates(), isVertical(false), hasSpecialAbility(false), abilityUses(1) {
    }

    virtual ~Ship() {}

    ShipType getType() const { return type; }
    Color getColor() const { return color; }
    int getSize() const { return size; }
    bool getIsVertical() const { return isVertical; }
    void setIsVertical(bool vertical) { isVertical = vertical; }
    bool getHasSpecialAbility() const { return hasSpecialAbility; }
    int getAbilityUses() const { return abilityUses; }
    void useAbility() { if (abilityUses > 0) abilityUses--; }

    void setCoordinates(const vector<pair<int, int>>& coords) {
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

    const vector<pair<int, int>>& getCoordinates() const {
        return coordinates;
    }

    virtual void activateSpecialAbility() {}
};


#endif // !SHIP_H