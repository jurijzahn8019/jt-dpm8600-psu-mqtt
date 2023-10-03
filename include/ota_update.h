#ifndef ota_update_h
#define ota_update_h

#include <ElegantOTA.h>

void ota_begin(AsyncWebServer* server);
void ota_loop();

#endif
