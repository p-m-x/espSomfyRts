#include <Arduino.h>
#include <EEPROM.h>
#include <SomfyRts.h>

#define PORT_TX D0

SomfyRts somfyRts(PORT_TX);

unsigned char frame[7];

void setup() {
    Serial.begin(115200);
    while(!Serial) {
    }

}

void loop() {
    if (Serial.available() > 0) {
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
   }

}
