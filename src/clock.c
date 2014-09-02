#include "clock.h"
#include <pebble.h>
#include "player_list.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_font_monkey_38;
static TextLayer *s_time_layer;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
  s_res_font_monkey_38 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONKEY_38));
  // s_time_layer
  s_time_layer = text_layer_create(GRect(0, 67, 144, 40));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, s_res_font_monkey_38);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_res_font_monkey_38);
}
// END AUTO-GENERATED UI CODE

static char time_text[] = "00:00:00";

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



void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	show_player_list();
}

void select_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_clock(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
	
  window_set_click_config_provider(s_window, select_config_provider);
	
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  

  handle_tick(tick_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

void hide_clock(void) {
  window_stack_remove(s_window, true);
}
