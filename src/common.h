#pragma once

#define ENDPOINT_PLAYERS 0
#define ENDPOINT_ATTACKS 1
#define ENDPOINT_PLAYERCREATE 2
#define ENDPOINT_PLAYERDELETE 3
#define ENDPOINT_GAMECREATE 4
#define ENDPOINT_GAMEACTION 5
#define ENDPOINT_CHECKSTATUS 6

typedef struct {
	int index;
	char name[24];
	int id;
} MenuItem;

enum {
	KEY_ENDPOINT = 0x0,
	KEY_INDEX = 0x1,
	KEY_NAME = 0x2,
	KEY_ID = 0x3,
	KEY_SPARE = 0x4,
	KEY_ERROR = 0x5,
	KEY_GAME_ID = 0x6,
	KEY_VICTIM_ID = 0x7,
	KEY_ACTION_ID = 0x8
};

#define menu_layer_reload_data_and_mark_dirty(layer) menu_layer_reload_data(layer); layer_mark_dirty(menu_layer_get_layer(layer));