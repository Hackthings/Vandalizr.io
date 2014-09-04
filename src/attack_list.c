#include <pebble.h>
#include "common.h"
#include "data.h"
#include "appmessage.h"
#include "attack_list.h"
	
// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *s_attack_list;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_fullscreen(s_window, true);
  
  // s_attack_list
  s_attack_list = menu_layer_create(GRect(0, 0, 144, 168));
  menu_layer_set_click_config_onto_window(s_attack_list, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_attack_list);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_attack_list);
}
// END AUTO-GENERATED UI CODE

static void al_appmessage_init(void);
static void al_in_received_handler(DictionaryIterator *iter, void *context);
static void al_refresh_list(uint16_t endpoint);

static MenuItem al_menuitems[20]; //const max items

static int al_num_menuitems;
static char al_error[24];

static uint16_t al_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 1;
}

static uint16_t al_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return (al_num_menuitems) ? al_num_menuitems : 1;
}

static int16_t al_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void al_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	char row_text[20];
	strcpy(row_text, "SELECT ATTACK!");
	menu_cell_basic_header_draw(ctx, cell_layer, row_text);
}

void al_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	if (strlen(al_error) != 0) {
		menu_cell_basic_draw(ctx, cell_layer, "Error!", al_error, NULL);
	} else if (al_num_menuitems == 0) {
		menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
	} else {
		menu_cell_basic_draw(ctx, cell_layer, al_menuitems[cell_index->row].name, NULL, NULL);
	}
}

// Here we capture when a user selects a menu item
void al_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

}


static void handle_window_unload(Window* window) {
	destroy_ui();
}

void show_attack_list(void) {
	initialise_ui();
		
	// Set all the callbacks for the menu layer
	menu_layer_set_callbacks(s_attack_list, NULL, (MenuLayerCallbacks){
		.get_num_sections = al_menu_get_num_sections_callback,
		.get_num_rows = al_menu_get_num_rows_callback,
		.get_header_height = al_menu_get_header_height_callback,
		.draw_header = al_menu_draw_header_callback,
		.draw_row = al_menu_draw_row_callback,
		.select_click = al_menu_select_callback,
	});
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "al - appmessage_init");
	al_appmessage_init();
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "al - refresh_list");
	al_refresh_list(ENDPOINT_ATTACKS);
	
	window_set_window_handlers(s_window, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	window_stack_push(s_window, true);
	
	
}

void hide_attack_list(void) {
	window_stack_remove(s_window, true);
}
			
			
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void al_in_received_handler(DictionaryIterator *iter, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "al - al_in_received_handler");
	Tuple *index_tuple = dict_find(iter, KEY_INDEX);
	Tuple *name_tuple = dict_find(iter, KEY_NAME);
	Tuple *id_tuple = dict_find(iter, KEY_ID);
	Tuple *spare_tuple = dict_find(iter, KEY_SPARE);
	Tuple *error_tuple = dict_find(iter, KEY_ERROR);
	
	if(error_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "al - al_in_received_handler: %s", error_tuple->value->cstring);	
	}
	

	if (index_tuple && name_tuple && id_tuple) {
		MenuItem menuitem;
		menuitem.index = index_tuple->value->int16;
		strncpy(menuitem.name, name_tuple->value->cstring, sizeof(menuitem.name));
		menuitem.id = id_tuple->value->int16;
		al_menuitems[menuitem.index] = menuitem;
		al_num_menuitems++;
		menu_layer_reload_data_and_mark_dirty(s_attack_list);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "received menuitem [%d] %s - %d", menuitem.index, menuitem.name, menuitem.id);
	}
	else if (name_tuple && !id_tuple) {
		strncpy(al_error, name_tuple->value->cstring, sizeof(al_error));
		menu_layer_reload_data_and_mark_dirty(s_attack_list);
	}

}

static void al_refresh_list(uint16_t endpoint) {
	memset(al_menuitems, 0x0, sizeof(al_menuitems));
	al_num_menuitems = 0;
	al_error[0] = '\0';
	menu_layer_set_selected_index(s_attack_list, (MenuIndex) { .row = 0, .section = 0 }, MenuRowAlignBottom, false);
	menu_layer_reload_data_and_mark_dirty(s_attack_list);
	request_data(endpoint);
}

	
static void al_appmessage_init(void) {
	app_message_deregister_callbacks();
	app_message_register_inbox_received(al_in_received_handler);
	appmessage_init();
}

