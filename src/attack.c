#include <pebble.h>
#include "common.h"
#include "data.h"
#include "appmessage.h"
#include "attack.h"	

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *s_attack;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_fullscreen(s_window, true);
  
  // s_attack
  s_attack = menu_layer_create(GRect(0, 0, 144, 168));
  menu_layer_set_click_config_onto_window(s_attack, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_attack);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_attack);
}
// END AUTO-GENERATED UI CODE



static uint16_t a_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 1;
}

static uint16_t a_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return 3;
}

static int16_t a_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void a_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	char row_text[20];
	strcpy(row_text, "ATTACK!");
	menu_cell_basic_header_draw(ctx, cell_layer, row_text);
}

void a_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch(cell_index->row){
		case 0:
		   menu_cell_basic_draw(ctx, cell_layer, "ROCK", NULL, NULL);
		   break;
		case 1:
		   menu_cell_basic_draw(ctx, cell_layer, "PAPER", NULL, NULL);
		   break;
		case 2:
		   menu_cell_basic_draw(ctx, cell_layer, "SCISSORS", NULL, NULL);
		   break;
	}	
}

// Here we capture when a user selects a menu item
void a_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

}


static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_attack(void) {
	initialise_ui();
	
	// Set all the callbacks for the menu layer
	menu_layer_set_callbacks(s_attack, NULL, (MenuLayerCallbacks){
		.get_num_sections = a_menu_get_num_sections_callback,
		.get_num_rows = a_menu_get_num_rows_callback,
		.get_header_height = a_menu_get_header_height_callback,
		.draw_header = a_menu_draw_header_callback,
		.draw_row = a_menu_draw_row_callback,
		.select_click = a_menu_select_callback,
	});
	
	window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
});
  window_stack_push(s_window, true);
}

void hide_attack(void) {
  window_stack_remove(s_window, true);
}
