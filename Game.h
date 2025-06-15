#pragma once
#include "Header.h"
#include "Board.h"
#include "AI.h"
#include "ParticleSystem.h"
#include "Vector.h"
#include "Battleship.h"
#include "Carrier.h"
#include "Cruiser.h"
#include "Destroyer.h"
#include "Submarine.h"
#include <iostream>
#include <fstream>
using namespace std;

#ifndef GAME_H
#define GAME_H

class Game {
private:
    Board playerBoard;
    Board computerBoard;
    GameState state;
    GameMode gameMode;
    PlacementMode placementMode;
    int winner;
    int currentPlayer;
    GameStats stats;
    ParticleSystem particleSystem;
    AI* ai;
    Vector<Ship*> shipsToPlace;
    int currentShipIndex;
    bool isPlacingShips;
    int selectedMenuItem;
    int selectedDifficulty;
    bool showStats;
    bool showSettings;
    float turnTimer;
    float gameTimer;
    int shotCount;
    int playerHits;
    int playerLevel;

    Rectangle playButton = { SCREEN_WIDTH / 2 - 100, 300, 200, 50 };
    Rectangle pvpButton = { SCREEN_WIDTH / 2 - 100, 370, 200, 50 };
    Rectangle settingsButton = { SCREEN_WIDTH / 2 - 100, 440, 200, 50 };
    Rectangle quitButton = { SCREEN_WIDTH / 2 - 100, 510, 200, 50 };

public:
    Game() : playerBoard(&particleSystem), computerBoard(&particleSystem),
        state(GameState::MAIN_MENU), gameMode(GameMode::PVE_MEDIUM),
        placementMode(PlacementMode::HORIZONTAL), winner(0), currentPlayer(1),
        ai(nullptr), currentShipIndex(0), isPlacingShips(false),
        selectedMenuItem(0), selectedDifficulty(1), showStats(false),
        showSettings(false), turnTimer(0), gameTimer(0), shotCount(0),
        playerHits(0), playerLevel(1) {

        loadStats(); 
        initializeShipsToPlace();    
    }

    ~Game() {
        saveStats();
        delete ai;
        for (auto ship : shipsToPlace) {
            delete ship;
        }
    }

    void initializeShipsToPlace() {
        shipsToPlace.clear();
        shipsToPlace.push_back(new Carrier());
        shipsToPlace.push_back(new Battleship());
        shipsToPlace.push_back(new Cruiser());
        shipsToPlace.push_back(new Submarine());
        shipsToPlace.push_back(new Destroyer());
    }

    void startNewGame() {
        playerBoard.reset();
        computerBoard.reset();
        delete ai;
        ai = new AI(gameMode);
        initializeShipsToPlace();
        currentShipIndex = 0;
        isPlacingShips = true;
        state = GameState::SHIP_PLACEMENT;
        computerBoard.setupFleet();
        turnTimer = 0;
        gameTimer = 0;
        shotCount = 0;
        playerHits = 0;
        currentPlayer = 1;
    }

    void calculatePlayerLevel() {
        stats.experience = stats.gamesPlayed + (stats.playerWins * 2);
        playerLevel = 1 + (stats.experience / 5); //level up every 5 experience points
        if (playerLevel > 50) playerLevel = 50; //maximum level is 50
        stats.playerLevel = playerLevel;
    }

    void updateGameStats() {
        stats.totalShots += shotCount;

        //update total hits based on current game hits
        stats.totalHits += playerHits;

        //recalculate accuracy based on total hits and shots
        if (stats.totalShots > 0) {
            stats.avgAccuracy = ((float)stats.totalHits / stats.totalShots) * 100.0f;
        }

        int gameLength = (int)gameTimer;

        if (stats.shortestGame == 0 || (gameLength > 0 && gameLength < stats.shortestGame)) {
            stats.shortestGame = gameLength;
        }

        if (gameLength > stats.longestGame) {
            stats.longestGame = gameLength;
        }

        calculatePlayerLevel();

        saveStats();
    }

    void loadStats() {
        ifstream file("battleship_stats.bin", ios::binary);
        if (!file) {
            throw runtime_error("No file");
        }
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(&stats), sizeof(GameStats));
            file.close();

            if (stats.playerLevel < 1) stats.playerLevel = 1;
            if (stats.experience < 0) stats.experience = 0;

            playerLevel = stats.playerLevel;

            calculatePlayerLevel();

            cout << "Stats loaded from battleship_stats.bin successfully!" << endl;
        }
        else {
            stats = GameStats();
            stats.shortestGame = 0;
            stats.longestGame = 0;
            stats.avgAccuracy = 0.0f;
            stats.playerLevel = 1;
            stats.experience = 0;
            playerLevel = 1;

            cout << "No existing stats file found. Creating new stats..." << endl;
            saveStats(); // Create initial .bin file
        }
    }

    void saveStats() {
        stats.playerLevel = playerLevel;
        stats.experience = stats.gamesPlayed + (stats.playerWins * 2);

        ofstream file("battleship_stats.bin", ios::binary);
        if (!file) {
            throw runtime_error("No file");
        }
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(&stats), sizeof(GameStats));
            file.close();
            cout << "Stats saved to battleship_stats.bin successfully!" << endl;
        }
        else {
            cout << "Error: Could not save stats to battleship_stats.bin!" << endl;
        }
    }

    void resetStats() {
        stats = GameStats();
        stats.shortestGame = 0;
        stats.longestGame = 0;
        stats.avgAccuracy = 0.0f;
        stats.playerLevel = 1;
        stats.experience = 0;
        playerLevel = 1;
        saveStats();
    }

    void update() {
        float deltaTime = GetFrameTime();
        particleSystem.update(deltaTime);
        gameTimer += deltaTime;

        switch (state) {
        case GameState::MAIN_MENU:
            updateMainMenu();
            break;
        case GameState::SHIP_PLACEMENT:
            updateShipPlacement();
            break;
        case GameState::PLAYER1_TURN:
        case GameState::PLAYER2_TURN:
            updatePlayerTurn();
            break;
        case GameState::AI_TURN:
            updateAITurn(deltaTime);
            break;
        case GameState::GAME_OVER:
            updateGameOver();
            break;
        case GameState::SETTINGS:
            updateSettings();
            break;
        }

        if (state == GameState::PLAYER1_TURN || state == GameState::PLAYER2_TURN || state == GameState::AI_TURN) {
            if (playerBoard.allShipsSunk()) {
                winner = (gameMode == GameMode::PVP) ? 2 : 0; // AI or Player 2 wins
                state = GameState::GAME_OVER;
                stats.gamesPlayed++;

                if (gameMode == GameMode::PVP) {
                    //in player vs player mode player 2 wins
                }
                else {
                    stats.aiWins++;
                }
                updateGameStats();
            }
            else if (computerBoard.allShipsSunk()) {
                winner = 1; //player 1 wins
                state = GameState::GAME_OVER;
                stats.gamesPlayed++;
                stats.playerWins++; //player wins
                updateGameStats();
            }
        }
    }

    void updateMainMenu() {
        Vector2 mousePos = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePos, playButton)) {
                gameMode = (GameMode)selectedDifficulty;
                startNewGame();
            }
            else if (CheckCollisionPointRec(mousePos, pvpButton)) {
                gameMode = GameMode::PVP;
                startNewGame();
            }
            else if (CheckCollisionPointRec(mousePos, settingsButton)) {
                state = GameState::SETTINGS;
            }
            else if (CheckCollisionPointRec(mousePos, quitButton)) {

            }
        }

        if (IsKeyPressed(KEY_UP)) selectedMenuItem = max(0, selectedMenuItem - 1);
        if (IsKeyPressed(KEY_DOWN)) selectedMenuItem = min(4, selectedMenuItem + 1);
        if (IsKeyPressed(KEY_ENTER)) {
            switch (selectedMenuItem) {
            case 0: gameMode = (GameMode)selectedDifficulty; startNewGame(); break;
            case 1: gameMode = GameMode::PVP; startNewGame(); break;
            case 2: state = GameState::SETTINGS; break;
            case 3: state = GameState::STATISTICS; break;
            case 4: break;
            }
        }
    }

    void updateShipPlacement() {
        if (currentShipIndex >= shipsToPlace.size()) {
            state = (gameMode == GameMode::PVP) ? GameState::PLAYER1_TURN : GameState::PLAYER1_TURN;
            isPlacingShips = false;
            return;
        }

        if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_SPACE)) {
            placementMode = (placementMode == PlacementMode::HORIZONTAL) ?
                PlacementMode::VERTICAL : PlacementMode::HORIZONTAL;
        }

        if (IsKeyPressed(KEY_A)) {
            while (currentShipIndex < shipsToPlace.size()) {
                Ship* ship = shipsToPlace[currentShipIndex];
                if (playerBoard.placeShipAutomatically(new Ship(*ship))) {
                    currentShipIndex++;
                }
                else {
                    break;
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            int gridX = (int)((mousePos.x - BOARD_OFFSET_X) / CELL_SIZE);
            int gridY = (int)((mousePos.y - BOARD_OFFSET_Y) / CELL_SIZE);

            if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
                Ship* ship = shipsToPlace[currentShipIndex];
                bool isVertical = (placementMode == PlacementMode::VERTICAL);

                Ship* newShip = createShipCopy(ship);
                if (playerBoard.placeShip(newShip, gridX, gridY, isVertical)) {
                    currentShipIndex++;
                }
            }
        }
    }

    Ship* createShipCopy(Ship* original) {
        switch (original->getType()) {
        case ShipType::CARRIER: return new Carrier();
        case ShipType::BATTLESHIP: return new Battleship();
        case ShipType::CRUISER: return new Cruiser();
        case ShipType::SUBMARINE: return new Submarine();
        case ShipType::DESTROYER: return new Destroyer();
        default: return new Destroyer();
        }
    }

    void updatePlayerTurn() {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            int gridX = (int)((mousePos.x - (BOARD_OFFSET_X + BOARD_SPACING)) / CELL_SIZE);
            int gridY = (int)((mousePos.y - BOARD_OFFSET_Y) / CELL_SIZE);

            if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
                Board* targetBoard = (gameMode == GameMode::PVP && state == GameState::PLAYER2_TURN) ?
                    &playerBoard : &computerBoard;

                if (targetBoard->attack(gridX, gridY)) {
                    shotCount++;

                    //check if it was a hit and update player hits
                    if (targetBoard->getCellState(gridX, gridY) == CellState::HIT) {
                        playerHits++;
                        stats.totalHits++; //update total hits in stats
                    }

                    if (gameMode == GameMode::PVP) {
                        state = (state == GameState::PLAYER1_TURN) ?
                            GameState::PLAYER2_TURN : GameState::PLAYER1_TURN;
                        currentPlayer = (currentPlayer == 1) ? 2 : 1;
                    }
                    else {
                        if (ai && targetBoard->getCellState(gridX, gridY) == CellState::HIT) {
                            ai->registerHit(gridX, gridY, targetBoard->getGrid());
                        }
                        state = GameState::AI_TURN;
                        turnTimer = 0;
                    }
                }
            }
        }
    }

    void updateAITurn(float deltaTime) {
        turnTimer += deltaTime;
        if (turnTimer >= 1.0f) {
            auto target = ai->getNextTarget(playerBoard.getGrid());

            if (playerBoard.attack(target.first, target.second)) {
                shotCount++;
                if (playerBoard.getCellState(target.first, target.second) == CellState::HIT) {
                    ai->registerHit(target.first, target.second, playerBoard.getGrid());
                }
            }

            state = GameState::PLAYER1_TURN;
            turnTimer = 0;
        }
    }

    void updateGameOver() {
        if (IsKeyPressed(KEY_R)) {
            startNewGame();
        }
        else if (IsKeyPressed(KEY_M)) {
            state = GameState::MAIN_MENU;
        }
    }

    void updateSettings() {
        if (IsKeyPressed(KEY_BACKSPACE)) {
            state = GameState::MAIN_MENU;
        }

        if (IsKeyPressed(KEY_UP)) selectedDifficulty = max(0, selectedDifficulty - 1);
        if (IsKeyPressed(KEY_DOWN)) selectedDifficulty = min(2, selectedDifficulty + 1);
    }

    void updateStatistics() {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
            state = GameState::MAIN_MENU;
        }

        if (IsKeyPressed(KEY_R)) {
            resetStats();
        }
    }

    void draw() {
        ClearBackground(BLACK);

        switch (state) {
        case GameState::MAIN_MENU:
            drawMainMenu();
            break;
        case GameState::SHIP_PLACEMENT:
            drawShipPlacement();
            break;
        case GameState::PLAYER1_TURN:
        case GameState::PLAYER2_TURN:
        case GameState::AI_TURN:
            drawGameplay();
            break;
        case GameState::GAME_OVER:
            drawGameplay();
            drawGameOver();
            break;
        case GameState::SETTINGS:
            drawSettings();
            break;
        }

        particleSystem.draw();
    }

private:
    void drawMainMenu() {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float alpha = (float)i / SCREEN_HEIGHT;
            Color color = ColorLerp(DARKBLUE, MAROON, alpha);
            DrawLine(0, i, SCREEN_WIDTH, i, color);
        }

        float titleScale = 1.0f + 0.1f * sin(GetTime() * 2);
        DrawTextEx(GetFontDefault(), "BATTLESHIP",
            { SCREEN_WIDTH / 2 - 200, 100 }, 60 * titleScale, 2, GOLD);

        DrawText("ULTIMATE EDITION", SCREEN_WIDTH / 2 - 120, 170, 20, LIGHTGRAY);

        DrawText(TextFormat("PLAYER LEVEL: %d", playerLevel), 50, 50, 20, GOLD);

        Vector2 mousePos = GetMousePosition();

        drawMenuButton(playButton, "PLAY vs AI", selectedMenuItem == 0 ||
            CheckCollisionPointRec(mousePos, playButton));
        drawMenuButton(pvpButton, "PLAYER vs PLAYER", selectedMenuItem == 1 ||
            CheckCollisionPointRec(mousePos, pvpButton));
        drawMenuButton(settingsButton, "SETTINGS", selectedMenuItem == 2 ||
            CheckCollisionPointRec(mousePos, settingsButton));
        drawMenuButton(quitButton, "QUIT", selectedMenuItem == 3 ||
            CheckCollisionPointRec(mousePos, quitButton));

        DrawText("AI DIFFICULTY:", SCREEN_WIDTH / 2 - 80, 650, 20, WHITE);
        const char* difficulties[] = { "EASY", "MEDIUM", "HARD" };
        for (int i = 0; i < 3; i++) {
            Color color = (i == selectedDifficulty) ? YELLOW : LIGHTGRAY;
            DrawText(difficulties[i], SCREEN_WIDTH / 2 - 60 + i * 80, 680, 16, color);
        }

        DrawText("Use ARROW KEYS and ENTER to navigate", 50, SCREEN_HEIGHT - 60, 16, RAYWHITE);
        DrawText("Press Backspace to return to menu", 50, SCREEN_HEIGHT - 40, 16, RAYWHITE);
    }

    void drawMenuButton(Rectangle button, const char* text, bool highlighted) {
        Color buttonColor = highlighted ? DARKBLUE : ColorAlpha(DARKBLUE, 0.7f);
        Color textColor = highlighted ? YELLOW : WHITE;

        DrawRectangleRounded(button, 0.3f, 8, buttonColor);
        DrawRectangleRoundedLines(button, 0.3f, 8, textColor);

        int textWidth = MeasureText(text, 20);
        DrawText(text, (int)(button.x + button.width / 2 - textWidth / 2),
            (int)(button.y + button.height / 2 - 10), 20, textColor);
    }

    void drawShipPlacement() {
        ClearBackground(DARKBLUE);

        DrawText("SHIP PLACEMENT", SCREEN_WIDTH / 2 - 120, 50, 30, WHITE);

        if (currentShipIndex < shipsToPlace.size()) {
            Ship* currentShip = shipsToPlace[currentShipIndex];
            const char* shipNames[] = { "Destroyer", "Submarine", "Cruiser", "Battleship", "Carrier" };
            DrawText(TextFormat("Place your %s (Size: %d)",
                shipNames[(int)currentShip->getType()], currentShip->getSize()),
                50, 100, 20, WHITE);
        }

        playerBoard.draw(BOARD_OFFSET_X, false);

        if (currentShipIndex < shipsToPlace.size()) {
            Vector2 mousePos = GetMousePosition();
            int gridX = (int)((mousePos.x - BOARD_OFFSET_X) / CELL_SIZE);
            int gridY = (int)((mousePos.y - BOARD_OFFSET_Y) / CELL_SIZE);

            if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
                Ship* ship = shipsToPlace[currentShipIndex];
                bool isVertical = (placementMode == PlacementMode::VERTICAL);
                bool canPlace = playerBoard.canPlaceShip(gridX, gridY, ship->getSize(), isVertical);

                Color previewColor = canPlace ? ColorAlpha(GREEN, 0.5f) : ColorAlpha(RED, 0.5f);

                for (int i = 0; i < ship->getSize(); i++) {
                    int x = gridX + (isVertical ? 0 : i);
                    int y = gridY + (isVertical ? i : 0);

                    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                        Rectangle cell = {
                            (float)(BOARD_OFFSET_X + x * CELL_SIZE),
                            (float)(BOARD_OFFSET_Y + y * CELL_SIZE),
                            (float)(CELL_SIZE - 2),
                            (float)(CELL_SIZE - 2)
                        };
                        DrawRectangleRec(cell, previewColor);
                    }
                }
            }
        }

        DrawText("Left Click: Place Ship", 50, SCREEN_HEIGHT - 120, 16, RAYWHITE);
        DrawText("R/Space: Rotate Ship", 50, SCREEN_HEIGHT - 100, 16, RAYWHITE);
        DrawText("A: Auto-place remaining ships", 50, SCREEN_HEIGHT - 80, 16, RAYWHITE);

        const char* orientationText = (placementMode == PlacementMode::HORIZONTAL) ? "HORIZONTAL" : "VERTICAL";
        DrawText(TextFormat("Orientation: %s", orientationText), 50, SCREEN_HEIGHT - 60, 16, YELLOW);

        DrawText(TextFormat("Ships placed: %d/5", currentShipIndex), SCREEN_WIDTH - 200, 100, 16, RAYWHITE);
    }

    void drawGameplay() {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float alpha = (float)i / SCREEN_HEIGHT * 0.3f;
            Color color = ColorLerp(BLUE, BLACK, alpha);
            DrawLine(0, i, SCREEN_WIDTH, i, color);
        }

        DrawText("BATTLESHIP WARFARE", SCREEN_WIDTH / 2 - 150, 20, 30, GOLD);

        DrawText("YOUR FLEET", BOARD_OFFSET_X, BOARD_OFFSET_Y - 60, 20, WHITE);
        playerBoard.draw(BOARD_OFFSET_X, false);

        const char* enemyLabel = (gameMode == GameMode::PVP) ? "ENEMY FLEET" : "AI FLEET";
        DrawText(enemyLabel, BOARD_OFFSET_X + BOARD_SPACING, BOARD_OFFSET_Y - 60, 20, WHITE);
        computerBoard.draw(BOARD_OFFSET_X + BOARD_SPACING, true);

        drawGameInfo();

        if (state == GameState::PLAYER1_TURN) {
            const char* turnText = (gameMode == GameMode::PVP) ? "PLAYER 1 TURN" : "YOUR TURN";
            DrawText(turnText, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 60, 20, GREEN);
            DrawText("Click to attack!", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 40, 16, LIGHTGRAY);
        }
        else if (state == GameState::PLAYER2_TURN) {
            DrawText("PLAYER 2 TURN", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 60, 20, BLUE);
            DrawText("Click to attack!", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 40, 16, LIGHTGRAY);
        }
        else if (state == GameState::AI_TURN) {
            DrawText("AI THINKING...", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 60, 20, RED);

            int dots = (int)(GetTime() * 3) % 4;
            string thinkingText = "Computing";
            for (int i = 0; i < dots; i++) thinkingText += ".";
            DrawText(thinkingText.c_str(), SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT - 40, 16, LIGHTGRAY);
        }
    }

    void drawGameInfo() {
        Rectangle infoPanel = { SCREEN_WIDTH - 180, 280, 160, 320 };
        DrawRectangleRounded(infoPanel, 0.1f, 8, ColorAlpha(RED, 0.8f));
        DrawRectangleRoundedLines(infoPanel, 0.1f, 8, DARKGRAY);

        int yOffset = (int)infoPanel.y + 20;

        DrawText("GAME INFO", (int)infoPanel.x + 20, yOffset, 18, RAYWHITE);
        yOffset += 30;

        DrawText(TextFormat("Level: %d", playerLevel), (int)infoPanel.x + 20, yOffset, 14, GOLD);
        yOffset += 20;

        DrawText(TextFormat("Time: %d:%02d", (int)gameTimer / 60, (int)gameTimer % 60),
            (int)infoPanel.x + 20, yOffset, 14, LIGHTGRAY);
        yOffset += 20;

        DrawText(TextFormat("Shots: %d", shotCount),
            (int)infoPanel.x + 20, yOffset, 14, LIGHTGRAY);
        yOffset += 20;

        DrawText(TextFormat("Hits: %d", playerHits),
            (int)infoPanel.x + 20, yOffset, 14, LIGHTGRAY);
        yOffset += 20;

        if (shotCount > 0) {
            float currentAccuracy = ((float)playerHits / shotCount) * 100;
            DrawText(TextFormat("Accuracy: %.1f%%", currentAccuracy),
                (int)infoPanel.x + 20, yOffset, 14, LIGHTGRAY);
        }
        yOffset += 30;

        DrawText("CONTROLS:", (int)infoPanel.x + 20, yOffset, 16, YELLOW);
        yOffset += 25;
        DrawText("ESC - Menu", (int)infoPanel.x + 20, yOffset, 12, LIGHTGRAY);
        yOffset += 20;

        DrawText("FLEET STATUS:", (int)infoPanel.x + 20, yOffset, 16, YELLOW);
        yOffset += 25;

        const char* shipNames[] = { "Destroyer", "Submarine", "Cruiser", "Battleship", "Carrier" };
        for (int i = 0; i < 5; i++) {
            DrawText(TextFormat("%s: OK", shipNames[i]),
                (int)infoPanel.x + 20, yOffset, 10, GREEN);
            yOffset += 15;
        }
    }

    void drawGameOver() {
        Rectangle gameOverPanel = { SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 100, 400, 200 };
        DrawRectangleRounded(gameOverPanel, 0.2f, 8, ColorAlpha(BLACK, 0.9f));
        DrawRectangleRoundedLines(gameOverPanel, 0.2f, 8, GOLD);

        const char* winnerText;
        Color winnerColor;

        if (winner == 1) {
            winnerText = (gameMode == GameMode::PVP) ? "PLAYER 1 WINS!" : "VICTORY!";
            winnerColor = GREEN;
        }
        else {
            winnerText = (gameMode == GameMode::PVP) ? "PLAYER 2 WINS!" : "DEFEAT!";
            winnerColor = RED;
        }

        int textWidth = MeasureText(winnerText, 30);
        DrawText(winnerText, SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - 50, 30, winnerColor);

        DrawText(TextFormat("Game Time: %d:%02d", (int)gameTimer / 60, (int)gameTimer % 60),
            SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 10, 16, WHITE);
        DrawText(TextFormat("Total Shots: %d", shotCount),
            SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 10, 16, WHITE);

        DrawText("R - Play Again", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 40, 16, YELLOW);
        DrawText("M - Main Menu", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 60, 16, YELLOW);
    }

    void drawSettings() {
        ClearBackground(DARKBLUE);

        DrawText("SETTINGS", SCREEN_WIDTH / 2 - 80, 100, 30, WHITE);

        DrawText("Sound Effects: Enabled", 200, 200, 20, WHITE);

        DrawText("AI Difficulty:", 200, 250, 20, WHITE);
        const char* difficulties[] = { "EASY", "MEDIUM", "HARD" };
        for (int i = 0; i < 3; i++) {
            Color color = (i == selectedDifficulty) ? YELLOW : LIGHTGRAY;
            DrawText(difficulties[i], 400 + i * 100, 250, 20, color);
        }

        DrawText("Grid Size: 10x10 (Fixed)", 200, 300, 20, WHITE);
        DrawText("Ship Count: 5 (Fixed)", 200, 350, 20, WHITE);

        DrawText("Controls:", 200, 450, 20, YELLOW);
        DrawText("S - Toggle Sound", 220, 480, 16, WHITE);
        DrawText("UP/DOWN - Change Difficulty", 220, 500, 16, WHITE);
        DrawText("Backspace - Return to Menu", 220, 520, 16, WHITE);
    }

};

#endif
