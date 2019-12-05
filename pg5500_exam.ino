#define LIGHTSENSOR A0
#define TEMPSENSOR A1
#define FIRESENSOR A2

int lightValue = 0;
double tempValue = 0.0;

const int numFireReadings = 10;

int fireReadings[numFireReadings];
int fireReadIndex = 0;
int fireTotal = 0;
int fireAverage = 0;

void setup() {
  Serial.begin(9600);
  pinMode(D7, OUTPUT);
  for (int i = 0; i < numFireReadings; i++) {
    fireReadings[i] = 0;
  }
  
  Particle.variable("lightLevel", lightValue);
  Particle.variable("temp", tempValue);
}

void loop() {

  updateLightValue();
  updateTemperature();
 
  digitalWrite(D7, HIGH);
  //delay(2000);
  digitalWrite(D7, LOW);
  updateFireReadings();
}

void updateLightValue() {
  lightValue = analogRead(LIGHTSENSOR);
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
  
  Serial.println(fireAverage);
  delay(1);
}
