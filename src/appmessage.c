#include <pebble.h>
#include "appmessage.h"
#include "common.h"

static void in_dropped_handler(AppMessageResult reason, void *context);
static void out_sent_handler(DictionaryIterator *sent, void *context);
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
static bool AppMessageOpened = false;
	
static void in_dropped_handler(AppMessageResult reason, void *context) {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Incoming AppMessage from Pebble dropped, %d", reason);
}

static void out_sent_handler(DictionaryIterator *sent, void *context) {
	// outgoing message was delivered
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "OUT SENT");	
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send AppMessage to Pebble");
}

void appmessage_init(void) {
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	if(!AppMessageOpened) {
		app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());	
		AppMessageOpened = true;
	}
	
}


void request_data(uint16_t endpoint) {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "request_data: %u", endpoint);	
	Tuplet endpoint_tuple = TupletInteger(KEY_ENDPOINT, endpoint);

	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "request_data: RET %d", ret);	

	if (iter == NULL) {
		return;
	}

	dict_write_tuplet(iter, &endpoint_tuple);
	dict_write_end(iter);

	app_message_outbox_send(); 
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "request_data: RET2 %d", ret2);	
	
} 

void send_data(uint16_t endpoint, char game_id[35], char victim_id[35], uint8_t action_id) {

	Tuplet endpoint_tuple = TupletInteger(KEY_ENDPOINT, endpoint);
	Tuplet game_id_tuple = TupletCString(KEY_GAME_ID, game_id);
	Tuplet victim_id_tuple = TupletCString(KEY_VICTIM_ID, victim_id);
	Tuplet action_id_tuple = TupletInteger(KEY_ACTION_ID, action_id);

	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
		return;
	}

	dict_write_tuplet(iter, &endpoint_tuple);
	dict_write_tuplet(iter, &game_id_tuple);
	dict_write_tuplet(iter, &victim_id_tuple);
	dict_write_tuplet(iter, &action_id_tuple);
	dict_write_end(iter);

	app_message_outbox_send(); 
	
}