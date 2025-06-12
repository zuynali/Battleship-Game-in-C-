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

    // Position and orientation
    int x, y;
    bool horizontal;

public:
    Ship(int _size, ShipType _type, Color _color);
    virtual ~Ship();

    ShipType getType() const;
    Color getColor() const;
    int getSize() const;
    void setCoordinates(const Vector<pair<int, int>>& coords);
    bool isSunk() const;
    bool occupies(int x, int y) const;
    bool registerHit(int x, int y);
    const Vector<pair<int, int>>& getCoordinates() const;

    // Position and orientation methods
    void setPosition(int _x, int _y);
    void setOrientation(bool _horizontal);
    int getX() const;
    int getY() const;
    bool isHorizontal() const;

    // Update coordinates based on position and orientation
    void updateCoordinates();

    //virtual void activateSpecialAbility() {}
};