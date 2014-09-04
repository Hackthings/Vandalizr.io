#include <pebble.h>
#include "common.h"
#include "data.h"
#include "appmessage.h"
#include "player_list.h"
#include "attack_list.h"

	

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *s_player_list;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_fullscreen(s_window, true);
  
  // s_player_list
  s_player_list = menu_layer_create(GRect(0, 0, 144, 168));
  menu_layer_set_click_config_onto_window(s_player_list, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_player_list);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_player_list);
}
// END AUTO-GENERATED UI CODE

static void pl_appmessage_init(void);
static void pl_in_received_handler(DictionaryIterator *iter, void *context);
static void pl_refresh_list(uint16_t endpoint);

static MenuItem pl_menuitems[20]; //const max items

static int pl_num_menuitems;
static char pl_error[24];


static uint16_t pl_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 1;
}

static uint16_t pl_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return (pl_num_menuitems) ? pl_num_menuitems : 1;
}

static int16_t pl_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void pl_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	char row_text[20];
	strcpy(row_text, "SELECT VICTIM!");
	menu_cell_basic_header_draw(ctx, cell_layer, row_text);
}

void pl_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	if (strlen(pl_error) != 0) {
		menu_cell_basic_draw(ctx, cell_layer, "Error!", pl_error, NULL);
	} else if (pl_num_menuitems == 0) {
		menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
	} else {
		menu_cell_basic_draw(ctx, cell_layer, pl_menuitems[cell_index->row].name, NULL, NULL);
	}	
}

// Here we capture when a user selects a menu item
void pl_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Selected Victim: %i", (int)cell_index);	
	gamedata->victim_id = (int)cell_index->row;	
	show_attack_list();
}

static void handle_window_unload(Window* window) {
	destroy_ui();
}

void show_player_list(void) {
	initialise_ui();
	
	// Set all the callbacks for the menu layer
	menu_layer_set_callbacks(s_player_list, NULL, (MenuLayerCallbacks){
		.get_num_sections = pl_menu_get_num_sections_callback,
		.get_num_rows = pl_menu_get_num_rows_callback,
		.get_header_height = pl_menu_get_header_height_callback,
		.draw_header = pl_menu_draw_header_callback,
		.draw_row = pl_menu_draw_row_callback,
		.select_click = pl_menu_select_callback,
	});
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "pl - appmessage_init");
	pl_appmessage_init();
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "pl - refresh_list");
	pl_refresh_list(ENDPOINT_PLAYERS);

	window_set_window_handlers(s_window, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	window_stack_push(s_window, true);	
}

void hide_player_list(void) {
	window_stack_remove(s_window, true);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void pl_in_received_handler(DictionaryIterator *iter, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "pl - pl_in_received_handler");
	Tuple *index_tuple = dict_find(iter, KEY_INDEX);
	Tuple *name_tuple = dict_find(iter, KEY_NAME);
	Tuple *id_tuple = dict_find(iter, KEY_ID);
	Tuple *spare_tuple = dict_find(iter, KEY_SPARE);
	Tuple *error_tuple = dict_find(iter, KEY_ERROR);

	if (index_tuple && name_tuple && id_tuple) {
		MenuItem menuitem;
		menuitem.index = index_tuple->value->int16;
		strncpy(menuitem.name, name_tuple->value->cstring, sizeof(menuitem.name));
		menuitem.id = id_tuple->value->int16;
		pl_menuitems[menuitem.index] = menuitem;
		pl_num_menuitems++;
		menu_layer_reload_data_and_mark_dirty(s_player_list);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "received menuitem [%d] %s - %d", menuitem.index, menuitem.name, menuitem.id);
	}
	else if (name_tuple && !id_tuple) {
		strncpy(pl_error, name_tuple->value->cstring, sizeof(pl_error));
		menu_layer_reload_data_and_mark_dirty(s_player_list);
	}

}

static void pl_refresh_list(uint16_t endpoint) {
	memset(pl_menuitems, 0x0, sizeof(pl_menuitems));
	pl_num_menuitems = 0;
	pl_error[0] = '\0';
	menu_layer_set_selected_index(s_player_list, (MenuIndex) { .row = 0, .section = 0 }, MenuRowAlignBottom, false);
	menu_layer_reload_data_and_mark_dirty(s_player_list);
	request_data(endpoint);
}

	
static void pl_appmessage_init(void) {
	app_message_deregister_callbacks();
	app_message_register_inbox_received(pl_in_received_handler);
	appmessage_init();
}






