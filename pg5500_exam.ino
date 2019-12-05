#include <Adafruit_DHT.h>

#define DHTTYPE DHT11

#define LIGHTSENSOR A0
#define TEMPSENSOR A1

#define DHTPIN 2 

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

int lightValue = 0;
double tempValue = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(D7, OUTPUT);
  
  Particle.variable("lightLevel", lightValue);
  Particle.variable("temp", tempValue);
}

void loop() {
  lightValue = analogRead(LIGHTSENSOR);
  int sensorValue = analogRead(TEMPSENSOR);
  //  (4.9 * sensorValue * 100.0) / 1024.0;
  // float temperatureC = (4.9 * sensorValue * 100.0) / 1024.0;
  tempValue = (sensorValue * 100.0) / 1024.0 - 5;
  
  
  digitalWrite(D7, HIGH);
  delay(2000);
  digitalWrite(D7, LOW);
}
