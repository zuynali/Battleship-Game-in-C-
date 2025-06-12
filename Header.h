#pragma once
#include"raylib.h"
#include"String.h"

const int screen_width = 1400;
const int screen_height = 800;
const int grid_size = 10;
const int cell_size = 45;
const int board_x = 100;
const int board_y = 150;
const int menu = 300;

enum class Cellstate { Empty, Ship, Hit, Miss, Skinking };
enum class Gamestate { MainMenu, Ship_placement, P1_turn, P2_turn, AI_turn, Game_Over, Settings, Stats };
enum class ShipType { Destroyer, Submarine, Cruiser, Battleship, Carrier };
enum class GameMode { P1vP2, PvAi_easy, PvAi_medium, PvAi_hard };


struct GamePlayed {
	int games_played;
	int player_win;
	int ai_wins;
	int total_shots;
	int total_hits;
};

struct Particle {
	Vector2 pos;
	Vector2 velocity;
	Color color;
	float size;
};

struct Animation {
	Vector2 pos;
	float time;
	Color color;
	String text;
};