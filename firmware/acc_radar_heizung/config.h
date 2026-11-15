// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors
//
// Konfiguration: alle Pins, Zeiten und Grenzwerte an einer Stelle.
// Die Bedeutung der Werte steht in docs/verhalten.md (Verhaltensspezifikation).
//
// Die Firmware rechnet ohne Fließkomma (spart Flash):
//   Temperaturen in 0,1 Grad  (Suffix _DC, z. B. 195 = 19,5 Grad)
//   Luftfeuchte in 0,1 Prozent (Suffix _D10, z. B. 800 = 80,0 %)
//   Spannungen in mV           (Suffix _MV)
//   Zeiten in ms               (Suffix _MS)
// Die Hilfsfunktionen DC(), PCT10() und MV() rechnen Konstanten nur beim Übersetzen um.
#pragma once

#include <Arduino.h>

#define FIRMWARE_VERSION "0.4.0-dev"

constexpr int16_t DC(float celsius) { return (int16_t)(celsius * 10.0f + (celsius >= 0.0f ? 0.5f : -0.5f)); }
constexpr int16_t PCT10(float percent) { return (int16_t)(percent * 10.0f + 0.5f); }
constexpr uint16_t MV(float volt) { return (uint16_t)(volt * 1000.0f + 0.5f); }

// ---------------------------------------------------------------------------
// Luftsensor-Treiber (air_dht11.ino)
//   0 = eigener DHT11-Treiber (Standard, spart ca. 2,5 KB Flash)
//   1 = Adafruit-DHT-Bibliothek (zum Vergleich, braucht die Bibliotheken
//       "DHT sensor library" und "Adafruit Unified Sensor")
// ---------------------------------------------------------------------------
#define AIR_DRIVER_OWN 0
#define AIR_DRIVER_LIBRARY 1
#define AIR_DRIVER AIR_DRIVER_OWN

// ---------------------------------------------------------------------------
// Pins (Platine V3.2, siehe docs/hardware-analyse-v3.2.md)
// ---------------------------------------------------------------------------
constexpr uint8_t PIN_AIR_SENSOR = PIN_PB3;   // DHT11, Daten
constexpr uint8_t PIN_NTC_HEATER = PIN_PA7;   // NTC Heizelement (IN1)
constexpr uint8_t PIN_NTC_PLATE  = PIN_PB4;   // NTC Logo-Platte (IN2)
constexpr uint8_t PIN_VBAT       = PIN_PA5;   // Spannungsteiler Bordspannung
constexpr uint8_t PIN_HEATER     = PIN_PC2;   // Heizelement (Treiber INB)
constexpr uint8_t PIN_FAN        = PIN_PC3;   // Lüfter (Treiber INA)
constexpr uint8_t PIN_PSU_HOLD   = PIN_PA4;   // HIGH = Netzteil halten, LOW = Netzteil aus
constexpr uint8_t PIN_ERROR_LED  = PIN_PA6;   // Fehler-LED

// Direkter Portzugriff des eigenen DHT11-Treibers (muss zu PIN_AIR_SENSOR passen: PB3)
#define AIR_VPORT VPORTB
#define AIR_BM PIN3_bm

// ---------------------------------------------------------------------------
// Messung
// ---------------------------------------------------------------------------
constexpr uint32_t ADC_VREF_MV = 4900;        // Versorgung/ADC-Referenz der Spannungsmessung (fest, Audit F-12)
constexpr uint32_t ADC_MAX = 1023;

constexpr uint32_t DIV_R_TOP_OHM = 5000;      // R10, Teiler Bordspannung
constexpr uint32_t DIV_R_BOTTOM_OHM = 2200;   // R11

// NTC: Temperatur aus der Tabelle ntc_table.h (erzeugt mit tools/gen_ntc_table.py).
// Die Tabelle gilt für 10 kOhm / B = 3435 und Pull-up 10 kOhm an VDD; die ADC-Referenz ist
// ebenfalls VDD, die Versorgungsspannung kürzt sich heraus.
constexpr uint16_t NTC_RAW_OPEN = 981;        // Rohwert darüber: Unterbrechung (entspricht ca. 4,7 V, unter -40 Grad)
constexpr uint16_t NTC_RAW_SHORT = 31;        // Rohwert darunter: Kurzschluss (entspricht ca. 0,15 V, über 140 Grad)

constexpr unsigned long SENSE_INTERVAL_MS = 200;   // Spannung und NTC
constexpr unsigned long AIR_INTERVAL_MS = 2100;    // DHT11 (Datenblatt: nicht schneller)

// ---------------------------------------------------------------------------
// Spannungsfreigabe "Motor läuft" (voltage.ino)
// Werte aus den Logs abgeleitet (Ruhe ca. 12.2 V, Motor läuft ca. 14.2 V), am Fahrzeug zu bestätigen
// ---------------------------------------------------------------------------
constexpr uint16_t VOLTAGE_ON_MV = MV(13.5f);
constexpr uint16_t VOLTAGE_OFF_MV = MV(13.0f);
constexpr uint16_t VOLTAGE_RESTART_MAX_MV = MV(15.0f);
constexpr uint16_t VOLTAGE_MAX_MV = MV(15.5f);
constexpr unsigned long VOLTAGE_ON_DELAY_MS = 5000;
constexpr unsigned long VOLTAGE_OFF_DELAY_MS = 3000;

// ---------------------------------------------------------------------------
// Wetterbedingung Variante A: kalt und feucht, mit Hysterese
// ---------------------------------------------------------------------------
constexpr int16_t WX_TEMP_ON_DC = DC(8.0f);        // ein bei Lufttemperatur <= Wert
constexpr int16_t WX_TEMP_HYST_DC = DC(1.0f);      // aus bei Lufttemperatur >= Wert + Hysterese
constexpr int16_t WX_RH_ON_D10 = PCT10(80.0f);     // ein bei Luftfeuchte >= Wert (früher Testwert 30 %)
constexpr int16_t WX_RH_HYST_D10 = PCT10(5.0f);    // aus bei Luftfeuchte <= Wert - Hysterese

// ---------------------------------------------------------------------------
// Solltemperatur der Logo-Platte: Kennlinie über der Außentemperatur
// (Stützpunkte, linear dazwischen; Startwerte konstant wie bisher, siehe docs/verhalten.md)
// ---------------------------------------------------------------------------
constexpr uint8_t SETPOINT_POINTS = 3;
constexpr int16_t SETPOINT_AIR_DC[SETPOINT_POINTS] = {DC(-20.0f), DC(-5.0f), DC(8.0f)};
constexpr int16_t SETPOINT_PLATE_DC[SETPOINT_POINTS] = {DC(19.5f), DC(19.5f), DC(19.5f)};
constexpr int16_t SETPOINT_MIN_DC = DC(2.0f);
constexpr int16_t SETPOINT_MAX_DC = DC(25.0f);
constexpr int16_t HEATER_HYST_DC = DC(0.5f);       // Zweipunktregelung: ein bei Soll - Wert, aus bei Soll + Wert

// ---------------------------------------------------------------------------
// Zeiten
// ---------------------------------------------------------------------------
constexpr unsigned long FAN_PRERUN_MS = 5000;      // Lüfter vor der Heizung
constexpr unsigned long FAN_AFTERRUN_MS = 30000;   // Lüfter-Nachlauf (vorläufig)
constexpr unsigned long WEATHER_LOST_MS = 10000;   // Wetter fällt weg bis Heizung aus
constexpr unsigned long SHUTDOWN_DELAY_MS = 20000; // ohne Spannungsfreigabe bis Selbstabschaltung (psu.ino)

// ---------------------------------------------------------------------------
// Fehlergrenzen (faults.ino)
// ---------------------------------------------------------------------------
constexpr int16_t HEATER_OVERTEMP_DC = DC(30.0f);  // Übertemperatur Heizelement (Code 4)
constexpr int16_t PLATE_OVERTEMP_DC = DC(40.0f);   // Übertemperatur Logo-Platte (Code 5)
constexpr uint8_t AIR_ERROR_LIMIT = 5;             // Fehlversuche des Luftsensors bis Code 1

// ---------------------------------------------------------------------------
// Fehler-LED
// ---------------------------------------------------------------------------
constexpr unsigned long LED_HEARTBEAT_MS = 100;    // Normalbetrieb: Wechsel alle 100 ms
constexpr unsigned long LED_BLINK_MS = 400;        // Fehlercode: Impuls an/aus
constexpr unsigned long LED_PAUSE_MS = 1000;       // Fehlercode: Pause nach den Impulsen

// ---------------------------------------------------------------------------
// EEPROM-Fehlerspeicher (faultlog.ino): Ringspeicher der letzten Fehler, 4 + 4 * 24 = 100 von 128 Byte
// ---------------------------------------------------------------------------
constexpr uint8_t FAULTLOG_ENTRIES = 24;
constexpr uint8_t FAULT_CODE_WATCHDOG = 6;         // Protokolleintrag nach einem Watchdog-Reset (kein Fehlerzustand)

// ---------------------------------------------------------------------------
// Debug-Ausgang (debug.ino): serielle Textausgabe am Pin PB2 (USART0 TX), nur senden, 8N1
// Auf der Platine V3.2 ist PB2 nur am Controller-Pad (Pin 12 des QFN-20) erreichbar.
// Im Normalbetrieb auf 0 setzen: dann entfällt der Code vollständig.
// ---------------------------------------------------------------------------
#define DEBUG_OUTPUT 1
constexpr uint32_t DEBUG_BAUD = 115200;
constexpr unsigned long DEBUG_STATUS_MS = 1000;    // Statuszeile im Abstand
constexpr uint8_t DEBUG_BUF_SIZE = 160;            // Sendepuffer (volle Zeichen werden verworfen, nie blockiert)

// ---------------------------------------------------------------------------
// Prüfstand
// ---------------------------------------------------------------------------
// Endlosschleife nach n ms Laufzeit, um den Watchdog zu testen (0 = aus). Nur für den Prüfstand!
#define TEST_HANG_AFTER_MS 0

// ---------------------------------------------------------------------------
// Watchdog
// ---------------------------------------------------------------------------
#define WATCHDOG_PERIOD WDT_PERIOD_2KCLK_gc        // ca. 2 s
