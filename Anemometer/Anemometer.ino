#include <JeeLib.h>
#include <Ports.h>
#include <RF12.h>

int pin=5;

unsigned long previousWdtMillis, previousEthernetMillis, previousClicksMillis;
float windSpeed;
unsigned volatile int clicks;

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// Pin change interrupt control register - enables interrupt vectors
// Bit 2 = enable PC vector 2 (PCINT23..16)
// Bit 1 = enable PC vector 1 (PCINT14..8)
// Bit 0 = enable PC vector 0 (PCINT7..0)
ISR(PCINT2_vect) {
      //Serial.println("Tick...");
      clicks++;
}

void setup () {
    Serial.begin(57600);
    Serial.println("\n[Anemometer_irq]");
    rf12_initialize(26, RF12_868MHZ, 4);
    pinMode(pin, INPUT); //Make pin an input
    digitalWrite(pin,HIGH); //Enable pullup resistor on Analog Pin
    
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT21);
    //MCUCR = (1<<ISC01) | (1<<ISC01);
    clicks=0;
    previousClicksMillis = previousEthernetMillis = previousWdtMillis = millis();
}


void loop () {
 if (clicks >= 5) { 
     //Update RPM every 20 counts, increase this for better RPM resolution,
     updateValues();
     Serial.print("windSpeed= ");
     Serial.print(windSpeed);
     Serial.print("  m/s   -> ");
     Serial.print((windSpeed*3600)/1000);
     Serial.println("  km/h");
     Sleepy::loseSomeTime(10000);
   }
}


void updateValues(){ 
  unsigned long now = millis();
  float windCountTime = (now - previousClicksMillis) / 1000.0;  
  windSpeed = 1.1176 * ((float) (clicks/2) / windCountTime);
  clicks = 0;  
  previousClicksMillis = now;
}


