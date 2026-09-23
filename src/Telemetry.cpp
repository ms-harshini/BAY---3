#include <ArduinoJson.h>
#include "Telemetry.h"
#include "network.h"
#include "state.h"
#include "config.h"

// ---------------------------------------------------------------------
// FR-7 telemetry publish. Adds `overloadActive` (additive, beyond SRS
// 8.3's baseline schema) so the ThingsBoard Overcurrent alarm rule can
// be a one-line filter. Also adds `manualOverrideActive` so the
// dashboard can show when a bay is under operator control instead of
// automatic optimization.
// ---------------------------------------------------------------------
void publishTelemetry()
 {
  // check the device is connected to the cloud or not 
  if (!mqtt.connected()) return;
 
  // store the values as key value pair format 
  JsonDocument doc;
  doc["bayId"] = BAY_ID;
  doc["voltage"] = round(voltage * 10) / 10.0;
  doc["current"] = round(current * 10) / 10.0;
  doc["power"] = round(power * 10) / 10.0;
  doc["temperature"] = round(temperature * 10) / 10.0;
  doc["bayStatus"] = bayStatus;
  //adding 2 more vlaues to the buffer 
  doc["predictedArrivalProb"] = round(predictedArrivalProb * 100) / 100.0;
  doc["predictedDurationMin"] = predictedDurationMin;
  doc["lastHourOfDay"] = lastHourOfDay;
  //add load decision , overloadActive and throttle level to the buffer
  doc["loadDecision"] = loadDecision;
  doc["throttleLevel"] = throttleLevel;
  doc["overloadActive"] = overloadActive;

    char buffer[350];
  serializeJson(doc, buffer);

  // push the data to the cloud,topic,data ->buffer
 bool published = mqtt.publish("v1/devices/me/telemetry", buffer);

if (published) {
  Serial.println("[MQTT OK] Telemetry published");
} else {
  Serial.println("[MQTT ERROR] Telemetry publish failed");
}

  // readable debug print
  Serial.println("---- Telemetry ----");
  Serial.print("Bay ID:            "); Serial.println(doc["bayId"].as<String>());
  Serial.print("Voltage:           "); Serial.println(doc["voltage"].as<float>());
  Serial.print("Current:           "); Serial.println(doc["current"].as<float>());
  Serial.print("Power:             "); Serial.println(doc["power"].as<float>());
  Serial.print("Temperature:       "); Serial.println(doc["temperature"].as<float>());
  Serial.print("Bay Status:        "); Serial.println(doc["bayStatus"].as<String>());
  Serial.print("Arrival Prob:      "); Serial.println(doc["predictedArrivalProb"].as<float>());
  Serial.print("Duration (min):    "); Serial.println(doc["predictedDurationMin"].as<int>());
  Serial.print("Hour of Day:       "); Serial.println(doc["lastHourOfDay"].as<int>());
  Serial.print("Load Decision:     "); Serial.println(doc["loadDecision"].as<String>());
  Serial.print("Throttle Level:    "); Serial.println(doc["throttleLevel"].as<int>());
  Serial.print("Overload Active:   "); Serial.println(doc["overloadActive"].as<bool>());
  Serial.println("--------------------");
}
