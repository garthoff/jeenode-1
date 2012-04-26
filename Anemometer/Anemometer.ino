#include <JeeLib.h>
#include <Ports.h>
#include <RF12.h>

//int pin=5;
#define pin (5)

unsigned long previousWdtMillis, previousEthernetMillis, previousClicksMillis;
float windSpeed;
unsigned volatile int clicks;

ISR(WDT_vect) { Sleepy::watchdogEvent();}

// Pin change interrupt control register - enables interrupt vectors
// Bit 2 = enable PC vector 2 (PCINT23..16)
// Bit 1 = enable PC vector 1 (PCINT14..8)
// Bit 0 = enable PC vector 0 (PCINT7..0)
ISR(PCINT2_vect) {
      //Serial.println("Tick...");
      clicks++;
}

typedef struct {   
    unsigned int time;
    float windSpeed;   
} WindData;


void setup () {
    Serial.begin(57600);
    Serial.println("\n[Anemometer_irq]");
    rf12_initialize(1, RF12_868MHZ, 33);
    pinMode(pin, INPUT); //Make pin an input
    digitalWrite(pin,HIGH); //Enable pullup resistor on Analog Pin
    
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT21);
    //MCUCR = (1<<ISC01) | (1<<ISC01);
    clicks=0;
    previousClicksMillis = previousEthernetMillis = previousWdtMillis = millis();
}


void loop () {
 if (clicks >= 10) { 
    
      //Update RPM every 20 counts, increase this for better RPM resolution,
      measureWind();
      WindData windData;
      windData.time = millis();
      windData.windSpeed = windSpeed;
      Serial.print("windSpeed= ");
      Serial.print(windData.windSpeed);
      Serial.print("  mph   -> ");
      Serial.print(windData.windSpeed*1.60934);
      Serial.print("  km/h   ");
      Serial.print((windData.windSpeed)*1.60934*1000/3600);
      Serial.println("  m/s");

      while (!rf12_canSend()){    // wait until sending is allowed
            rf12_recvDone();
      }

      rf12_sendStart(0, &windData, sizeof windData); // send the data

      while (!rf12_canSend()){    // wait until sending has been completed
            rf12_recvDone();
      }
     
     // go to sleep
     /* Serial.print("wait 5sec...");
      delay(5000);
      Serial.print("sleep...");
      delay(2000);
      Sleepy::loseSomeTime(30000);
      delay(1000);
      Serial.print("wakeup..");
      */
   }
}


static void measureWind(){ 
  unsigned long now = millis();
  float windCountTime = ((now - previousClicksMillis) / 1000.0); 
  Serial.print("Hz= "); 
  Serial.print(((float) (clicks/2) / windCountTime));
  Serial.print("   "); 
  windSpeed = (2.5 * ((float) (clicks/2) / windCountTime));
  clicks = 0;  
  previousClicksMillis = now;
}


