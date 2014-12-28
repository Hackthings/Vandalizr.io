#include <pebble.h>
#include "clock.h"
#include "common.h"
#include "data.h"
#include "appmessage.h"	
#include "player_list.h"

static Window *s_window;

static GFont s_res_font_monkey_38;
static GFont s_res_font_monkey_24;

static TextLayer *s_time_layer;
static char time_text[] = "00:00:00";

static TextLayer *s_player_layer;

static GBitmap *arrow_image;
static BitmapLayer *arrow_layer;

static TextLayer *debug_layer;
static char debug_text[500];

static GBitmap *game_image;
static BitmapLayer *game_layer;

static void c_appmessage_init(void);
static void c_in_received_handler(DictionaryIterator *iter, void *context);
static void c_player_action(uint16_t endpoint);
static char c_error[24];

static bool unloading = false;

static AppTimer *timer = NULL;

static void initialise_ui(void) {
	//window
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
	//fonts
  s_res_font_monkey_38 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONKEY_38));
  s_res_font_monkey_24 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONKEY_24));
	
  //s_time_layer
  s_time_layer = text_layer_create(GRect(0, 15, 144, 40));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, s_res_font_monkey_38);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time_layer);
	
  //s_player_layer
  s_player_layer = text_layer_create(GRect(0, 117, 144, 40));
  text_layer_set_background_color(s_player_layer, GColorBlack);
  text_layer_set_text_color(s_player_layer, GColorWhite);
  text_layer_set_text(s_player_layer, "registering...");
  text_layer_set_text_alignment(s_player_layer, GTextAlignmentCenter);
  text_layer_set_font(s_player_layer, s_res_font_monkey_24);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_player_layer);
	
  //arrow_layer
  GRect framemask = (GRect) {
    .origin = { .x = 82, .y = 80 },
    .size = { .w = 54, .h = 18 }
  };
  arrow_layer = bitmap_layer_create(framemask);
  layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(arrow_layer));
  arrow_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ARROW);
  bitmap_layer_set_bitmap(arrow_layer, arrow_image);
	
  //debug_layer
	debug_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_background_color(s_player_layer, GColorBlack);
  text_layer_set_text_color(s_player_layer, GColorWhite);
	text_layer_set_text_alignment(debug_layer, GTextAlignmentLeft);
	text_layer_set_text(debug_layer, "Registering player!");
	text_layer_set_font(debug_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	layer_add_child(window_get_root_layer(s_window), (Layer *)debug_layer);
	layer_set_hidden(text_layer_get_layer(debug_layer), true);
	
  //game_layer
	game_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	layer_set_hidden(bitmap_layer_get_layer(game_layer), true);
  layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(game_layer));
  game_image = NULL;
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_res_font_monkey_38);
	fonts_unload_custom_font(s_res_font_monkey_24);
}




static void timer_callback(void *data) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ENDPOINT_CHECKSTATUS");
	c_player_action(ENDPOINT_CHECKSTATUS);
	timer = NULL;
  
	//are you mid attack
  //disable attack button 
  //change message "waiting victim response"

}

void game_status_timer(bool enable) {
	if(enable && timer == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "game_status_timer: Enabled");
    timer = app_timer_register(5000 /* milliseconds */, timer_callback, NULL);	
	}
	else {
		if(timer!=NULL) {
			APP_LOG(APP_LOG_LEVEL_DEBUG, "game_status_timer: Disabled");
			app_timer_cancel(timer);
			timer = NULL;
		}
	}
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
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Attack! SELECT button" );
	game_status_timer(false);
	show_player_list();	
}

void back_click_handler(ClickRecognizerRef recognizer, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Quit! BACK button" );
	unloading = true;
	c_player_action(ENDPOINT_PLAYERDELETE);
}

void button_config_provider(void *context) {
	if(strlen(gamedata->victim_id) == 0) {
		window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	}
	window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

static void handle_window_unload(Window* window) {
	destroy_ui();
}

void set_debug_info() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "set_debug_info()" );
	snprintf(debug_text, sizeof(debug_text), "u:%s\nv:%s\na:%s\ng:%s", gamedata->username, gamedata->victim_id, gamedata->attack_id, gamedata->game_id);
	text_layer_set_text(debug_layer, debug_text);
}

void show_clock(void) {
	
  c_appmessage_init();

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
	window_set_click_config_provider(s_window, button_config_provider);
  
	if(strlen(gamedata->victim_id) == 0) {
		//Register player
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ENDPOINT_PLAYERCREATE");
		c_player_action(ENDPOINT_PLAYERCREATE);
		
		//TODO: poll for game action (long poll)
	}
	else {
		//TODO: poll for game action (fast polling)
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Game in progress!");
		//set_debug_info();
	}

  APP_LOG(APP_LOG_LEVEL_DEBUG, "INIT FIRST TIMER!");
  game_status_timer(true);

}

void hide_clock(void) {
	window_stack_remove(s_window, true);
}


static void c_in_received_handler(DictionaryIterator *iter, void *context) {

	Tuple *index_tuple = dict_find(iter, KEY_INDEX);
	Tuple *name_tuple = dict_find(iter, KEY_NAME);
	Tuple *id_tuple = dict_find(iter, KEY_ID);
	Tuple *endpoint_tuple = dict_find(iter, KEY_ENDPOINT);

	if (index_tuple && name_tuple && id_tuple) {

		if(endpoint_tuple->value->int16 == ENDPOINT_CHECKSTATUS) {
			
			APP_LOG(APP_LOG_LEVEL_DEBUG, "endpoint: %d - received [index: %d] name:%d - id:%d", endpoint_tuple->value->int16, index_tuple->value->int16, name_tuple->value->int16, id_tuple->value->int16);
			if(name_tuple->value->int16 > 0) { 

				//game status
				APP_LOG(APP_LOG_LEVEL_DEBUG, "LOST!");
				
				game_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ATTACK_6);
				bitmap_layer_set_bitmap(game_layer, game_image);
				layer_set_hidden(bitmap_layer_get_layer(game_layer), false);
				
				game_status_timer(false);
				
			}
			else {
				//game status
				APP_LOG(APP_LOG_LEVEL_DEBUG, "WAIT!");
				game_status_timer(true);
			}
		}
		else {
				APP_LOG(APP_LOG_LEVEL_DEBUG, "CLOCK endpoint: %d - received [index: %d] name:%s - id:%d", endpoint_tuple->value->int16, index_tuple->value->int16, name_tuple->value->cstring, id_tuple->value->int16);
			
				strcpy(gamedata->username, name_tuple->value->cstring);
				text_layer_set_text(s_player_layer, name_tuple->value->cstring);
				set_debug_info();
		}
	}
	else if (name_tuple && !id_tuple) {
		strncpy(c_error, name_tuple->value->cstring, sizeof(c_error));
	}
	
	if(unloading) {
		game_status_timer(false);
		hide_clock();
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



