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
int brightestHourOfDay = 0;
int brightestMinuteOfDay = 0; 

char buffer[621];

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
int highestFireAverage = 0;

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
  accelgyro.initialize();
  
  // Put the variables on le interwebz
  // Particle.variable("lightLevel", &lightValue, INT);
  // Particle.variable("temp", &tempValue, DOUBLE);
  Particle.variable("jsonData", buffer);
  // Particle.variable("fireLevel", &fireAverage, INT);
  // Particle.variable("waterLevel", &waterLevel, INT);
  
  analogWrite(bluePin, 0);  
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
}

void loop() {
    
  haveSensorDataChanged();
  
  // Let's construct data into json, so it's easier to handle on the dashboard.
  int j = snprintf(buffer, sizeof(buffer), "{ 'currTemp': %f, 'highestTemp': %f,", tempValue, highestTempValue);
  j += snprintf(buffer+j, sizeof(buffer), "'fireLevel': %d,", fireAverage);
  j += snprintf(buffer+j, sizeof(buffer), "'waterLevel': %d,", waterLevel);
  j += snprintf(buffer+j, sizeof(buffer), "'lightLevel': %d, 'atHour': %d%d }", lightValue, brightestHourOfDay, brightestMinuteOfDay);
  
  Serial.println(j);
 
  digitalWrite(D7, HIGH);
  delay(2000);
  digitalWrite(D7, LOW);
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
  
  if (tempValue > todaysHighestTemp+1) {
    highestTempValue = tempValue;
    updateRGBLed(redPin, 255);
    updateRGBLed(greenPin, 0);
    updateRGBLed(bluePin, 0);
    return true;  
  }
  
  if (fireAverage >= 30) {
    updateRGBLed(redPin, 255);
    updateRGBLed(greenPin, 127);
    updateRGBLed(bluePin, 80);
    return true;
  }
  
  if (hasMotionSensorChanged()) {
    updateRGBLed(redPin, 0);
    updateRGBLed(greenPin, 255);
    updateRGBLed(bluePin, 0);
    return true;  
  }
  
  if (updateWaterReadings()) {
    updateRGBLed(bluePin, 255); 
    updateRGBLed(redPin, 0);
    updateRGBLed(greenPin, 0);
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

// we'll always construct the same object, based on the same values.
// JS will fetch the same point
// Jeg er mer interessert i når på dagen den fikk mye lys, og hvor mye det var
// Været kommer ikke til å endre seg så fort.
// Blue pin funker ikke på lyset.
// Konstruerer json for å gjøre den sikrere og ha færre entry points.

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
  // float temperatureC = (4.9 * sensorValue * 100.0) / 1024.0;
  tempValue = (analogRead(TEMPSENSOR) * 100.0) / 1024.0 - 10;
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
