//==================================
//=== Include relevant libraries ===
// =================================
#include <Wire.h>
#include "cactus_io_BME280_I2C.h"
#include <Adafruit_VEML6070.h>
#include "cactus_io_AM2315.h"

//======================================================
//=== Create three instances for each of the sensors ===
//======================================================
AM2315 am2315;
BME280_I2C bme(0x76);
Adafruit_VEML6070 uv = Adafruit_VEML6070();

//=======================================
//=== Initialize variables for timing ===
//=======================================
unsigned long currentMillis=0;
unsigned long previousMillis=0;
unsigned long interval=500;

//======================
//=== Setup function ===
//======================
void setup() {
  // put your setup code here, to run once:
  // Initialize the serial port.
  Serial.begin(115200);
  delay(2000);
  // Initialize the AM2315 sensor.
  if(!am2315.begin()){
    Serial.println("AM2315 sensor not found, check wiring.");
    while(1);
  }
  delay(2000);
  // Initialize the BME280 sensor.
  while(!bme.begin()){
    Serial.println("BME280 sensor not found, check wiring.");
    while(1);
  }
  // Initialize the UV sensor.
  uv.begin(VEML6070_4_T);
  delay(1000);
  // Calibrate the BME280 temperature sensor in case it reads high.
  bme.setTempCal(-1);
}

//=====================
//=== Loop function ===
//=====================
void loop() {
  // put your main code here, to run repeatedly:
  // Keep track of the timestamp of the current iteration.
  currentMillis=millis();
  // If the length of the interval has passed, begin process of sending data to Raspberry Pi.
  if(currentMillis-previousMillis>=interval){
    // Read from sensors
    bme.readSensor();
    am2315.readSensor();
    // Begin printing to serial port in this format to make it easier to process on the Pi.
    // Last Serial.print needs to include '\n' so that it can be used as a delimiter for each batch of data.
    // Use ';' to split the Timestamp, BME280, AM2315, and VEML6070 data.
    // Use ',' to split each subset of data for each sensor if need be.
    // Use '=' to split string and actual data values.
    Serial.print("Timestamp=");
    Serial.print(currentMillis);
    Serial.print(";");
    Serial.print("BMEHumidity=");
    Serial.print(bme.getHumidity());             // Humidity is in percentage humidity.
    Serial.print(",");
    Serial.print("BMETemperature="); 
    Serial.print(bme.getTemperature_C());        // Temperature is in Celsius.
    Serial.print(",");
    Serial.print("BMEPressure="); 
    Serial.print(bme.getPressure_MB());          // Pressure is in millibars.
    Serial.print(";");
    Serial.print("AMHumidity=");
    Serial.print(am2315.getHumidity());          // Humidity is in relative humidity.
    Serial.print(",");
    Serial.print("AMTemperature=");
    Serial.print(am2315.getTemperature_C());     // Temperature is in Celsius.
    Serial.print(";");
    Serial.print("UV=");
    Serial.println(uv.readUV());                 // UV data is from 0-12. Refer to datasheet to see strength of irradiance.
    // Update timekeeping variable
    previousMillis=currentMillis;
    delay(1000);
  }
}
