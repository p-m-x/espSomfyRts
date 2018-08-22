#include <Arduino.h>
#include <SomfyRts.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Ticker.h>


#define PORT_TX D0
#define STATUS_LED_PIN D1

SomfyRts somfyRts(PORT_TX);

WiFiManager wifiManager;

Ticker ticker;

unsigned char frame[7];

void wifiConfigModeCallback (WiFiManager *myWiFiManager);
void wifiConnect(void);
void tickLed(void);

void setup() {
  Serial.begin(9600);
  pinMode(STATUS_LED_PIN, OUTPUT);
  ticker.attach_ms(50, tickLed);

  wifiManager.setAPCallback(wifiConfigModeCallback);

  if (!wifiManager.autoConnect("SimuHz-Gateway")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

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

    if (Serial.available() > 0) {
      digitalWrite(STATUS_LED_PIN, LOW);
      char serie = (char)Serial.read();
      Serial.println("");
  //    Serial.print("Remote : "); Serial.println(REMOTE, HEX);
      if(serie == 'm'||serie == 'u'||serie == 'h') {
        Serial.println("Monte"); // Somfy is a French company, after all.
        somfyRts.buildFrame(frame, HAUT);
      }
      else if(serie == 's') {
        Serial.println("Stop");
        somfyRts.buildFrame(frame, STOP);
      }
      else if(serie == 'b'||serie == 'd') {
        Serial.println("Descend");
        somfyRts.buildFrame(frame, BAS);
      }
      else if(serie == 'p') {
        Serial.println("Prog");
        somfyRts.buildFrame(frame, PROG);
      }
      else {
        Serial.println("Custom code");
        somfyRts.buildFrame(frame, serie);
      }

      Serial.println("");
      somfyRts.sendCommand(frame, 2);
      for(int i = 0; i<2; i++) {
        somfyRts.sendCommand(frame, 7);
      }
      digitalWrite(STATUS_LED_PIN, HIGH);
   }
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
