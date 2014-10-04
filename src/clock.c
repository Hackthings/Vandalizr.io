#include <pebble.h>
#include "clock.h"
#include "common.h"
#include "data.h"
#include "appmessage.h"	
#include "player_list.h"
//#include "netimage.h"

static Window *s_window;
static GFont s_res_font_monkey_38;
static TextLayer *s_time_layer;
static char time_text[] = "00:00:00";

static void c_appmessage_init(void);
static void c_in_received_handler(DictionaryIterator *iter, void *context);
static void c_player_action(uint16_t endpoint);
static char c_error[24];

/*
static BitmapLayer *bitmap_layer;
static NetImage *current_image;


void image_loaded_handler(NetImage *image) {
  bitmap_layer_set_bitmap(bitmap_layer, image->bmp);
  // Free the memory used by the previous image
  netimage_destroy(current_image);
  // Keep a pointer to this image data so we can free it later.
  current_image = image;
}
*/

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
	
  Layer *window_layer = window_get_root_layer(s_window);
  //GRect bounds = layer_get_bounds(window_layer);
	
  c_appmessage_init();
  
  s_res_font_monkey_38 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONKEY_38));
	
  // s_time_layer
  s_time_layer = text_layer_create(GRect(0, 67, 144, 40));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, s_res_font_monkey_38);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time_layer);
	
  // bitmap_layer
 /* bitmap_layer = bitmap_layer_create(bounds);
  layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
  current_image = NULL;
	
  // Need to initialize this first to make sure it is there when
  // the window_load function is called by window_stack_push.
  netimage_initialize(image_loaded_handler);
  */
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_res_font_monkey_38);

  /*
  bitmap_layer_destroy(bitmap_layer);
  netimage_destroy(current_image);
	
  netimage_deinitialize(); // call this to avoid 20B memory leak
  */
}


static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	if(clock_is_24h_style()) {
		strftime(time_text, sizeof(time_text), "%H:%M:%S", tick_time);
	}
	else {
		strftime(time_text, sizeof(time_text), "%I:%M:%S", tick_time);	
		if (time_text[0] == '0') {
			memmove(&time_text[0], &time_text[1], sizeof(time_text) - 1); //remove leading zero
		}
	}    
	text_layer_set_text(s_time_layer, time_text);
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	show_player_list();
	
	//netimage_request("http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-js/joseph.png.pbi");
}

void select_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void handle_window_unload(Window* window) {
	//Deregister player
	c_player_action(ENDPOINT_PLAYERDELETE);
	
	destroy_ui();
}

void foo(void* data) {
	
	c_player_action(ENDPOINT_PLAYERCREATE);
}

void show_clock(void) {
	initialise_ui();
	window_set_window_handlers(s_window, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	window_stack_push(s_window, true);

	//Somebody set us up the CLOCK
	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);  

	handle_tick(tick_time, SECOND_UNIT);
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
	
	//Setup the SELECT button
	window_set_click_config_provider(s_window, select_config_provider);

	app_timer_register(1000, foo, NULL);
	
	//Register player
	
}

void hide_clock(void) {
	window_stack_remove(s_window, true);
}


static void c_in_received_handler(DictionaryIterator *iter, void *context) {

	Tuple *index_tuple = dict_find(iter, KEY_INDEX);
	Tuple *name_tuple = dict_find(iter, KEY_NAME);
	Tuple *id_tuple = dict_find(iter, KEY_ID);
	//Tuple *spare_tuple = dict_find(iter, KEY_SPARE);
	//Tuple *error_tuple = dict_find(iter, KEY_ERROR);

	if (index_tuple && name_tuple && id_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "received [%d] %s - %d", index_tuple->value->int16, name_tuple->value->cstring, id_tuple->value->int16);
	}
	else if (name_tuple && !id_tuple) {
		strncpy(c_error, name_tuple->value->cstring, sizeof(c_error));
		APP_LOG(APP_LOG_LEVEL_ERROR, "c_in_received_handler: %s", c_error);
	}

}

static void c_player_action(uint16_t endpoint) {
	request_data(endpoint);
}

static void c_appmessage_init(void) {
	app_message_deregister_callbacks();
	app_message_register_inbox_received(c_in_received_handler);
	appmessage_init();
}

