#include <Arduino.h>
#include <WiFi.h>
#include "State.h"
#include "config.h"
#include "Peripherals.h"
#include "Network.h"
#include "Telemetry.h"
#include "model.h"
#include "edge_ai.h"
#include "optimization.h"

void setup()
{
    // intialize serial monitor 
    Serial.begin(115200);
      // initialise sesnor
    dht.begin();
    // config esp32 with real time 
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    // config peripheral pns 
    pinMode(BTN_PLUGIN, INPUT_PULLUP);
    pinMode(BTN_PLUGOUT, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);


    // connect board to wifi 
    connectWiFi();

    
    // Configure MQTT server
    mqtt.setServer(MQTT_SERVER, MQTT_PORT); // MQTT SERVER ADDRESS OF THINGS AND POART NUMBER 


    // connect board to the cloud
    connectMQTT();  //TOKEN, DEVICE ID 
}

unsigned long now;
unsigned long last_print;

void loop()
{

// listen for incoming messages from the cloud
    mqtt.loop();
            
    //push  data every 5 sec
    now = millis();
    if((now - last_print) > 5000)
    {
        last_print = now;
        // read data from sensors
        // volatge , current , power , temperature , bay staus 
    
        sample_sensor();

       
        // run AI to get prediction
        runEdgeAIInference();
        // decide load based on prediction and current load
        runOptimization();
         // publish the data to the cloud
        publishTelemetry();
    }
    plug_status();
    update_led_status();


    
}

