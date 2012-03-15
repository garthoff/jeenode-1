#include <JeeLib.h>
#include <avr/sleep.h>

typedef struct {   
    //unsigned int time;
    float windSpeed;   
} payload;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup () {
  Serial.begin(57600);
  Serial.println("\n[Station_i]");
  rf12_initialize(1, RF12_868MHZ, 33);
}

void loop () {
  if (rf12_recvDone() && rf12_crc == 0) {
    // process incoming data here
    Serial.print(" seq ");
    Serial.print(rf12_seq);
    Serial.print(" =");
    for (byte i = 0; i < rf12_len; ++i) {
      Serial.print(' ');
      Serial.print(rf12_data[i],DEC);
    }
    Serial.println();
  }
}

