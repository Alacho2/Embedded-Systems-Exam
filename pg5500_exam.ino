#define LIGHTSENSOR A0
#define TEMPSENSOR A1

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
  // float temperatureC = (4.9 * sensorValue * 100.0) / 1024.0;
  tempValue = (analogRead(TEMPSENSOR) * 100.0) / 1024.0 - 5;
 
  digitalWrite(D7, HIGH);
  delay(2000);
  digitalWrite(D7, LOW);
}
