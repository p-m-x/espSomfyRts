#include <Arduino.h>
#include <SomfyRts.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Ticker.h>


#define PORT_TX D0
#define STATUS_LED_PIN D1

SomfyRts somfyRts;

WiFiManager wifiManager;

Ticker ticker;

unsigned char frame[7];

void wifiConfigModeCallback (WiFiManager *myWiFiManager);
void wifiConnect(void);
void tickLed(void);

void setup() {
  Serial.begin(9600);

  Serial.println("Somfy Rrs & Simu Hz WiFi Gate v0.0.1");
  Serial.println("Serial port commands:");
  Serial.println("u - move up");
  Serial.println("d - move down");
  Serial.println("s - stop");
  Serial.println("p - programing mode");

  somfyRts.init(PORT_TX);
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
        Serial.println("Move up"); // Somfy is a French company, after all.
        somfyRts.sendCommandUp();
      }
      else if(serie == 's') {
        Serial.println("Stop");
        somfyRts.sendCommandStop();
      }
      else if(serie == 'b'||serie == 'd') {
        Serial.println("Move down");
        somfyRts.sendCommandDown();
      }
      else if(serie == 'p') {
        Serial.println("Prog");
        somfyRts.sendCommandProg();
      }
      else {
        Serial.println("Custom code");
      }

      Serial.println("");
<<<<<<< HEAD

=======
      somfyRts.sendCommand(frame, 2);
      for(int i = 0; i<2; i++) {
        somfyRts.sendCommand(frame, 7);
      }
      digitalWrite(STATUS_LED_PIN, HIGH);
>>>>>>> fea6449aee4957c6ab21a21f55b10290ad9e2ae6
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
