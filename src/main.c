#include <pebble.h>
#include "clock.h"
	
Window *my_window;
TextLayer *text_layer;

void handle_init(void) {
  show_clock();
}

void handle_deinit(void) {
	
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
