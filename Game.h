#pragma once
#include "Header.h"
#include "Board.h"
#include "AI.h"
#include "ParticleSystem.h"
#include "Vector.h"
#include"Battleship.h"
#include"Carrier.h"
#include"Cruiser.h"
#include"Destroyer.h"
#include"Submarine.h"
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

    Rectangle playButton = { SCREEN_WIDTH / 2 - 100, 300, 200, 50 };
    Rectangle pvpButton = { SCREEN_WIDTH / 2 - 100, 370, 200, 50 };
    Rectangle settingsButton = { SCREEN_WIDTH / 2 - 100, 440, 200, 50 };
    Rectangle statsButton = { SCREEN_WIDTH / 2 - 100, 510, 200, 50 };
    Rectangle quitButton = { SCREEN_WIDTH / 2 - 100, 580, 200, 50 };

public:
    Game() : playerBoard(&particleSystem), computerBoard(&particleSystem),
        state(GameState::MAIN_MENU), gameMode(GameMode::PVE_MEDIUM),
        placementMode(PlacementMode::HORIZONTAL), winner(0), currentPlayer(1),
        ai(nullptr), currentShipIndex(0), isPlacingShips(false),
        selectedMenuItem(0), selectedDifficulty(1), showStats(false),
        showSettings(false), turnTimer(0), gameTimer(0), shotCount(0) {

        loadStats();//this loads all the stats of the user from the log file
        initializeShipsToPlace();//initializes all the ships to be placed on board
    }

    ~Game() {
        saveStats();
        delete ai;//using auto because different ships have different data types
        for (auto ship : shipsToPlace) {
            delete ship;
        }
    }

    void initializeShipsToPlace() {//initializes all the ships to be placed on board
        shipsToPlace.clear();
        shipsToPlace.push_back(new Carrier());
        shipsToPlace.push_back(new Battleship());
        shipsToPlace.push_back(new Cruiser());
        shipsToPlace.push_back(new Submarine());
        shipsToPlace.push_back(new Destroyer());
    }

    void startNewGame() {//this reset all the stats of the user
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
        currentPlayer = 1;
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
        case GameState::STATISTICS:
            updateStatistics();
            break;
        }

        if (state == GameState::PLAYER1_TURN || state == GameState::PLAYER2_TURN || state == GameState::AI_TURN) {
            if (playerBoard.allShipsSunk()) {
                winner = (gameMode == GameMode::PVP) ? 2 : 0;
                state = GameState::GAME_OVER;
                stats.gamesPlayed++;
                if (gameMode != GameMode::PVP) stats.aiWins++;
                updateGameStats();
            }
            else if (computerBoard.allShipsSunk()) {
                winner = 1; // Player 1 wins
                state = GameState::GAME_OVER;
                stats.gamesPlayed++;
                stats.playerWins++;
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
            else if (CheckCollisionPointRec(mousePos, statsButton)) {
                state = GameState::STATISTICS;
            }
            else if (CheckCollisionPointRec(mousePos, quitButton)) {
                
            }
        }

        // Keyboard navigation
        if (IsKeyPressed(KEY_UP)) selectedMenuItem = max(0, selectedMenuItem - 1);
        if (IsKeyPressed(KEY_DOWN)) selectedMenuItem = min(4, selectedMenuItem + 1);
        if (IsKeyPressed(KEY_ENTER)) {
            switch (selectedMenuItem) {
            case 0: gameMode = (GameMode)selectedDifficulty; startNewGame(); break;
            case 1: gameMode = GameMode::PVP; startNewGame(); break;
            case 2: state = GameState::SETTINGS; break;
            case 3: state = GameState::STATISTICS; break;
            case 4: break; //game will end 
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
        if (turnTimer >= 1.0f) { // 1 second delay for AI move
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

    void updateGameStats() {
        stats.totalShots += shotCount;
        if (shotCount > 0) {
            stats.avgAccuracy = (float)stats.totalHits / stats.totalShots * 100;
        }

        int gameLength = (int)gameTimer;
        if (gameLength > stats.longestGame) stats.longestGame = gameLength;
        if (gameLength < stats.shortestGame) stats.shortestGame = gameLength;

        saveStats();
    }

    void loadStats() {
        ifstream file("battleship_stats.dat", ios::binary);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(&stats), sizeof(GameStats));
            file.close();
        }
    }

    void saveStats() {
        ofstream file("battleship_stats.dat", ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(&stats), sizeof(GameStats));
            file.close();
        }
    }

    void resetStats() {
        stats = GameStats();
        saveStats();
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
        case GameState::STATISTICS:
            drawStatistics();
            break;
        }

        particleSystem.draw();
    }

private:
    void drawMainMenu() {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float alpha = (float)i / SCREEN_HEIGHT;
            Color color = ColorLerp(DARKBLUE, MAROON, alpha);//for the gradient effect of the game
            DrawLine(0, i, SCREEN_WIDTH, i, color);
        }

        float titleScale = 1.0f + 0.1f * sin(GetTime() * 2);
        DrawTextEx(GetFontDefault(), "BATTLESHIP",
            { SCREEN_WIDTH / 2 - 200, 100 }, 60 * titleScale, 2, GOLD);

        DrawText("ULTIMATE EDITION", SCREEN_WIDTH / 2 - 120, 170, 20, LIGHTGRAY);

        Vector2 mousePos = GetMousePosition();

        drawMenuButton(playButton, "PLAY vs AI", selectedMenuItem == 0 ||
            CheckCollisionPointRec(mousePos, playButton));
        drawMenuButton(pvpButton, "PLAYER vs PLAYER", selectedMenuItem == 1 ||
            CheckCollisionPointRec(mousePos, pvpButton));
        drawMenuButton(settingsButton, "SETTINGS", selectedMenuItem == 2 ||
            CheckCollisionPointRec(mousePos, settingsButton));
        drawMenuButton(statsButton, "STATISTICS", selectedMenuItem == 3 ||
            CheckCollisionPointRec(mousePos, statsButton));
        drawMenuButton(quitButton, "QUIT", selectedMenuItem == 4 ||
            CheckCollisionPointRec(mousePos, quitButton));

        DrawText("AI DIFFICULTY:", SCREEN_WIDTH / 2 - 80, 650, 20, WHITE);
        const char* difficulties[] = { "EASY", "MEDIUM", "HARD" };
        for (int i = 0; i < 3; i++) {
            Color color = (i == selectedDifficulty) ? YELLOW : LIGHTGRAY;//the mode which is selected will have a yellow color else will have lightgray color
            DrawText(difficulties[i], SCREEN_WIDTH / 2 - 60 + i * 80, 680, 16, color);
        }

        DrawText("Use ARROW KEYS and ENTER to navigate", 50, SCREEN_HEIGHT - 60, 16, RAYWHITE);
        DrawText("Press ESC anytime to return to menu", 50, SCREEN_HEIGHT - 40, 16, RAYWHITE);//using raywhite for clarity in reading
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
            DrawText(TextFormat("Place your (Size:)",
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
        DrawText(TextFormat("Orientation:", orientationText), 50, SCREEN_HEIGHT - 60, 16, YELLOW);

        // Progress indicator
        DrawText(TextFormat("Ships placed: /5", currentShipIndex), SCREEN_WIDTH - 200, 100, 16, RAYWHITE);
    }

    void drawGameplay() {
        // Background
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

            // AI thinking animation
            int dots = (int)(GetTime() * 3) % 4;
            string thinkingText = "Computing";
            for (int i = 0; i < dots; i++) thinkingText += ".";
            DrawText(thinkingText.c_str(), SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT - 40, 16, LIGHTGRAY);
        }

    }

    void drawGameInfo() {
        Rectangle infoPanel = { SCREEN_WIDTH - 200, 180, 260, 300 };
        DrawRectangleRounded(infoPanel, 0.1f, 8, ColorAlpha(BLACK, 0.8f));
        DrawRectangleRoundedLines(infoPanel, 0.1f, 8, DARKGRAY);

        int yOffset = (int)infoPanel.y + 20;

        DrawText("GAME INFO", (int)infoPanel.x + 20, yOffset, 18, RAYWHITE);
        yOffset += 30;

        DrawText(TextFormat("Time:", (int)gameTimer / 60, (int)gameTimer % 60),
            (int)infoPanel.x + 20, yOffset, 14, LIGHTGRAY);
        yOffset += 20;

        DrawText(TextFormat("Shots:", shotCount),
            (int)infoPanel.x + 20, yOffset, 14, LIGHTGRAY);
        yOffset += 20;

        if (shotCount > 0) {
            float accuracy = ((float)stats.totalHits / shotCount) * 100;
            DrawText(TextFormat("Accuracy:", accuracy),
                (int)infoPanel.x + 20, yOffset, 14, LIGHTGRAY);
        }
        yOffset += 30;

        DrawText("CONTROLS:", (int)infoPanel.x + 20, yOffset, 16, YELLOW);
        yOffset += 25;
        DrawText("ESC - Menu", (int)infoPanel.x + 20, yOffset, 12, LIGHTGRAY);
        yOffset += 25;
        // Ship status
        DrawText("FLEET STATUS:", (int)infoPanel.x + 20, yOffset, 16, YELLOW);
        yOffset += 25;

        const char* shipNames[] = { "Destroyer", "Submarine", "Cruiser", "Battleship", "Carrier" };
        for (int i = 0; i < 5; i++) {
            DrawText(TextFormat("OK", shipNames[i]),
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

        DrawText(TextFormat("Game Time:", (int)gameTimer / 60, (int)gameTimer % 60),
            SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 10, 16, WHITE);
        DrawText(TextFormat("Total Shots:", shotCount),
            SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 10, 16, WHITE);

        DrawText("R - Play Again", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 40, 16, YELLOW);
        DrawText("M - Main Menu", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 60, 16, YELLOW);
    }

    void drawSettings() {
        ClearBackground(DARKBLUE);

        DrawText("SETTINGS", SCREEN_WIDTH / 2 - 80, 100, 30, WHITE);

        DrawText("Sound Effects:", 200, 200, 20, WHITE);
      
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
        DrawText("ESC - Return to Menu", 220, 520, 16, WHITE);
    }

    void drawStatistics() {
        ClearBackground(DARKBLUE);

        DrawText("STATISTICS", SCREEN_WIDTH / 2 - 100, 100, 30, WHITE);

        int yPos = 200;
        DrawText(TextFormat("Games Played:", stats.gamesPlayed), 200, yPos, 20, WHITE);
        yPos += 40;

        DrawText(TextFormat("Player Wins:", stats.playerWins), 200, yPos, 20, GREEN);
        yPos += 30;

        DrawText(TextFormat("AI Wins:", stats.aiWins), 200, yPos, 20, RED);
        yPos += 40;

        if (stats.gamesPlayed > 0) {
            float winRate = ((float)stats.playerWins / stats.gamesPlayed) * 100;
            DrawText(TextFormat("Win Rate:", winRate), 200, yPos, 20, YELLOW);
        }
        yPos += 40;

        DrawText(TextFormat("Total Shots:", stats.totalShots), 200, yPos, 20, WHITE);
        yPos += 30;

        DrawText(TextFormat("Average Accuracy: ", stats.avgAccuracy), 200, yPos, 20, WHITE);
        yPos += 40;

        if (stats.longestGame < 999) {
            DrawText(TextFormat("Longest Game:", stats.longestGame / 60, stats.longestGame % 60),
                200, yPos, 20, WHITE);
        }
        yPos += 30;

        if (stats.shortestGame < 999) {
            DrawText(TextFormat("Shortest Game:", stats.shortestGame / 60, stats.shortestGame % 60),
                200, yPos, 20, WHITE);
        }

        DrawText("R - Reset Statistics", 200, SCREEN_HEIGHT - 100, 16, YELLOW);
        DrawText("ESC - Return to Menu", 200, SCREEN_HEIGHT - 80, 16, WHITE);
    }
};

#endif
