#include <JeeLib.h>
#include <avr/sleep.h>

typedef struct {   
    unsigned int time;
    float windSpeed;   
} WindData;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup () {
  Serial.begin(57600);
  Serial.println("\n[Station_i]");
  rf12_initialize(1, RF12_868MHZ, 33);
}

void loop () {
  WindData windData;
  if (rf12_recvDone() && rf12_crc == 0 /*&& rf12_len == sizeof windData*/) {
    
    // process incoming data here
    memcpy(&windData, (byte*) rf12_data, sizeof windData);
    
    Serial.print("time= ");
    Serial.print(windData.time);
    Serial.print("  windSpeed= ");
    Serial.print(windData.windSpeed);
    Serial.print("  m/s   -> ");
    Serial.print((windData.windSpeed*3600)/1000);
    Serial.println("  km/h");
  }
}

