#include "webApp.h"

void onBody(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  // Handle body
}

void onUpload(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
  // Handle upload
}

void onRequest(AsyncWebServerRequest* request) {
  if (request->method() == HTTP_OPTIONS) {
    request->send(200);
    return;
  }

  // Handle Unknown Request
  Serial.println("Not Found Serve Index page");
  AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

void webAppBegin(AsyncWebServer* server) {
  server->on("/", [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
  server->on("/index.js", [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "application/javascript", index_js_gz, index_js_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
  server->on("/index.css", [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "text/css", index_css_gz, index_css_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server->onNotFound(onRequest);
  server->onFileUpload(onUpload);
  server->onRequestBody(onBody);

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
}