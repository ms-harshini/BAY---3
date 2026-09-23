#include <Arduino.h>
#include <DHT.h>
#include "State.h"
#include "Peripherals.h"
#include "config.h"


DHT dht(DHT_PIN, DHT_TYPE);

float mapFloat(long x, long inMin, long inMax, float outMin, float outMax){
   return (x-inMin) * (outMax - outMin) / (float)(inMax - inMin) + outMin;
}


void sample_sensor(void)
{
    int raw_current = analogRead(CURRENT_PIN); // 0 to 4095   // 0 to 30A
    int raw_voltage = analogRead(VOLTAGE_PIN); // 0 to 4095   // 0 to 250v
    // map voltage 0 to 250 v
   voltage = mapFloat(raw_voltage, 0, 4095, 0, 250); // convert to voltage
   
   
   if(bayStatus == "CHARGING")
   {
   // map current 0 to 32 A
   current = mapFloat(raw_current, 0, 4095, 0, 32); // convert to current
   }


// read current and % values array 

   // calculate power 
   power = voltage * current; // calculate power in watts
    
    //to read temperature 
    float t = dht.readTemperature();
    if(!(isnan(t)))temperature = t;

    if (bayStatus == "CHARGING" && (current > 30.0 || temperature > 60.0))
    {
        bayStatus = "FAULT";
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("FAULT detected — relay disabled");
    }


    
    

}
 
float recentAvgCurrent()
{
    float sum=0;
    //read five current values and take average
    for(int i=0;i<5;i++)
    {
        sum = sum + current;
    }
    return sum/5;
}

bool plugin_flag_once = 1;
bool plugout_flag_once = 1;


void plug_status(void)
{
   bool pluginReading = digitalRead(BTN_PLUGIN);
   // detect the sw is pressed
   if(pluginReading == LOW && plugin_flag_once )
   {

    sessionStartMs = millis();  // record the start time of the charging session
   // plug in switch is pressed
   plugin_flag_once = 0;
   // change bay_status FREE to charging
   if (bayStatus == "FREE")
   {
      bayStatus = "CHARGING";
      Serial.println("Bay 3 plug in  is detected and Bay 3 is Charging ");
      // turn on relay to start charging
      digitalWrite(RELAY_PIN, HIGH);
   }

   //update leds
   }
   if(pluginReading == HIGH )
   {
      plugin_flag_once = 1;
   }


   // plug out switch is pressed


    bool plugoutReading = digitalRead(BTN_PLUGOUT);
   // detect the sw is pressed
   if(plugoutReading == LOW && plugout_flag_once )
   {
   // plug in switch is pressed
   plugout_flag_once = 0;
   // change bay_status  charging to FREE
   if (bayStatus == "CHARGING")
   {
      bayStatus = "FREE";
      Serial.println("Bay 3 plug out is detected and Bay 3 is Free ");
   }

   //update leds
   }
   if(plugoutReading == HIGH )
   {
      plugout_flag_once = 1;
   }
   
}
void update_led_status(void)
{
    if (bayStatus == "FREE")
    {
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, LOW);
    }
    else if (bayStatus == "CHARGING")
    {
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_RED, LOW);
    }
    else if (bayStatus == "FAULT")
    {
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, HIGH);
    }
    else
    {
        // unexpected/unhandled state — treat as fault to be safe
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, HIGH);
    }
}