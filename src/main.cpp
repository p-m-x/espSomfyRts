#include <Arduino.h>
#include <EEPROM.h>
#include <SomfyRts.h>

#define PORT_TX D0

SomfyRts somfyRts;

unsigned char frame[7];

void setup() {
    Serial.begin(9600);

    Serial.println("Somfy Rrs & Simu Hz WiFi Gate v0.0.1");
    Serial.println("Serial port commands:");
    Serial.println("u - move up");
    Serial.println("d - move down");
    Serial.println("s - stop");
    Serial.println("p - programing mode");

    somfyRts.init(PORT_TX);

}

void loop() {
    if (Serial.available() > 0) {
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

   }

}
