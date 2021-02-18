//==================================
//=== Include relevant libraries ===
// =================================
#define SensorPin 0 //this is the pin the Ph sensor is connected to for analog input
#include <Wire.h>
#include "SoftwareSerial.h"
#include "ping1d.h"
#include "cactus_io_BME280_I2C.h"
#include <Adafruit_VEML6070.h>
#include "cactus_io_AM2315.h"

//======================================================
//=== Create three instances for each of the sensors ===
//======================================================
AM2315 am2315;
BME280_I2C bme(0x76);
Adafruit_VEML6070 uv = Adafruit_VEML6070();

// The white (tx) cable on the ping connects to pin 9 (Software Serial rx port)
// The green (rx) cable on the ping connects to pin 10 (Software serial tx port) 
SoftwareSerial pingSerial { 9, 10}; 
Ping1D myPing { pingSerial };



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

  myPing.begin(9600);
  // Initialize the serial port (Must be 9600 for the pH and sonar sensor).
  Serial.begin(9600);
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

  while(!myPing.intialize()) {
    Serial.println("Ping device did not initialize");
    while(1);
  }
  // Initialize the UV sensor.
  uv.begin(VEML6070_4_T);
  delay(1000);
  // Calibrate the BME280 temperature sensor in case it reads high.
  bme.setTempCal(-1);
  
  pinMode(13, OUTPUT);
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
    Serial.print(",");
    Serial.print("UV=");
    Serial.print(uv.readUV());                 // UV data is from 0-12. Refer to datasheet to see strength of irradiance.
    Serial.print("ph=");
    Serial.print(get_ph_value());
    Serial.print(",");
    get_sonar_reading();
    Serial.print(";");
    
    // Update timekeeping variable
    previousMillis=currentMillis;
    delay(1000);
  }
}

float get_ph_value(){ 
  unsigned long int avgValue;
  float b;
  int buf[10], temp;

  
  //get 10 readings from sensor
  for(int i=0; i<9;i++) {
    buf[i] = analogRead(sensorPin);
    delay(10);
  }
  
  //sort array (bubble sort)
  for(int i=0;i<9;i++)       
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }

  avgValue = 0;
  for(int i=2; i<8;i++)
    avgValue+=buf[i];
  float phValue = (float)avgValue*5.0/1024/6;
  phValue*=3.5
  return phValue;
}

void get_sonar_reading(){
  while (!myPing.update()) {
        Serial.println("Ping device update failed");
        while(1);
  }

  Serial.print("Distance: ");
  Serial.print(myPing.distance())
  Serial.print(",");
  Serial.print(myPing.confidence());
  
}
