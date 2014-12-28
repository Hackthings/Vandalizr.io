#pragma once
struct GameData {
	char player_id[35];
	char victim_id[35];
	char attack_id[10];
	char game_id[35];
	char username[35];
} __attribute__((__packed__));
extern struct GameData* gamedata;
