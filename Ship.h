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

};