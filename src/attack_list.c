#include "attack_list.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *s_attack_list;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
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

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_attack_list(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_attack_list(void) {
  window_stack_remove(s_window, true);
}
