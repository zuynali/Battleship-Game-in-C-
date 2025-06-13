// Battleship_game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Game.h"
using namespace std;
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Battleship OOP Project with all the concepts: ");
    SetTargetFPS(60);//Frame rates of the screen 
    //Raylib function so that the screen doesn't closes quickly

    Game game;//intialiazing the game
    //from here the game will be started

    while (!WindowShouldClose()) {//game will be running until the window is closed raylib feature
        game.update();// this will update the game and get ready the game to player according to the players settings
        BeginDrawing();//raylib function to start the screen
        game.draw();//this function will start drawing on the screen 
        EndDrawing();//end the screen
    }

    CloseWindow();//Ends the program
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
