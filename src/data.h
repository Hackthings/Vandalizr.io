#pragma once
struct GameData {
	int player_id;
	int victim_id;
	int attack_id;
} __attribute__((__packed__));

extern struct GameData* gamedata;