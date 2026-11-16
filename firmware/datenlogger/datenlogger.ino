// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors
/*
 The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 7 (for MKRZero SD: SDCARD_SS_PIN)*/
 
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>

#define EVENT 2
#define STOP 3
#define GENERATOR A4           //Anschluss Spannungsteiler des Boardnetzes
#define R11 2200.00 //R11 im Spannungsteiler Generator
#define R10 5000.00 //R10 im Spannungsteiler Generator
#define DELAY 300
#define LED1 10  //Stop LED
#define LED2 9  //Init LED
#define LED3 8 //Logging LED

unsigned long writetime = 0;
const int chipSelect = 7;
float boardvoltage = 0.0;
unsigned long time = 0.0;
bool initial = false;
volatile byte state = LOW;
volatile byte event = false;
volatile byte stop = false;

void setup() {

Serial.begin(9600);
 /*while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/
pinMode(GENERATOR, INPUT);
pinMode(EVENT, INPUT_PULLUP);
pinMode(STOP, INPUT_PULLUP);
pinMode(LED1, OUTPUT);
pinMode(LED2, OUTPUT);
pinMode(LED3, OUTPUT);
digitalWrite(LED1, LOW);
digitalWrite(LED2, LOW);
digitalWrite(LED3, LOW);

if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    digitalWrite(LED2, HIGH);
    // don't do anything more:
    while (1);
  }


}

void loop() {
  
  if(writetime <= 1000)
  {
    writetime = millis();
    Serial.println("Initialisieren...");
    delay(100);
    digitalWrite(LED2, !state);
    state = !state;
  }
  else if(initial == false)
    {
      Serial.println("Initialisiert!");
      initial = true;
      writetime = millis();
      digitalWrite(LED2, LOW);
      delay(10);
      state = !state;
      stop = true;
      //attachInterrupt(digitalPinToInterrupt(EVENT), ISR_EVENT, FALLING);
      attachInterrupt(digitalPinToInterrupt(STOP), ISR_STOP, RISING);
    }

  while(stop == true)
  {
    Serial.println("Lesen wurde beendet!");
    digitalWrite(LED3, LOW);
    
    digitalWrite(LED1, !state);
    delay(500);
    state = !state;
    event = true;
    digitalWrite(LED1, !state);
    delay(500);
  }


  if(((millis() - writetime) >= DELAY) && (initial == true) && (stop == false))
  {
    state = !state;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, !state);
    Serial.println("Liest Spannung ein...");
    
    //String 
    String dataString = "";

    dataString += "Time ";
    time = writetime;
    dataString += String(time);
    writetime = millis();
    
    
    boardvoltage = analogRead(GENERATOR);                     //Eingang des Spannungsteilers einlesen
    boardvoltage = (5.00 / 1023.00) * boardvoltage;           //Umrechung von Analogwert des Eingangs in eine Spannung
    boardvoltage = (boardvoltage * (R10 + R11)) / R11;  //14V      //Berechnung Boardspannung in [Volt]
    
    dataString += " ";
    dataString += String(boardvoltage);
    
    if(event == true)
    {
      digitalWrite(LED2, HIGH);
      dataString += " 15";
    }

    else {dataString += " 0";}

    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    if ((dataFile) && (stop != true)) 
    {
      dataFile.println(dataString);
      dataFile.close();
      digitalWrite(LED3, !state);      
      // print to the serial port too:
      Serial.println(dataString);
      
      if(event == true)
      {
        event = false;
        attachInterrupt(digitalPinToInterrupt(EVENT), ISR_EVENT, FALLING);
        attachInterrupt(digitalPinToInterrupt(STOP), ISR_STOP, RISING);
      }
    }
    else {
    Serial.println("error opening datalog.txt");
    delay(5000);
    }
  }

}

void ISR_EVENT(void)
{ 
  detachInterrupt(digitalPinToInterrupt(EVENT));
  detachInterrupt(digitalPinToInterrupt(STOP));
  event = !event;
}

void ISR_STOP(void)
{
  detachInterrupt(digitalPinToInterrupt(EVENT));
  stop = !stop;
}
