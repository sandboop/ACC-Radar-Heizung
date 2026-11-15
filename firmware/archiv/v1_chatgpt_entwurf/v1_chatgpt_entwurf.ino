
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>

#define DHTPIN PA6
#define DHTTYPE DHT11
#define GENERATOR_VOLTAGE_MIN 12.3
#define GENERATOR_VOLTAGE_MAX 14.6
#define BETA 3435

DHT_unified dht(DHTPIN, DHTTYPE);
int heatingElementOut = PB0;
int fanOut = PA1;
int twelveVOut = PB1;
int tempSensor1 = PA4;
int tempSensor2 = PA5;
int generatorVoltage = PA7;
float temp = 0.0;
float humid = 0.0;
unsigned long mytime1 = 0;
unsigned long mytime2 = 0;
unsigned long mytime3 = 0;
unsigned long mytime4 = 0;
const float minTemp = 50.0;
const float maxTemp = 60.0;
int fanSpeedStart = 20;
int fanSpeed = 0;
float tempNozzle = 0.0;
bool heating = false;

void setup() {
  pinMode(heatingElementOut, OUTPUT);
  digitalWrite(heatingElementOut, LOW);

  pinMode(fanOut, OUTPUT);
  analogWrite(fanOut, 0);

  pinMode(twelveVOut, OUTPUT);
  digitalWrite(twelveVOut, LOW);

  pinMode(tempSensor1, INPUT);
  pinMode(tempSensor2, INPUT);
  pinMode(generatorVoltage, INPUT);

  dht.begin();
}

void loop() {
  if ((temp < 5.0) && (humid > 70.0)) {
    if (generatorRunning()) {
      if ((mytime2 + 400) < millis()) {
        checkTemp();
      }
      if ((mytime3 + 500) < millis()) {
        if (startFan()) {
          mytime3 = millis();
        }
      }
    }
  } else {
    endHeat();
  }
  else {
    digitalWrite(heatingElementOut, LOW);
    analogWrite(fanOut, fanSpeedStart);
  }
}

int tempOutside() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    return 0;
  }

  temp = t;
  humid = h;
  return 1;
}

int startFan() {
  if (fanSpeedStart < MAXRPM) {
    fanSpeedStart += 5;
    analogWrite(fanOut, map(fanSpeedStart, 0, 100, 0, 255));
    analogWrite(heatingElementOut, LOW);
    return 0;
  } else {
    return 1;
  }
}

int endHeat() {
  if (heating && tempNozzle >= 20.0) {
    digitalWrite(heatingElementOut, LOW);
    mytime4 = millis();
  }
}

int checkTemp() {
  float val1 = 1 / (log(1 / (1023. / analogRead(tempSensor1) - 1)) / BETA + 1.0 / 298.15) - 273.15;
  tempNozzle = val1;

  if ((val1 < minTemp) && (fanSpeedStart >= MAXRPM)) {
    heating = true;
    digitalWrite(heatingElementOut, HIGH);
  }

  if (val1 > maxTemp) {
    digitalWrite(heatingElementOut, LOW);
  }

  mytime2 = millis();
}

int generatorRunning() {
  int val = analogRead(generatorVoltage);
  float val1 = map(val, 0, 1023, 0.0, 5.0);
  
  if (val1 > GENERATOR_VOLTAGE_MIN && val1 < GENERATOR_VOLTAGE_MAX) {
    return true;
  }
  return false;
}
