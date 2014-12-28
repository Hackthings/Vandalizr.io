#pragma once

void request_data(uint16_t endpoint);
void appmessage_init(void);
void send_data(uint16_t endpoint, char game_id[35], char victim_id[35], uint8_t action_id);
