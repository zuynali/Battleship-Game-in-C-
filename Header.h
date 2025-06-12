#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>
using namespace std;

const int SCREEN_WIDTH = 1400;
const int SCREEN_HEIGHT = 800;
const int GRID_SIZE = 10;
const int CELL_SIZE = 45;
const int BOARD_OFFSET_X = 100;
const int BOARD_OFFSET_Y = 150;
const int BOARD_SPACING = 600;
const int MENU_WIDTH = 300;

enum class CellState { EMPTY, SHIP, HIT, MISS, SINKING };
enum class GameState { MAIN_MENU, SHIP_PLACEMENT, PLAYER1_TURN, PLAYER2_TURN, AI_TURN, GAME_OVER, SETTINGS, STATISTICS };
enum class ShipType { DESTROYER, SUBMARINE, CRUISER, BATTLESHIP, CARRIER };
enum class GameMode { PVP, PVE_EASY, PVE_MEDIUM, PVE_HARD };

struct GameStats {
    int gamesPlayed = 0;
    int playerWins = 0;
    int aiWins = 0;
    int totalShots = 0;
    int totalHits = 0;
    float avgAccuracy = 0.0f;
    int longestGame = 0;
    int shortestGame = 999;
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    float maxLife;
    float size;
};

struct Animation {
    Vector2 position;
    float time;
    float duration;
    Color color;
    string text;
};
