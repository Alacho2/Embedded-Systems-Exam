// This #include statement was automatically added by the Particle IDE.
#include <MPU6050.h>

#define LIGHTSENSOR A3
#define TEMPSENSOR A1
#define FIRESENSOR A2
#define WATERSENSOR A0

// EXPOSED VARIABLES
int lightValue = 0;
double tempValue = 0.0;
int fireAverage = 0;
int waterLevel = 0;
int highestLightValue = 0;
int highestTempValue = 0;

// MPU values
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;

// RGB LED
int redPin = D2;
int greenPin = D3;
int bluePin = D4;

// Vars for fireValues.
const int numFireReadings = 10;
int fireReadings[numFireReadings];
int fireReadIndex = 0;
int fireTotal = 0;

void setup() {
  Serial.begin(9600);
  pinMode(D7, OUTPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  for (int i = 0; i < numFireReadings; i++) {
    fireReadings[i] = 0;
  }
  Time.zone(+1);
  
  Wire.begin();
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  
  // Put the variables on le interwebz
  Particle.variable("lightLevel", &lightValue, INT);
  Particle.variable("temp", &tempValue, DOUBLE);
  Particle.variable("fireLevel", &fireAverage, INT);
  Particle.variable("waterLevel", &waterLevel, INT);
}

void loop() {

  updateLightValue();
  updateTemperature();
 
  digitalWrite(D7, HIGH);
  delay(2000);
  digitalWrite(D7, LOW);
  updateFireReadings();
  resetTotalReadings();
  updateWaterValue();
  
  /*
  accelgyro.getRotation(&gx, &gy, &gz);
  Serial.println(accelgyro.getRotationX());
  Serial.println(accelgyro.getRotationY());
  Serial.println(accelgyro.getRotationZ());
  Serial.println("Hei"); */
  
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);
}

void updateHighestValues() {
  if (lightValue > highestLightValue) {
    // lightValue = analogRead(LIGHTSENSOR);
    highestLightValue = lightValue;
    // Add what time it is.
  }
  
  if (tempValue > highestLightValue) {
    highestTempValue = tempValue;
    // Add what time it is.
  }
}

void updateLightValue() {
  lightValue = analogRead(LIGHTSENSOR);
}

void updateWaterValue() {
  waterLevel = analogRead(WATERSENSOR);
  //Serial.println(waterLevel);
}


void resetTotalReadings() {
  if (Time.hour() == 0 && Time.isAM()) {
    highestTempValue = 0;
    highestLightValue = 0;
    fireTotal = 0;
    fireReadIndex = 0;
    fireAverage = 0;
    
    for (int i = 0; i < numFireReadings; i++) {
      fireReadings[i] = 0;
    }
  }
}

void updateTemperature() {
  // float temperatureC = (4.9 * sensorValue * 100.0) / 1024.0;
  tempValue = (analogRead(TEMPSENSOR) * 100.0) / 1024.0 - 5;
}

void updateFireReadings() {
  fireTotal = fireTotal - fireReadings[fireReadIndex];

  fireReadings[fireReadIndex] = analogRead(FIRESENSOR);
  fireTotal = fireTotal + fireReadings[fireReadIndex];
  fireReadIndex = fireReadIndex + 1;
  
  if (fireReadIndex >= numFireReadings) {
    fireReadIndex = 0;
  }

  fireAverage = fireTotal / numFireReadings;
  
  //Serial.println(fireAverage);
  delay(1);
}
