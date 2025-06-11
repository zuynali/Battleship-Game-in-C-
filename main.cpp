// Battleship_game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <random>
using namespace std;

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 10;
const int CELL_SIZE = 40;
const int BOARD_OFFSET_X = 50;
const int BOARD_OFFSET_Y = 100;
const int BOARD_SPACING = 500;

enum class CellState { EMPTY, SHIP, HIT, MISS };
enum class GameState { SETUP, PLAYER1_TURN, PLAYER2_TURN, GAME_OVER };
enum class ShipType { DESTROYER, SUBMARINE, CRUISER, BATTLESHIP, CARRIER };

class Ship {
protected:
    int size;
    vector<pair<int, int>> coordinates;
    int hits;
    ShipType type;
    Color color;

public:
    Ship(int _size, ShipType _type, Color _color) : size(_size), hits(0), type(_type), color(_color) {}
    virtual ~Ship() {}

    ShipType getType() const { return type; }
    Color getColor() const { return color; }
    int getSize() const { return size; }

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
};

class Destroyer : public Ship {
public:
    Destroyer() : Ship(2, ShipType::DESTROYER, MAROON) {}
};

class Submarine : public Ship {
public:
    Submarine() : Ship(3, ShipType::SUBMARINE, DARKGRAY) {}
};

class Cruiser : public Ship {
public:
    Cruiser() : Ship(3, ShipType::CRUISER, BROWN) {}
};

class Battleship : public Ship {
public:
    Battleship() : Ship(4, ShipType::BATTLESHIP, DARKPURPLE) {}
};

class Carrier : public Ship {
public:
    Carrier() : Ship(5, ShipType::CARRIER, DARKGREEN) {}
};

class Board {
private:
    CellState grid[GRID_SIZE][GRID_SIZE];
    vector<Ship*> ships;
    random_device rd;
    mt19937 rng;

public:
    Board() : rng(rd()) {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                grid[i][j] = CellState::EMPTY;
            }
        }
    }

    ~Board() {
        for (auto ship : ships) {
            delete ship;
        }
    }

    bool placeShipAutomatically(Ship* ship) {
        int size = ship->getSize();
        int maxAttempts = 100;

        for (int attempt = 0; attempt < maxAttempts; attempt++) {
            bool horizontal = true;
            uniform_int_distribution<int> distX(0, horizontal ? GRID_SIZE - size : GRID_SIZE - 1);
            uniform_int_distribution<int> distY(0, horizontal ? GRID_SIZE - 1 : GRID_SIZE - size);
            int x = distX(rng);
            int y = distY(rng);
            bool valid = true;
            vector<pair<int, int>> coords;

            for (int i = 0; i < size; i++) {
                int newX = x + (horizontal ? i : 0);
                int newY = y + (horizontal ? 0 : i);

                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        int checkX = newX + dx;
                        int checkY = newY + dy;

                        if (checkX >= 0 && checkX < GRID_SIZE && checkY >= 0 && checkY < GRID_SIZE) {
                            if (grid[checkX][checkY] == CellState::SHIP) {
                                valid = false;
                                break;
                            }
                        }
                    }
                    if (!valid) break;
                }

                if (!valid) break;
                coords.push_back(make_pair(newX, newY));
            }

            if (valid) {
                for (const auto& coord : coords) {
                    grid[coord.first][coord.second] = CellState::SHIP;
                }

                ship->setCoordinates(coords);
                ships.push_back(ship);
                return true;
            }
        }

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

        if (grid[x][y] == CellState::SHIP) {
            grid[x][y] = CellState::HIT;
            for (auto ship : ships) {
                if (ship->occupies(x, y)) {
                    ship->registerHit(x, y);
                    break;
                }
            }
            return true;
        }
        else {
            grid[x][y] = CellState::MISS;
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

    Ship* getShipAt(int x, int y) const {
        for (auto ship : ships) {
            if (ship->occupies(x, y)) {
                return ship;
            }
        }
        return nullptr;
    }

    void draw(int offsetX, bool hideShips) const {
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                Rectangle cell = {
                    static_cast<float>(offsetX + x * CELL_SIZE),
                    static_cast<float>(BOARD_OFFSET_Y + y * CELL_SIZE),
                    static_cast<float>(CELL_SIZE - 2),
                    static_cast<float>(CELL_SIZE - 2)
                };

                Color cellColor = BLUE;
                DrawRectangleRec(cell, cellColor);

                switch (grid[x][y]) {
                case CellState::SHIP:
                    if (!hideShips) {
                        Ship* ship = getShipAt(x, y);
                        if (ship) {
                            DrawRectangleRec(cell, ship->getColor());
                            DrawRectangleLinesEx(cell, 1, BLACK);
                        }
                    }
                    break;
                case CellState::HIT:
                    DrawRectangleRec(cell, RED);
                    DrawText("X", offsetX + x * CELL_SIZE + CELL_SIZE / 3,
                        BOARD_OFFSET_Y + y * CELL_SIZE + CELL_SIZE / 3, 20, WHITE);
                    break;
                case CellState::MISS:
                    DrawCircle(offsetX + x * CELL_SIZE + CELL_SIZE / 2,
                        BOARD_OFFSET_Y + y * CELL_SIZE + CELL_SIZE / 2,
                        5, WHITE);
                    break;
                }
            }
        }

    }
};

class Game {
private:
    Board playerBoard;
    Board computerBoard;
    GameState state;
    int winner;

public:
    Game() : state(GameState::SETUP), winner(0) {
        playerBoard.setupFleet();
        computerBoard.setupFleet();
        state = GameState::PLAYER1_TURN;
    }

    void update() {
        if (state == GameState::GAME_OVER) {
            return;
        }

        if (playerBoard.allShipsSunk()) {
            state = GameState::GAME_OVER;
            winner = 2;
            return;
        }

        if (computerBoard.allShipsSunk()) {
            state = GameState::GAME_OVER;
            winner = 1;
            return;
        }

        if (state == GameState::PLAYER1_TURN) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                int gridX = (mousePos.x - (BOARD_OFFSET_X + BOARD_SPACING)) / CELL_SIZE;
                int gridY = (mousePos.y - BOARD_OFFSET_Y) / CELL_SIZE;

                if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
                    if (computerBoard.attack(gridX, gridY)) {
                        state = GameState::PLAYER2_TURN;
                    }
                }
            }
        }
        else if (state == GameState::PLAYER2_TURN) {
            static random_device rd;
            static mt19937 rng(rd());
            static uniform_int_distribution<int> dist(0, GRID_SIZE - 1);
            static float timer = 0;
            timer += GetFrameTime();

            if (timer >= 0.5f) {
                timer = 0;

                for (int attempts = 0; attempts < 100; attempts++) {
                    int x = dist(rng);
                    int y = dist(rng);

                    if (playerBoard.attack(x, y)) {
                        state = GameState::PLAYER1_TURN;
                        break;
                    }
                }
            }
        }
    }

    void draw() {
        ClearBackground(BLACK);

        DrawText("BATTLESHIP", SCREEN_WIDTH / 2 - 100, 20, 30, WHITE);
        DrawText("YOUR FLEET", BOARD_OFFSET_X, BOARD_OFFSET_Y - 45, 20, WHITE);
        playerBoard.draw(BOARD_OFFSET_X, false);
        DrawText("AI FLEET", BOARD_OFFSET_X + BOARD_SPACING, BOARD_OFFSET_Y - 45, 20, WHITE);
        computerBoard.draw(BOARD_OFFSET_X + BOARD_SPACING, true);

        if (state == GameState::PLAYER1_TURN) {
            DrawText("YOUR TURN - CLICK TO ATTACK", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 50, 20, DARKBLUE);
        }
        else if (state == GameState::PLAYER2_TURN) {
            DrawText("AI'S TURN", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 50, 20, BLUE);
        }

        if (state == GameState::GAME_OVER) {
            const char* message = (winner == 1) ? "YOU WIN!" : "AI WINS!";
            DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 30, 200, 60, ColorAlpha(BLACK, 0.8f));
            DrawText(message, SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 15, 30, winner == 1 ? GREEN : RED);
        }
    }

    GameState getState() const {
        return state;
    }
};

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Battleship Game");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose()) {
        game.update();

        BeginDrawing();
        game.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
