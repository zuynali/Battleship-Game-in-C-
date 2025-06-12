#pragma once
#include "Header.h"
#include "Vector.h"
using namespace std;
#include<random>

class AI {
private:
    Vector<pair<int, int>> huntTargets;
    Vector<pair<int, int>> shotHistory;
    GameMode difficulty;
    random_device rd;
    mutable mt19937 rng;

    pair<int, int> getRandomTarget(const vector<vector<Cellstate>>& enemyGrid);
    pair<int, int> getMediumTarget(const vector<vector<Cellstate>>& enemyGrid);
    pair<int, int> getHardTarget(const vector<vector<Cellstate>>& enemyGrid);
    int calculateCellProbability(int x, int y, const vector<vector<Cellstate>>& enemyGrid);

public:
    AI(GameMode diff);
    pair<int, int> getNextTarget(const vector<vector<Cellstate>>& enemyGrid);
    void registerHit(int x, int y, const vector<vector<Cellstate>>& enemyGrid);
};