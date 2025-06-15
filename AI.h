#pragma once
#include "Header.h"
using namespace std;

#ifndef AI_H
#define AI_H

class AI {
private:
    vector<pair<int, int>> huntTargets;
    vector<pair<int, int>> shotHistory;
    GameMode difficulty;
    random_device rd;//class of random to generate numbers from hardware
    mutable mt19937 rng;

public:
    AI(GameMode diff) : difficulty(diff), rng(rd()) {}

    pair<int, int> getNextTarget(const vector<vector<CellState>>& enemyGrid) {
        //if we have targets from previous hits
        if (!huntTargets.empty() && difficulty != GameMode::PVE_EASY) {
            auto target = huntTargets.back();
            huntTargets.pop_back();
            return target;
        }

        //different strategies based on difficulty
        switch (difficulty) {
        case GameMode::PVE_EASY:
            return getRandomTarget(enemyGrid);
        case GameMode::PVE_MEDIUM:
            return getMediumTarget(enemyGrid);
        case GameMode::PVE_HARD:
            return getHardTarget(enemyGrid);
        default:
            return getRandomTarget(enemyGrid);
        }
    }

    void registerHit(int x, int y, const vector<vector<CellState>>& enemyGrid) {
        if (difficulty == GameMode::PVE_EASY) return;

        //add adjacent cells to hunt targets
        vector<pair<int, int>> directions = { {0,1}, {0,-1}, {1,0}, {-1,0} };
        for (auto& dir : directions) {
            int newX = x + dir.first;
            int newY = y + dir.second;
            if (newX >= 0 && newX < GRID_SIZE && newY >= 0 && newY < GRID_SIZE) {
                if (enemyGrid[newX][newY] == CellState::EMPTY ||
                    enemyGrid[newX][newY] == CellState::SHIP) {
                    huntTargets.push_back({ newX, newY });
                }
            }
        }
    }

private:
    pair<int, int> getRandomTarget(const vector<vector<CellState>>& enemyGrid) {
        uniform_int_distribution<int> dist(0, GRID_SIZE - 1);
        int attempts = 0;
        while (attempts < 100) {
            int x = dist(rng);
            int y = dist(rng);
            if (enemyGrid[x][y] == CellState::EMPTY || enemyGrid[x][y] == CellState::SHIP) {
                return { x, y };
            }
            attempts++;
        }
        return { 0, 0 }; 
    }

    pair<int, int> getMediumTarget(const vector<vector<CellState>>& enemyGrid) {
        //checkerboard pattern for better coverage
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                if ((x + y) % 2 == 0 &&
                    (enemyGrid[x][y] == CellState::EMPTY || enemyGrid[x][y] == CellState::SHIP)) {
                    return { x, y };
                }
            }
        }
        return getRandomTarget(enemyGrid);
    }

    pair<int, int> getHardTarget(const vector<vector<CellState>>& enemyGrid) {
        //target areas with highest probability
        vector<vector<int>> probability(GRID_SIZE, vector<int>(GRID_SIZE, 0));

        //calculate probability based on possible ship placements
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                if (enemyGrid[x][y] == CellState::EMPTY || enemyGrid[x][y] == CellState::SHIP) {
                    probability[x][y] = calculateCellProbability(x, y, enemyGrid);
                }
            }
        }

        //highest probability cell
        int maxProb = 0;
        pair<int, int> bestTarget = { 0, 0 };
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                if (probability[x][y] > maxProb) {
                    maxProb = probability[x][y];
                    bestTarget = { x, y };
                }
            }
        }

        return bestTarget;
    }

    int calculateCellProbability(int x, int y, const vector<vector<CellState>>& enemyGrid) {
        int probability = 0;
        vector<int> shipSizes = { 2, 3, 3, 4, 5 }; //total ship sizes

        for (int size : shipSizes) {
            //check horizontal placement
            bool canPlaceHorizontal = true;
            if (x + size <= GRID_SIZE) {
                for (int i = 0; i < size; i++) {
                    if (enemyGrid[x + i][y] == CellState::HIT ||
                        enemyGrid[x + i][y] == CellState::MISS) {
                        canPlaceHorizontal = false;
                        break;
                    }
                }
                if (canPlaceHorizontal) probability++;
            }

            //check vertical placement
            bool canPlaceVertical = true;
            if (y + size <= GRID_SIZE) {
                for (int i = 0; i < size; i++) {
                    if (enemyGrid[x][y + i] == CellState::HIT ||
                        enemyGrid[x][y + i] == CellState::MISS) {
                        canPlaceVertical = false;
                        break;
                    }
                }
                if (canPlaceVertical) probability++;
            }
        }

        return probability;
    }
};


#endif