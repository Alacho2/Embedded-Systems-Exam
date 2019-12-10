// This #include statement was automatically added by the Particle IDE.
#include <MPU6050.h>

#define LIGHTSENSOR A3
#define TEMPSENSOR A1
#define FIRESENSOR A2
#define WATERSENSOR A0

// EXPOSED VARIABLES
int lightValue = 0;
int fireAverage = 0;
int waterLevel = 0;
int highestLightValue = 0;
double tempValue = 0.0;
double highestTempValue = 0;

int waterLevelArr[3] = {0, 0, 0};
int waterReadIndex = 0;

// MPU values
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;

// RGB LED
int redPin = D2;
int greenPin = D3;
int bluePin = D5;

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
  
  analogWrite(bluePin, 0);  
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
}

void loop() {
    
  haveSensorDataChanged();    

  //updateLightValue();
  // updateTemperature();
 
  digitalWrite(D7, HIGH);
  delay(2000);
  digitalWrite(D7, LOW);
  updateFireReadings();
  resetTotalReadings();
  // updateWaterValue();
  
  /*
  accelgyro.getRotation(&gx, &gy, &gz);
  Serial.println(accelgyro.getRotationX());
  Serial.println(accelgyro.getRotationY());
  Serial.println(accelgyro.getRotationZ());
  Serial.println("Hei"); */
  
  /* accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz); */
}

bool haveSensorDataChanged() {
  updateLightValue();
  updateTemperature();
  
  const int todaysBestLight = highestLightValue;
  const double todaysHighestTemp = highestTempValue;
  
  if (updateWaterReadings()) {
    updateRGBLed(bluePin, 255);  
    return true;
  }
  
  if (lightValue > todaysBestLight+5) {
    highestLightValue = lightValue;
    updateRGBLed(redPin, 255);
    updateRGBLed(greenPin, 255);
    return true;
  }
  
  if (tempValue > todaysHighestTemp+1) {
    highestTempValue = tempValue;
    updateRGBLed(redPin, 255);
    updateRGBLed(greenPin, 0);
    updateRGBLed(bluePin, 0);
    return true;  
  }
  
  // Fire: rgb(255,127,80)
  
  Serial.println("No change");
  // If everything fails, then not data has changed
  // And we don't wanna update the string
  return false;
}

void updateRGBLed(int pin, int intensity){
  analogWrite(pin, intensity);
}

// we'll always construct the same object, based on the same values.
// JS will fetch the same point
// Jeg er mer interessert i når på dagen den fikk mye lys, og hvor mye det var
// Været kommer ikke til å endre seg så fort.
// Blue pin funker ikke på lyset.

int updateLightValue() {
  lightValue = analogRead(LIGHTSENSOR);
}

void updateWaterValue() {
  waterLevel = analogRead(WATERSENSOR);
  //Serial.println(waterLevel);
}

int getCurrentWaterValue() {
  return analogRead(WATERSENSOR);
}


void resetTotalReadings() {
  if (Time.hour() == 0 && Time.isAM() && Time.second() >= 0) {
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
  tempValue = (analogRead(TEMPSENSOR) * 100.0) / 1024.0 - 8;
}

bool updateWaterReadings() {
  const int currentWater = getCurrentWaterValue() / 100; // Water level in mm
  
  if (currentWater > waterLevel) {
    waterLevel = currentWater;
    return true;
  }
  
  Serial.println(currentWater);
  return false;
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


/*

void setup() {
    
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);
    Serial.println(accelgyro.getDeviceID())
    delay(2000);
    
    toggleLed();
    
}

*/
