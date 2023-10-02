#ifndef webApp_h
#define webApp_h

#include <ESPAsyncWebServer.h>

#include "favicon_png.h"
#include "index_css.h"
#include "index_html.h"
#include "index_js.h"

void webAppBegin(AsyncWebServer* server);

#endif