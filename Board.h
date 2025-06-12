#pragma once
#include "Header.h"
#include "Ship.h"
#include "Vector.h"
#include <random>

class Board {
private:
    vector<vector<CellState>> grid;
    vector<Ship*> ships;
    random_device rd;
    mutable mt19937 rng;
    ParticleSystem* particleSystem;

public:
    Board(ParticleSystem* ps = nullptr)
        : rng(rd()), particleSystem(ps) {
        grid.resize(GRID_SIZE, vector<CellState>(GRID_SIZE, CellState::EMPTY));
    }

    ~Board() {
        for (auto ship : ships) {
            delete ship;
        }
    }

    bool canPlaceShip(int x, int y, int size) const {
        for (int i = 0; i < size; i++) {
            int newX = x + (i);
            int newY = y + (0);

            if (newX >= GRID_SIZE || newY >= GRID_SIZE) return false;

            // Check for ships and adjacent cells
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int checkX = newX + dx;
                    int checkY = newY + dy;
                    if (checkX >= 0 && checkX < GRID_SIZE &&
                        checkY >= 0 && checkY < GRID_SIZE) {
                        if (grid[checkX][checkY] == CellState::SHIP) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool placeShip(Ship* ship, int x, int y) {
        int size = ship->getSize();
        if (!canPlaceShip(x, y, size)) {
            return false;
        }

        vector<pair<int, int>> coords;
        for (int i = 0; i < size; i++) {
            int newX = x + (i);
            int newY = y + (0);
            grid[newX][newY] = CellState::SHIP;
            coords.push_back({ newX, newY });
        }

        ship->setCoordinates(coords);
        ships.push_back(ship);
        return true;
    }

    bool placeShipAutomatically(Ship* ship) {
        int size = ship->getSize();
        int maxAttempts = 100;

        for (int attempt = 0; attempt < maxAttempts; attempt++) {
            uniform_int_distribution<int> orientDist(0, 1);
            bool isVertical = orientDist(rng) == 1;

            uniform_int_distribution<int> distX(0, isVertical ? GRID_SIZE - 1 : GRID_SIZE - size);
            uniform_int_distribution<int> distY(0, isVertical ? GRID_SIZE - size : GRID_SIZE - 1);

            int x = distX(rng);
            int y = distY(rng);

            if (placeShip(ship, x, y)) {
                return true;
            }
        }

        delete ship;
        return false;
    }

    void setupFleet() {
        placeShipAutomatically(new Carrier());
        placeShipAutomatically(new Battleship());
        placeShipAutomatically(new Cruiser());
        placeShipAutomatically(new Submarine());
        placeShipAutomatically(new Destroyer());
    }

    bool attack(int x, int y) {
        if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE ||
            grid[x][y] == CellState::HIT || grid[x][y] == CellState::MISS) {
            return false;
        }

        Vector2 cellPos = {
            (float)(BOARD_OFFSET_X + BOARD_SPACING + x * CELL_SIZE + CELL_SIZE / 2),
            (float)(BOARD_OFFSET_Y + y * CELL_SIZE + CELL_SIZE / 2)
        };

        if (grid[x][y] == CellState::SHIP) {
            grid[x][y] = CellState::HIT;

            // Find and hit the ship
            Ship* hitShip = nullptr;
            for (auto ship : ships) {
                if (ship->occupies(x, y)) {
                    ship->registerHit(x, y);
                    hitShip = ship;
                    break;
                }
            }

            if (particleSystem) {
                particleSystem->addExplosion(cellPos, RED);
                if (hitShip && hitShip->isSunk()) {
                    particleSystem->addAnimation(cellPos, "SUNK!", YELLOW);
                }
                else {
                    particleSystem->addAnimation(cellPos, "HIT!", RED);
                }
            }

            return true;
        }
        else {
            grid[x][y] = CellState::MISS;
            if (particleSystem) {
                particleSystem->addSplash(cellPos);
                particleSystem->addAnimation(cellPos, "MISS", LIGHTGRAY);
            }
            return true;
        }
    }

    bool allShipsSunk() const {
        for (const auto ship : ships) {
            if (!ship->isSunk()) {
                return false;
            }
        }
        return !ships.empty();
    }

    CellState getCellState(int x, int y) const {
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            return grid[x][y];
        }
        return CellState::EMPTY;
    }

    const vector<vector<CellState>>& getGrid() const {
        return grid;
    }

    Ship* getShipAt(int x, int y) const {
        for (auto ship : ships) {
            if (ship->occupies(x, y)) {
                return ship;
            }
        }
        return nullptr;
    }

    void reset() {
        for (auto ship : ships) {
            delete ship;
        }
        ships.clear();
        for (auto& row : grid) {
            fill(row.begin(), row.end(), CellState::EMPTY);
        }
    }

    void draw(int offsetX, bool hideShips, bool showGrid = true) const {
        if (showGrid) {
            // Draw grid background
            DrawRectangle(offsetX - 5, BOARD_OFFSET_Y - 5,
                GRID_SIZE * CELL_SIZE + 10,
                GRID_SIZE * CELL_SIZE + 10, DARKBLUE);
        }

        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                Rectangle cell = {
                    (float)(offsetX + x * CELL_SIZE),
                    (float)(BOARD_OFFSET_Y + y * CELL_SIZE),
                    (float)(CELL_SIZE - 2),
                    (float)(CELL_SIZE - 2)
                };

                Color cellColor = BLUE;
                DrawRectangleRec(cell, cellColor);

                switch (grid[x][y]) {
                case CellState::SHIP:
                    if (!hideShips) {
                        Ship* ship = getShipAt(x, y);
                        if (ship) {
                            DrawRectangleRec(cell, ship->getColor());
                            DrawRectangleLinesEx(cell, 2, BLACK);
                        }
                    }
                    break;
                case CellState::HIT:
                    DrawRectangleRec(cell, RED);
                    DrawText("X", (int)(cell.x + CELL_SIZE / 3),
                        (int)(cell.y + CELL_SIZE / 3), 20, WHITE);
                    break;
                case CellState::MISS:
                    DrawCircle((int)(cell.x + CELL_SIZE / 2),
                        (int)(cell.y + CELL_SIZE / 2), 5, WHITE);
                    break;
                case CellState::SINKING:
                    DrawRectangleRec(cell, ORANGE);
                    DrawText("~", (int)(cell.x + CELL_SIZE / 3),
                        (int)(cell.y + CELL_SIZE / 3), 20, WHITE);
                    break;
                }

                // Highlight on hover
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, cell) && !hideShips) {
                    DrawRectangleLinesEx(cell, 3, YELLOW);
                }
            }
        }

        // Draw coordinate labels
        for (int x = 0; x < GRID_SIZE; x++) {
            char label = 'A' + x;
            DrawText(TextFormat("%c", label),
                offsetX + x * CELL_SIZE + CELL_SIZE / 3,
                BOARD_OFFSET_Y - 30, 20, WHITE);
        }

        for (int y = 0; y < GRID_SIZE; y++) {
            DrawText(TextFormat("%d", y + 1),
                offsetX - 30,
                BOARD_OFFSET_Y + y * CELL_SIZE + CELL_SIZE / 3,
                20, WHITE);
        }
    }
};
