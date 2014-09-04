#pragma once

#define ENDPOINT_PLAYERS 0
#define ENDPOINT_ATTACKS 1
#define ENDPOINT_TBC1 2
#define ENDPOINT_TBC2 3

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
};

#define menu_layer_reload_data_and_mark_dirty(layer) menu_layer_reload_data(layer); layer_mark_dirty(menu_layer_get_layer(layer));