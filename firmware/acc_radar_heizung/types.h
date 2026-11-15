// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors
#pragma once

// Zustände des Ablaufs (siehe docs/verhalten.md, Abschnitt 3)
enum State : uint8_t
{
  ST_START,        // Start, wartet auf die ersten Messwerte
  ST_STANDBY,      // wartet auf Motor läuft + Wetterbedingung
  ST_FAN_PRERUN,   // Lüfter läuft vor der Heizung
  ST_HEATING,      // geregeltes Heizen
  ST_AFTERRUN,     // Heizung aus, Lüfter-Nachlauf
  ST_FAULT         // Fehler: Heizung aus, Fehlercode wird angezeigt
};

// Eintrag im EEPROM-Fehlerspeicher (4 Byte, siehe faultlog.ino)
struct FaultEntry
{
  uint8_t code;          // Fehlercode 1 bis 5, 6 = Watchdog-Reset, 0xFF = leer
  uint8_t state;         // Zustand, in dem der Fehler auftrat (State)
  int8_t t_plate_c;      // Temperatur Logo-Platte in Grad
  uint8_t vbat_100mv;    // Bordspannung in 0,1 V
};

// Aktuelle Messwerte, alles in Ganzzahlen (Einheiten siehe config.h)
struct Sensors
{
  uint16_t vbat_mv = 0;          // Bordspannung in mV
  uint16_t raw_heater = 0;       // ADC-Rohwert NTC Heizelement
  uint16_t raw_plate = 0;        // ADC-Rohwert NTC Logo-Platte
  int16_t t_heater_dc = 0;       // Temperatur Heizelement in 0,1 Grad
  int16_t t_plate_dc = 0;        // Temperatur Logo-Platte in 0,1 Grad
  bool ntc_valid = false;        // NTC mindestens einmal gelesen
  int16_t t_air_dc = 1000;       // Außenluft in 0,1 Grad (Start: 100 Grad, damit nie geheizt wird)
  int16_t rh_air_d10 = 0;        // Luftfeuchte in 0,1 Prozent
  bool air_valid = false;        // letzter Luftsensor-Messwert war gültig
  uint8_t air_errors = 0;        // Fehlerzähler Luftsensor
};
