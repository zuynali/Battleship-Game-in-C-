#pragma once
#include "Header.h"
#include "Vector.h"
using namespace std;

class Ship {
protected:
    int size;
    Vector<pair<int, int>> coordinates;
    int hits;
    ShipType type;
    Color color;

public:
    Ship(int _size, ShipType _type, Color _color);
    virtual ~Ship() = 0;

    ShipType getType() const;
    Color getColor() const;
    int getSize() const;
    void setCoordinates(const Vector<pair<int, int>>& coords);
    bool isSunk() const;
    bool occupies(int x, int y) const;
    bool registerHit(int x, int y);
    const Vector<pair<int, int>>& getCoordinates() const;
    //virtual void activateSpecialAbility() {}
};