#include <pebble.h>
#include "clock.h"

	
void handle_init(void) {
  show_clock();
}

void handle_deinit(void) {
	hide_clock();	
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
