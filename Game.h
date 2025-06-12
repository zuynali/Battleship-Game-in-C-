#pragma once
#include "Header.h"
#include "Board.h"
#include "AI.h"
#include "ParticleSystem.h"
#include "Vector.h"
using namespace std;

class Game {
private:
    Board playerBoard;
    Board computerBoard;
    Gamestate state;
    GameMode gameMode;
    int winner;
    int currentPlayer;
    Game_stats stats;
    ParticleSystem particleSystem;
    AI* ai;

    Vector<Ship*> shipsToPlace;
    int currentShipIndex;
    bool isPlacingShips;

    // UI state variables
    int selectedMenuItem;
    int selectedDifficulty;
    bool showStats;
    bool showSettings;
    float turnTimer;
    float gameTimer;
    int shotCount;

    // UI rectangles
    Rectangle playButton;
    Rectangle pvpButton;
    Rectangle settingsButton;
    Rectangle statsButton;
    Rectangle quitButton;

public:
    Game();
    ~Game();

    void initializeShipsToPlace();
    void startNewGame();
    void update();
    void draw();

private:
    void updateMainMenu();
    void updateShipPlacement();
    void updatePlayerTurn();
    void updateAITurn(float deltaTime);
    void updateGameOver();
    void updateSettings();
    void updateStatistics();

    void drawMainMenu();
    void drawShipPlacement();
    void drawGameplay();
    void drawGameOver();
    void drawSettings();
    void drawStatistics();

    Ship* createShipCopy(Ship* original);
    void showHint();
    void updateGameStats();
    void loadStats();
    void saveStats();
    void resetStats();
};