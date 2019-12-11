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
double highestTempValue = 0.0;
int brightestHourOfDay = 0;
int brightestMinuteOfDay = 0; 
int warmestMinuteOfDay = 0;
int warmestHourOfDay = 0;
bool hasBeenMoved = false;

// Maximum string length of the Particle is 622.
char buffer[621];

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
int highestFireAverage = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  for (int i = 0; i < numFireReadings; i++) {
    fireReadings[i] = 0;
  }
  
  Time.zone(+1);
  
  Wire.begin();
  accelgyro.initialize();
  
  Particle.variable("jsonData", buffer);
  
  analogWrite(bluePin, 0);  
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
}

void loop() {
    
  // If sensorDataChanged or every 5th seconds of a minute, we'll construct the JSON object with new info    
  if(haveSensorDataChanged() || Time.second() % 5 == 0) {
    int j = snprintf(buffer, sizeof(buffer), "{ 'currTemp': %f, 'highestTemp': %f,", tempValue, highestTempValue);
    j += snprintf(buffer+j, sizeof(buffer), "'hourAtTemp': %d%d,", warmestHourOfDay, warmestMinuteOfDay);
    j += snprintf(buffer+j, sizeof(buffer), "'fireLevel': %d,", fireAverage);
    j += snprintf(buffer+j, sizeof(buffer), "'waterLevel': %d,", waterLevel);
    j += snprintf(buffer+j, sizeof(buffer), "'hasBeenMoved': %d,", hasBeenMoved);
    j += snprintf(buffer+j, sizeof(buffer), "'lightLevel': %d, 'hourAtLight': %d%d, ", lightValue, brightestHourOfDay, brightestMinuteOfDay);
    j += snprintf(buffer+j, sizeof(buffer), "'highestLightValue': %d }", highestLightValue); 
  }
  
  delay(1000);
  resetTotalReadings();
}

bool haveSensorDataChanged() {
  updateLightValue();
  updateTemperature();
  updateFireReadings();
  
  const int todaysBestLight = highestLightValue;
  const double todaysHighestTemp = highestTempValue;
  
  if (lightValue > todaysBestLight+5) {
    highestLightValue = lightValue;
    brightestHourOfDay = Time.hour();
    brightestMinuteOfDay = Time.minute();
    updateRGBLed(redPin, 255);
    updateRGBLed(greenPin, 255);
    updateRGBLed(bluePin, 0);
    return true;
  }
  
  if (tempValue > todaysHighestTemp+0.1) {
    highestTempValue = tempValue;
    warmestHourOfDay = Time.hour();
    warmestMinuteOfDay = Time.minute();
    updateRGBLed(redPin, 255);
    updateRGBLed(greenPin, 0);
    updateRGBLed(bluePin, 0);
    return true;  
  }
  
  // We don't really care about doing anything else than updating the user locally, 
  // The frontend will handle anything specific.
  if (fireAverage >= 30) {
    updateRGBLed(redPin, 255);
    updateRGBLed(greenPin, 127);
    updateRGBLed(bluePin, 80);
    return true;
  }
  
  if (hasMotionSensorChanged()) {
    hasBeenMoved = true;  
    updateRGBLed(redPin, 0);
    updateRGBLed(greenPin, 255);
    updateRGBLed(bluePin, 0);
    return true;  
  }
  
  if (updateWaterReadings()) {
    updateRGBLed(redPin, 0);
    updateRGBLed(greenPin, 0);
    updateRGBLed(bluePin, 255); 
    return true;
  }
  
  // If everything fails, then not data has changed
  // And we don't wanna update the string
  Serial.println("No change");
  return false;
}

void updateRGBLed(int pin, int intensity){
  analogWrite(pin, intensity);
}

bool hasMotionSensorChanged(){
  if (accelgyro.getRotationX() >= 250 || accelgyro.getRotationX() <= -250) {
    return true;  
  }
  
  if (accelgyro.getRotationY() >= 250 || accelgyro.getRotationY() <= -250) {
    return true;  
  }
  
  if (accelgyro.getAccelerationX() >= 550 || accelgyro.getAccelerationX() <= -550) {
    return true;
  }
  
  if (accelgyro.getAccelerationY() >= 550 || accelgyro.getAccelerationY() <= -550) {
    return true;
  }
  // We only care about motions that actually could make the plant fall.
  // Simple stuff like watering, taking care and so forth should not trigger.
  return false;
}

int updateLightValue() {
  lightValue = analogRead(LIGHTSENSOR);
}

void updateWaterValue() {
  waterLevel = analogRead(WATERSENSOR);
}

int getCurrentWaterValue() {
  return analogRead(WATERSENSOR);
}

void updateMotionSensors() {
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
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
  tempValue = (4.9 * sensorValue * 100.0) / 1024.0;
}

bool updateWaterReadings() {
  const int currentWater = getCurrentWaterValue() / 100; // Water level in mm
  
  if (currentWater > waterLevel) {
    waterLevel = currentWater;
    return true;
  }
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
  
  delay(1);
}
