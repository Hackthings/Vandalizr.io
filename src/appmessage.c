#include <pebble.h>
#include "appmessage.h"
#include "common.h"
//include "windows/storylist.h"

static void in_dropped_handler(AppMessageResult reason, void *context);
static void out_sent_handler(DictionaryIterator *sent, void *context);
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
static bool AppMessageOpened = false;
	
static void in_dropped_handler(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Incoming AppMessage from Pebble dropped, %d", reason);
}

static void out_sent_handler(DictionaryIterator *sent, void *context) {
	// outgoing message was delivered
	APP_LOG(APP_LOG_LEVEL_DEBUG, "OUT SENT");	
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send AppMessage to Pebble");
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
	APP_LOG(APP_LOG_LEVEL_DEBUG, "request_data: %u", endpoint);	
	Tuplet endpoint_tuple = TupletInteger(KEY_ENDPOINT, endpoint);

	DictionaryIterator *iter;
	int ret = app_message_outbox_begin(&iter);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "request_data: RET %d", ret);	

	if (iter == NULL) {
		return;
	}

	dict_write_tuplet(iter, &endpoint_tuple);
	dict_write_end(iter);

	int ret2 = app_message_outbox_send(); 
	APP_LOG(APP_LOG_LEVEL_DEBUG, "request_data: RET2 %d", ret2);	
	
}