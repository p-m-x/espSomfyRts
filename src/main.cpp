#include <Arduino.h>
#include <SomfyRts.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Ticker.h>
#include <ArduinoJson.h>

#define STATUS_LED_PIN D1

#define REMOTE_FIRST_ADDR 0x121300
#define REMOTE_COUNT 10

SomfyRts rtsDevices[REMOTE_COUNT] = {
  SomfyRts(REMOTE_FIRST_ADDR),
  SomfyRts(REMOTE_FIRST_ADDR + 1),
  SomfyRts(REMOTE_FIRST_ADDR + 2),
  SomfyRts(REMOTE_FIRST_ADDR + 3),
  SomfyRts(REMOTE_FIRST_ADDR + 4),
  SomfyRts(REMOTE_FIRST_ADDR + 5),
  SomfyRts(REMOTE_FIRST_ADDR + 6),
  SomfyRts(REMOTE_FIRST_ADDR + 7),
  SomfyRts(REMOTE_FIRST_ADDR + 8),
  SomfyRts(REMOTE_FIRST_ADDR + 9)
};

WiFiManager wifiManager;

Ticker ticker;

ESP8266WebServer server(80);

unsigned char frame[7];

void wifiConfigModeCallback (WiFiManager *myWiFiManager);
void wifiConnect(void);
void tickLed(void);
void handleRemoteCmd();
void handleNotFound();

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nSomfy Rrs & Simu Hz WiFi Gate v0.0.1\n");
  Serial.println("Initialize remote devices");
  for (uint8_t i=0; i<REMOTE_COUNT; i++) {
    rtsDevices[i].init();
  }

  pinMode(STATUS_LED_PIN, OUTPUT);
  ticker.attach_ms(50, tickLed);

  wifiManager.setAPCallback(wifiConfigModeCallback);
  if (!wifiManager.autoConnect("SimuHz-Gateway")) {
    Serial.println("failed two connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  server.onNotFound(handleNotFound);
  server.on("/remote", HTTP_POST, handleRemoteCmd);
  server.begin();

  ticker.detach();
  digitalWrite(STATUS_LED_PIN, HIGH);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(STATUS_LED_PIN, HIGH);
  }
  else {
    digitalWrite(STATUS_LED_PIN, LOW);
    return;
  }

  server.handleClient();

}

void tickLed(void) {
  //toggle state
  int state = digitalRead(STATUS_LED_PIN);
  digitalWrite(STATUS_LED_PIN, !state);
}

void wifiConfigModeCallback (WiFiManager *myWiFiManager) {
  ticker.attach_ms(200, tickLed);
  Serial.println("Entered config mode");
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void handleRemoteCmd() {
  digitalWrite(STATUS_LED_PIN, LOW);

  if (server.hasArg("plain")) {
    // Use arduinojson.org/assistant to compute the capacity.
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
    if (root.success() && root.containsKey("remoteId") && root.containsKey("cmd")) {

      uint8_t remoteId = root["remoteId"].as<uint8_t>();
      if (remoteId > 0 && remoteId <= REMOTE_COUNT) {
        Serial.print("Remote ID: ");
        Serial.print(remoteId);
        Serial.print(", cmd: ");
        Serial.println(root["cmd"].as<char*>());

        const char* cmd = root["cmd"].as<char*>();
        if (strcmp(cmd, "UP") == 0) {
          rtsDevices[remoteId-1].sendCommandUp();
        } else if (strcmp(cmd, "DOWN") == 0) {
          rtsDevices[remoteId-1].sendCommandDown();
        } else if (strcmp(cmd, "STOP") == 0) {
          rtsDevices[remoteId-1].sendCommandStop();
        } else if (strcmp(cmd, "PROG") == 0) {
          rtsDevices[remoteId-1].sendCommandProg();
        } else {
          server.send(400, "text/plain", "400: Bad request");
          digitalWrite(STATUS_LED_PIN, HIGH);
          return;
        }

        server.send(200, "application/json", "{'status': 'OK'}");

        digitalWrite(STATUS_LED_PIN, HIGH);
        return;
      }
    }
  }
  digitalWrite(STATUS_LED_PIN, HIGH);
  server.send(400, "text/plain", "400: Bad request");
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}
