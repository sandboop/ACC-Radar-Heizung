
//#define DEBUG     //Auskommentieren nach Debugging

#include "DHT.h"
#include <Adafruit_Sensor.h>

//NTC-Sensorik Eigenschaften
//________________________________
#define RT0 10000   // Ω
#define B 3950      //  K
#define VCC 5    //Supply  voltage
#define R 9980  //R=10KΩ

//Generator Sensorik
//________________________________

#define R11 2200.00 //R11 im Spannungsteiler Generator
#define R10 5000.00 //R10 im Spannungsteiler Generator

//DHT11 Eigenschaften
//________________________________
#define DHTPIN PIN_PB3              //Anschlusspin des DHT11-Sensors
#define DHTTYPE DHT11               //Typ des DHT Sensors
#define DHTDELAY 2100 //ms          //Auslesegeschwindigkeit laut Datenblatt
#define TEMPSCHWELLE 8 //°C        //Später Ändern auf 8°C
#define HUMIDSCHWELLE 70 //%        //Später Ändern auf 80%
#define REGULATIONDELAY 500.00 //ms //Ausleseverzögerung für Temp-regulation
#define STARTUPDELAY 3000 //ms      //Verzögerung zum Einschalten des Heizelementes
#define SHUTDOWNDELAY 10000//ms     //Ausschaltverzögerung des Controlmoduls
#define STANDYBYDELAY 2000 //ms     //Zeit die verstreicht, bis in Standby gewechselt wird
#define OVERTEMPHEATING 80.00 //°C  //Maximaltemperatur für Heizelement
#define MAXTEMPPLATE 45.00 //°C     //Maximaltemperatur für Logo-Platte
#define MINTEMPPLATE 43.00 //°C     //Minimaltemperatur für Logo-Platte 
#define NTCDELAY 1000 //ms          //Auslesehäufigkeit der NTC-Sensoren
#define MINBOARDVOLTAGE 13.0 //[V]  //Minimale Spannung des Boardnetzes 13.8, (Messung steht noch aus)

//Output Eigenschaften
//________________________________
#define FAN PIN_PC3                 //Anschluss des Lüfters
#define HEATING PIN_PC2             //Anschluss des Heizelementes
#define PSUENABLE PIN_PA4           //Anschluss für Spannungsversorgung Controlmodul
#define NTCPOWER PIN_PB5            //Spannungsversorgung der NTC-Thermistoren
#define ERRORLED PIN_PA6            //Anschluss Error-LED, Visuelle Ausgabe

//Input Eigenschaften
#define NTCHEATING PIN_PA7          //Anschluss NTC für Heizelementüberwachung
#define NTCPLATE PIN_PB4            //Anschluss NTC für Oberflächentemperaturüberwachung Logo
#define GENERATOR PIN_PA5           //Anschluss Spannungsteiler des Boardnetzes

//Variablen für Freigaben
//________________________________
bool release_fan = false;
bool release_heating = false;
bool no_error = false;
bool outheating = false;
bool outfan = false;
bool psuenable = true;
bool shutdown = false;

//Variablen für Sensorik
//________________________________
int errordetected = 5;
float dht_h = 0.0;
float dht_t = 100.0;
float RT1, RT2, VR1, VR2, ln1, ln2, temp1, temp2,  T0, VRT1, VRT2;
float heating_act, plate_act, boardvoltage;
float errorcounter = 0;
unsigned long readtimedelay  = 0;
unsigned long readtimedelay1 = 0;
unsigned long readtimedelay2 = 0;
unsigned long readtimedelay3 = 0;
unsigned long readtimedelay4 = 0;
unsigned long readtimedelay5 = 0;
DHT dht(DHTPIN, DHTTYPE);



void setup() {
  pinMode(FAN, OUTPUT);
  pinMode(HEATING, OUTPUT);
  pinMode(PSUENABLE, OUTPUT);
  pinMode(ERRORLED, OUTPUT);
  pinMode(GENERATOR, INPUT);
  pinMode(NTCHEATING, INPUT);
  pinMode(NTCPLATE, INPUT);
  digitalWrite(FAN, LOW);
  digitalWrite(HEATING, LOW);
  digitalWrite(PSUENABLE, HIGH);
  digitalWrite(NTCPOWER, LOW);
  
  #ifdef DEBUG
    Serial.begin(9600);               //Optional für Debugging
  #endif
  
  dht.begin();                      //DHT Sensor Starten, bzw. One-wire-bus initialisieren
  T0 = 25 + 273.15;                 //Temperature  T0 from datasheet, conversion from Celsius to kelvin
}
void loop()

{
  
  get_inputs();

  safety();

  process();

  write_output();

  return;
  
}

void get_inputs() 
{


  //________________________________________________________________________________________
  //NTC-Sensorik wird eingelesen und berechnet  

  if((readtimedelay + NTCDELAY) <= millis()) 
  { 
    digitalWrite(NTCPOWER, HIGH);                             //Spannungsversorgung NTC einschalten
    delay(10);                                                //Wartezeit bis Ausgang sich eingependelt hat. Datenblatt typisch 1,5ns bei 20pF (Cap)
    
    readtimedelay = millis();                                 //Auslesetimer auf millis()-Timer setzen um die Bedingung beim nächsten Durchlauf zu erfüllen

    boardvoltage = analogRead(GENERATOR);                     //Eingang des Spannungsteilers einlesen
    boardvoltage = (5.00 / 1023.00) * boardvoltage;           //Umrechung von Analogwert des Eingangs in eine Spannung
    boardvoltage = (boardvoltage * (R10 + R11)) / R11;  //14V      //Berechnung Boardspannung in [Volt]
  
    #ifdef DEBUG
     Serial.print("Boardvoltage Calc Wert = ");
     Serial.println(boardvoltage);
    #endif
    

    
    VRT1 = analogRead(NTCHEATING);              //Analog Wert von NTC-PINs Lesen
    VRT2 = analogRead(NTCPLATE);

    digitalWrite(NTCPOWER, LOW);                //Spannungsversorgung NTC ausschalten um eigenerwärmung zu reduzieren
    
    VRT1  = (5.00 / 1023.00) * VRT1;            //Umrechung von Analogwert der Eingänge in Spannungen
    VRT2  = (5.00 / 1023.00) * VRT2;
    VR1 = VCC - VRT1;                           //Berechung des NTC-Spannngsabfalls
    VR2 = VCC - VRT2;
    RT1 = VRT1 / (VR1 / R);                     //Berechung Widerstand NTC
    RT2 = VRT2 / (VR2 / R);

    ln1 = log(RT1 / RT0);                       
    ln2 = log(RT2 / RT0);
    temp1 = (1 / ((ln1 / B) + (1 / T0)));       //Temperature from thermistor
    temp2 = (1 / ((ln2 / B) + (1 / T0)));
    heating_act =  temp1 - 273.15;      //     10°C   //Umrechung in Temperatur und speicherung in einer Variable
    plate_act =  temp2 - 273.15;        //     43°C
  }


  //________________________________________________________________________________________
  //DHT-11 Sensor-Daten werden eingelesen und verifiziert

  if((readtimedelay1 + DHTDELAY) <=  millis())    //Bedingung prüfen ob DHT-11 Sensor wieder ausgelesen werden kann
  {
    dht_h = dht.readHumidity();         //1°C
    dht_t = dht.readTemperature();      //71%
    if(errorcounter >= 1)
    {
      errorcounter--
    }
    readtimedelay1 = millis();
  
    if (isnan(dht_h) || isnan(dht_t))             //Verifikation ob ausgelesene Daten korrekt sind
    {
      readtimedelay1 = millis();
      release_heating = false;
      errorcounter++;
                          
    
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

}

void safety()

{//     10°C            80°C
  if(heating_act >= OVERTEMPHEATING)
    {//          true
      no_error = false;
      //           false
      outheating = false;
      //                false
      release_heating = false;
      //    14V             13V
      if(boardvoltage >= MINBOARDVOLTAGE)
      {//             false
        release_fan = true;
        //       false
        outfan = false;
      }
      
    }
  else if((no_error == false) && (heating_act <= OVERTEMPHEATING - 20.0)) 
    {//             false
      release_fan = false;
      //         false
      shutdown = true;
    }
  if (errordetected <= errorcounter)
    {
      no_error = false;
    }      

}

void process()
{
//                true        71%         70%             1°C        8°C               14V             13,0V
  if((no_error == true) && ((dht_h >= HUMIDSCHWELLE) && (dht_t <= TEMPSCHWELLE) && (boardvoltage >= MINBOARDVOLTAGE)))  
  {
    fan();    //            false                 true
    if ((release_heating == true) && (no_error == true))
    {
      heating();
    }
    else {outheating = false;}
  }//         14.0V             13.0V                       true
  else if((boardvoltage <= MINBOARDVOLTAGE) && (no_error == true)) 
  
      {//          false
        shutdown = true;
        standby();
      }

      else 
      {
        standby();
      }

loop();
}


void heating()
{//              false                 true
  if((out_fan == true) && (no_error == true))
  {//     14.0V             13.0V
    if (boardvoltage >= MINBOARDVOLTAGE)
    {//       43°C           43°C                         
      if (plate_act <= MINTEMPPLATE)
      {//            false
        outheating = true;
      }//       43°C            45°C
      else if(plate_act >= MAXTEMPPLATE)
      {
        outheating = false;
      }
      
    }
      
  }
  
}

void fan()
{//                                                                   true                   true
  if(((readtimedelay2 + STARTUPDELAY) <= millis()) && (release_fan == false) && (no_error == true))
  {
  readtimedelay2 = millis();
  release_fan = true;
  
  #ifdef DEBUG
    Serial.print("release_fan = ");
    Serial.println(release_fan);
  #endif
  }
//                                                                    true                   true
  else if(((readtimedelay2 + STARTUPDELAY) <= millis()) && (outfan == false) && (no_error == true))
  {
    readtimedelay2 = millis();
    outfan = true;
    release_heating = true;
    
    #ifdef DEBUG
      Serial.print("outfan = ");
      Serial.println(outfan);
    #endif
  }
  else 
  {
    release_heating = false;
    outheating = false;
  }
}

void write_output()

{
  if((readtimedelay3 + REGULATIONDELAY) <= millis())
  {
    readtimedelay3 = millis();

  if(release_fan == true)
    {digitalWrite(FAN, HIGH);}
    else {digitalWrite(FAN, LOW);}

  if(outheating == true)
    {digitalWrite(HEATING, HIGH);}
    else {digitalWrite(HEATING, LOW);}

  if(psuenable == true)
    {digitalWrite(PSUENABLE, HIGH);}
    else {digitalWrite(PSUENABLE, LOW);}

  if((no_error == false) || (errorcounter >= errordetected))
    {digitalWrite(ERRORLED, HIGH);}
    else {digitalWrite(ERRORLED, LOW);}
  }

}

void standby()

{//                                                                      false
  if((readtimedelay4 + STANDYBYDELAY <= millis()) && (release_heating == true))
  {
    release_heating = false;
    readtimedelay4 = millis();
  }
//                                                                true                     false
  if((readtimedelay4 + STANDYBYDELAY <= millis()) && (no_error == true) && (release_fan == true))
    {
      release_fan = false;
      out_fan = false;
    }
    else {release_fan = true;}
//                                                                false
  if((readtimedelay4 + SHUTDOWNDELAY <= millis()) && (shutdown == true))
  {
    readtimedelay4 = millis();
    release_heating = false;
    release_fan = false;
    psuenable = false;
  }
  
}