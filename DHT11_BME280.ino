// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// Depends on the following Arduino libraries:
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>
#include "cactus_io_BME280_I2C.h"
#include <MemoryFree.h>

BME280_I2C bme(0x76); // I2C using address 0x76

// Uncomment the type of sensor in use:
//#define DHTTYPE           DHT11     // DHT 11 
//#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

const int maxDHT = 3;
DHT_Unified dht[maxDHT] = {
  DHT_Unified(7,DHT11,6,1,1),
  DHT_Unified(8,DHT11,6,2,2),
  DHT_Unified(9,DHT11,6,3,3)
};

uint32_t delayMS;
const char stemp[] = "Temperature";
const char shumi[] = "Humidity";

void setup() {
  Serial.begin(9600); 
  // Initialize device.

  DHT_Unified *p;
  p = dht;
  sensor_t sensor;
  for (int i=0; i<(maxDHT); i++) {
    p->begin();
    // Print temperature sensor details.
    p->temperature().getSensor(&sensor);
    printSpec(&sensor,stemp);
    // Print humidity sensor details.
    p->humidity().getSensor(&sensor);
    printSpec(&sensor,shumi);
    p++;
  }

  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 500;

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  bme.setTempCal(-1); // Temp was reading high so subtract 1 degree
}

void printSpec(sensor_t* sensor, const char *s) {
  Serial.println("------------------------------------");
  Serial.println(s);
  Serial.print  ("Sensor:       "); Serial.println(sensor->name);
  //Serial.print  ("Driver Ver:   "); Serial.println(sensor->version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor->sensor_id);
  //Serial.print  ("Max Value:    "); Serial.print(sensor->max_value);  Serial.println(" *C");
  //Serial.print  ("Min Value:    "); Serial.print(sensor->min_value);  Serial.println(" *C");
  //Serial.print  ("Resolution:   "); Serial.print(sensor->resolution); Serial.println(" *C");  
  //Serial.println("------------------------------------");
}

void printTempEvent(const sensors_event_t* event) {
  if (isnan(event->temperature)) {
    Serial.print(event->sensor_id);
    Serial.print(" Error reading temperature! ");
  } else {
    Serial.print("DHT11(");
    Serial.print(event->sensor_id);
    Serial.print(")\t");
    Serial.print(event->temperature);
    Serial.print(" *C\t");
  }
}

void printHumiEvent(const sensors_event_t* event) {
  if (isnan(event->relative_humidity)) {
    Serial.println("Error reading humidity!");
  } else {
    Serial.print(event->relative_humidity);
    Serial.println(" %");
  }
}

// BME280
void bmePrintValue() {
  Serial.println("Device\t\tTemp\t\tHumdity\t\tPressure");
  bme.readSensor(); 
  Serial.print("BME280\t\t");
  Serial.print(bme.getTemperature_C()), Serial.print(" *C\t");
  Serial.print(bme.getHumidity()),      Serial.print(" %\t\t");
  Serial.print(bme.getPressure_MB()),   Serial.println(" mb"); // Pressure in millibars
}


void loop() {
  // Delay between measurements.
  delay(delayMS);
  Serial.println("------------------------------------");
  
  // Get temperature event and print its value.
  sensors_event_t event;
  
  bmePrintValue();
  
  DHT_Unified *p;
  p = dht;
  for (int i=0; i<(maxDHT); i++) {
    p->temperature().getEvent(&event);
    printTempEvent(&event);
    p->humidity().getEvent(&event);
    printHumiEvent(&event);
    p++;
  }

  Serial.print("freeMemory=");
  Serial.println(freeMemory());
}
