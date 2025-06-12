#include "AI.h"
#include <stdlib.h> // For rand()

AI::AI(GameMode diff) : difficulty(diff), rng(rd()) {}

pair<int, int> AI::getNextTarget(const vector<vector<Cellstate>>& enemyGrid) {
    if (!huntTargets.empty() && difficulty != GameMode::PvAi_easy) {
        auto target = huntTargets.back();
        huntTargets.pop_back_pair();
        return target;
    }

    switch (difficulty) {
    case GameMode::PvAi_easy: return getRandomTarget(enemyGrid);
    case GameMode::PvAi_medium: return getMediumTarget(enemyGrid);
    case GameMode::PvAi_hard: return getHardTarget(enemyGrid);
    default: return getRandomTarget(enemyGrid);
    }
}

pair<int, int> AI::getRandomTarget(const vector<vector<Cellstate>>& enemyGrid) {
    uniform_int_distribution<int> dist(0, grid_size - 1);
    int attempts = 0;
    while (attempts < 100) {
        int x = dist(rng);
        int y = dist(rng);
        if (enemyGrid[x][y] == Cellstate::Empty || enemyGrid[x][y] == Cellstate::Ship) {
            return { x, y };
        }
        attempts++;
    }
    return { 0, 0 }; // Fallback
}

pair<int, int> AI::getMediumTarget(const vector<vector<Cellstate>>& enemyGrid) {
    // Checkerboard pattern for better coverage
    for (int x = 0; x < grid_size; x++) {
        for (int y = 0; y < grid_size; y++) {
            if ((x + y) % 2 == 0 &&
                (enemyGrid[x][y] == Cellstate::Empty || enemyGrid[x][y] == Cellstate::Ship)) {
                return { x, y };
            }
        }
    }
    return getRandomTarget(enemyGrid);
}

pair<int, int> AI::getHardTarget(const vector<vector<Cellstate>>& enemyGrid) {
    Vector<Vector<int>> probability(grid_size, Vector<int>(grid_size, 0));

    // Calculate probability based on possible ship placements
    for (int x = 0; x < grid_size; x++) {
        for (int y = 0; y < grid_size; y++) {
            if (enemyGrid[x][y] == Cellstate::Empty || enemyGrid[x][y] == Cellstate::Ship) {
                probability[x][y] = calculateCellProbability(x, y, enemyGrid);
            }
        }
    }

    // Find highest probability cell
    int maxProb = 0;
    pair<int, int> bestTarget = { 0, 0 };
    for (int x = 0; x < grid_size; x++) {
        for (int y = 0; y < grid_size; y++) {
            if (probability[x][y] > maxProb) {
                maxProb = probability[x][y];
                bestTarget = { x, y };
            }
        }
    }

    return bestTarget;
}

int AI::calculateCellProbability(int x, int y, const vector<vector<Cellstate>>& enemyGrid) {
    int probability = 0;
    vector<int> shipSizes = { 2, 3, 3, 4, 5 };

    for (int size : shipSizes) {
        // Check horizontal placement
        bool canPlaceHorizontal = true;
        if (x + size <= grid_size) {
            for (int i = 0; i < size; i++) {
                if (enemyGrid[x + i][y] == Cellstate::Hit ||
                    enemyGrid[x + i][y] == Cellstate::Miss) {
                    canPlaceHorizontal = false;
                    break;
                }
            }
            if (canPlaceHorizontal) probability++;
        }

        // Check vertical placement
        bool canPlaceVertical = true;
        if (y + size <= grid_size) {
            for (int i = 0; i < size; i++) {
                if (enemyGrid[x][y + i] == Cellstate::Hit ||
                    enemyGrid[x][y + i] == Cellstate::Miss) {
                    canPlaceVertical = false;
                    break;
                }
            }
            if (canPlaceVertical) probability++;
        }
    }

    return probability;
}

void AI::registerHit(int x, int y, const vector<vector<Cellstate>>& enemyGrid) {
    if (difficulty == GameMode::PvAi_easy) return;

    vector<pair<int, int>> directions = { {0,1}, {0,-1}, {1,0}, {-1,0} };
    for (int i = 0; i < directions.size(); i++) {
        int newX = x + directions[i].first;
        int newY = y + directions[i].second;
        if (newX >= 0 && newX < grid_size && newY >= 0 && newY < grid_size) {
            if (enemyGrid[newX][newY] == Cellstate::Empty ||
                enemyGrid[newX][newY] == Cellstate::Ship) {
                huntTargets.push_back({ newX, newY });
            }
        }
    }
}