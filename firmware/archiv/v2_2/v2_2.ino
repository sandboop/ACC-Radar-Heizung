
//#define DEBUG     //Auskommentieren nach Debugging

#include "DHT.h"
#include <Adafruit_Sensor.h>

//NTC-Sensorik Eigenschaften
//________________________________
#define RT0 10000   // Ω
#define B 3950      //  K
#define VCC 5    //Supply  voltage
#define R 10000  //R=10KΩ

//Generator Sensorik
//________________________________

#define R9 2200.00 //R9 im Spannungsteiler Generator
#define R10 5000.00 //R10 im Spannungsteiler Generator

//DHT11 Eigenschaften
//________________________________
#define DHTPIN 3      //Anschluss
#define DHTTYPE DHT11
#define DHTDELAY 2100 //ms
#define TEMPSCHWELLE 30 //°C  //Später Ändern auf 8°C
#define HUMIDSCHWELLE 20 //% //Später Ändern auf 80%
#define REGULATIONDELAY 500.00 //ms Ausleseverzögerung für Temp-regulation
#define STARTUPDELAY 3000 //ms Verzögerung zum Einschalten des Heizelementes
#define SHUTDOWNDELAY 10000//ms Ausschaltverzögerung des Controlmoduls
#define STANDYBYDELAY 2000 //ms Zeit die verstreicht, bis in Standby gewechselt wird
#define OVERTEMPHEATING 80.00 //°C Maximaltemperatur für Heizelement
#define MAXTEMPPLATE 45.00 //°C Maximaltemperatur für Logo-Platte
#define MINTEMPPLATE 45.00 //°C Minimaltemperatur für Logo-Platte 
#define NTCDELAY 1000 //ms Auslesehäufigkeit der NTC-Sensoren
#define MINBOARDVOLTAGE 12.3 //Minimale Spannung des Boardnetzes 13.8

//Output Eigenschaften
//________________________________
#define FAN 8     //Anschluss des Lüfters
#define HEATING 9   //Anschluss des Heizelementes
#define PSUENABLE 10 //Anschluss für Spannungsversorgung Controlmodul

//Input Eigenschaften
#define NTCHEATING A3 //Anschluss NTC für Heizelementüberwachung
#define NTCPLATE A2 //Anschluss NTC für Oberflächentemperaturüberwachung Logo
#define GENERATOR A1 //Anschluss Spannungsteiler des Boardnetzes

//Variablen für Freigaben
//________________________________
bool release_fan = false;
bool release_heating = false;
bool no_error = false;
bool outheating = false;
bool outfan = false;
bool psuenable = true;

//Variablen für Sensorik
//________________________________
float boardvoltage;
float dht_h = 0.0;
float dht_t = 100.0;
float RT1, RT2, VR1, VR2, ln1, ln2, temp1, temp2,  T0, VRT1, VRT2;
float heating_act, plate_act;
unsigned long readtimedelay  = 0;
unsigned long readtimedelay1 = 0;
unsigned long readtimedelay2 = 0;
unsigned long readtimedelay3 = 0;
unsigned long readtimedelay4 = 0;
DHT dht(DHTPIN, DHTTYPE);



void setup() {
  pinMode(FAN, OUTPUT);
  pinMode(HEATING, OUTPUT);
  pinMode(PSUENABLE, OUTPUT);
  pinMode(GENERATOR, INPUT);
  pinMode(NTCHEATING, INPUT);
  pinMode(NTCPLATE, INPUT);
  digitalWrite(FAN, LOW);
  digitalWrite(HEATING, LOW);
  digitalWrite(PSUENABLE, HIGH);
  
  #ifdef DEBUG
    Serial.begin(9600);               //Optional für Debugging
  #endif
  
  dht.begin();                      //DHT Sensor Starten, bzw. One-wire-bus
  T0 = 25 + 273.15;                 //Temperature  T0 from datasheet, conversion from Celsius to kelvin
}

void loop() 
{
//Inputs einlesen

get_inputs();

regulation();

if((boardvoltage >= MINBOARDVOLTAGE) && (plate_act <= MINTEMPPLATE))
{
  #ifdef DEBUG
    Serial.println("Boardspannung OK");
  #endif
  
  if((no_error == true) && (((dht_h >= HUMIDSCHWELLE) && (dht_t <= TEMPSCHWELLE))))
  {
    #ifdef DEBUG
      Serial.println("Start FAN()");
    #endif
    
    fan();
    readtimedelay3 = millis();
  }
  else standby();

if((outfan == true) && (no_error == true) && (((dht_h >= HUMIDSCHWELLE) && (dht_t <= TEMPSCHWELLE))))
    {
      readtimedelay4 = millis();
      #ifdef DEBUG
        Serial.println("Start heating()");
        Serial.println("Run regulation()");
      #endif
      
      release_heating = true;
    }
    else release_heating = false;

}
else standby();

return;
}


void standby()
{
//________________________________________________________________
if((millis() - readtimedelay3) >= STANDYBYDELAY)
{
  #ifdef DEBUG
    Serial.println("Standby aktiv!");
  #endif  

  release_heating = false;
  outheating = false;
  digitalWrite(HEATING, LOW);
}

  
  if(((millis() - readtimedelay4) >= SHUTDOWNDELAY) && (heating_act <= MAXTEMPPLATE))
  {
    #ifdef DEBUG
     Serial.println("Shutdown, wait for reboot MCU!");
    #endif

    release_fan = false;
    outfan = false;
    digitalWrite(FAN, LOW);
    digitalWrite(PSUENABLE, LOW);
    delay(1000);
  }
  else digitalWrite(PSUENABLE, HIGH);

return;
}

void get_inputs() 
{
  
if((millis() - readtimedelay) >= NTCDELAY)

{
  readtimedelay = millis();

  boardvoltage = analogRead(GENERATOR);
  boardvoltage = (5.00 / 1023.00) * boardvoltage;
  boardvoltage = (boardvoltage * (R10 + R9)) / R9; //Berechnung Boardspannung in [Volt]
  
  #ifdef DEBUG
    Serial.print("Boardvoltage Calc Wert = ");
    Serial.println(boardvoltage);
  #endif

  VRT1 = analogRead(NTCHEATING);              //Acquisition analog value of VRT
  VRT2 = analogRead(NTCPLATE);
  VRT1  = (5.00 / 1023.00) * VRT1;      //Conversion to voltage
  VRT2  = (5.00 / 1023.00) * VRT2;
  VR1 = VCC - VRT1;
  VR2 = VCC - VRT2;
  RT1 = VRT1 / (VR1 / R);               //Resistance of RT
  RT2 = VRT2 / (VR2 / R);

  ln1 = log(RT1 / RT0);
  ln2 = log(RT2 / RT0);
  temp1 = (1 / ((ln1 / B) + (1 / T0))); //Temperature from thermistor
  temp2 = (1 / ((ln2 / B) + (1 / T0)));
  heating_act =  temp1 - 273.15;                 //Conversion to Celsius
  plate_act =  temp2 - 273.15;

}

if((millis() - readtimedelay1) >= DHTDELAY)
  {
  dht_h = dht.readHumidity();
  dht_t = dht.readTemperature();
  no_error = true;
  readtimedelay1 = millis();
  
  if (isnan(dht_h) || isnan(dht_t)) 
  {
    readtimedelay1 = millis();
    release_heating = false;
    no_error = false;

    #ifdef DEBUG
      Serial.println("Failed to read from DHT sensor!");
    #endif
   }
}

#ifdef DEBUG
  Serial.print("NTC_HEATING = ");
  Serial.print(heating_act);
  Serial.print("°C NTC_PLATE = ");
  Serial.print(plate_act);
  Serial.print("°C ");
  Serial.print("DHT-Messung: Temp = ");
  Serial.print(dht_t);
  Serial.print("°C Humid = ");
  Serial.print(dht_h);
  Serial.println("%");
  Serial.print("Lüfter zustand: ");
  Serial.print(outfan);
  Serial.print(" Heizung zustand: ");
  Serial.println(release_heating);
  Serial.print("Safety Variable: ");
  Serial.println(no_error);
  delay(50);
#endif

return;

}

void fan()
{
  if(release_fan == false)
  {
  readtimedelay2 = millis();
  release_fan = true;
  digitalWrite(FAN, HIGH);

  #ifdef DEBUG
    Serial.print("release_fan = ");
    Serial.println(release_fan);
  #endif
  }

  if((millis() - readtimedelay2) >= STARTUPDELAY)
  {
    readtimedelay2 = millis();
    outfan = true;
    #ifdef DEBUG
      Serial.print("outfan = ");
      Serial.println(outfan);
    #endif
  }
  
return;
  
}

void heating()
{

  release_heating = true;

#ifdef DEBUG
  Serial.print("release_heating = ");
  Serial.println(release_heating);
  delay(1000);
#endif

  return;
}

void regulation()
{
  if((plate_act >= MAXTEMPPLATE) || (heating_act >= OVERTEMPHEATING) || (boardvoltage <= MINBOARDVOLTAGE) || (release_heating == false) || (no_error == false) || (release_fan == false) || (outfan == false))
  {
    outheating = false;
    #ifdef DEBUG
      Serial.println("Bedingung für Heizung fehlt");
    #endif

    digitalWrite(HEATING, LOW);
  }
  else if(release_heating == true)
  {
    readtimedelay3 = millis();
    readtimedelay4 = millis();
    #ifdef DEBUG
      Serial.println("Heizung wird freigegeben");
      delay(1000);
      #endif
    if(no_error == true)
    {
      #ifdef DEBUG
        Serial.println("Kein Error");
      #endif

      if(plate_act <= MINTEMPPLATE)
      {

        #ifdef DEBUG
          Serial.println("Plattentemp niedrig");
          delay(1000);
        #endif

        if(outfan == true)
        {
          #ifdef DEBUG
            Serial.println("Lüfter aktiv");
            delay(1000);
          #endif

          if(boardvoltage >= MINBOARDVOLTAGE)
          { 
            #ifdef DEBUG
              Serial.print("Boardspannung OK");
            #endif

            if(heating_act <= OVERTEMPHEATING)
            {
              #ifdef DEBUG
                Serial.println("Heizung OK");
                delay(1000);
              #endif
              outheating = true;
              digitalWrite(HEATING, HIGH);
              
            }
          }
        }
      }
    }
  }

return;
}
