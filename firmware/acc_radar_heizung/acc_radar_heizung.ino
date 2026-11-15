// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors
//
// ACC-Radar-Heizung: Heizung für das VW-Logo über dem ACC-/Front-Assist-Radar.
// Ablauf und Werte: docs/verhalten.md, Pins und Konstanten: config.h.
//
// Dateien (Arduino-Tabs):
//   config.h / types.h   Konstanten, Zustände, Messwert-Struktur
//   ntc_table.h          NTC-Tabelle (erzeugt mit tools/gen_ntc_table.py)
//   sensors.ino          Bordspannung und NTC einlesen
//   air_dht11.ino        Luftsensor (DHT11)
//   voltage.ino          Spannungsfreigabe "Motor läuft"
//   faults.ino           Fehlererkennung und sofortiges Abschalten
//   statemachine.ino     Zustandsautomat, Wetterbedingung, Regelung
//   psu.ino              Netzteil halten / selbst abschalten
//   outputs.ino          Ausgänge und Fehler-LED
//   faultlog.ino         EEPROM-Fehlerspeicher
//   debug.ino            Debug-Ausgang (seriell, PB2)

#include <avr/wdt.h>

#include "config.h"
#include "types.h"
#include "ntc_table.h"

#if AIR_DRIVER == AIR_DRIVER_LIBRARY
#include "DHT.h"
#include <Adafruit_Sensor.h>
#endif

// ---------------------------------------------------------------------------
// Globale Zustände (alle Tabs greifen darauf zu)
// ---------------------------------------------------------------------------
Sensors in;                          // aktuelle Messwerte
State state = ST_START;              // Zustand des Ablaufs
unsigned long state_since_ms = 0;    // Zeitpunkt des letzten Zustandswechsels

uint8_t fault_code = 0;              // 0 = kein Fehler, sonst Fehlercode 1 bis 5
unsigned long fault_since_ms = 0;
bool fault_fan_was_on = false;       // lief der Lüfter beim Fehler (für den Nachlauf)

bool heater_cmd = false;             // Sollwert Heizelement
bool fan_cmd = false;                // Sollwert Lüfter
bool psu_hold = true;                // Sollwert PA4 (HIGH = Netzteil halten)

bool voltage_release = false;        // Bordspannung im zulässigen Fenster (Start: gesperrt)
bool voltage_timing = false;
unsigned long voltage_timer_ms = 0;

bool weather_active = false;         // Wetterbedingung erfüllt (mit Hysterese)
bool weather_lost = false;           // Wetterbedingung fällt gerade weg (Heizung regelt noch WEATHER_LOST_MS weiter)
unsigned long weather_lost_ms = 0;

bool psu_timing = false;
unsigned long psu_timer_ms = 0;

unsigned long sense_timer_ms = 0;
unsigned long air_timer_ms = 0;


void setup()
{
  pinMode(PIN_PSU_HOLD, OUTPUT);       // Als Erstes: Netzteil halten (PA4 = HIGH), sonst fällt die Versorgung wieder weg
  digitalWrite(PIN_PSU_HOLD, HIGH);

  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_HEATER, OUTPUT);
  pinMode(PIN_ERROR_LED, OUTPUT);
  pinMode(PIN_VBAT, INPUT);
  pinMode(PIN_NTC_HEATER, INPUT);
  pinMode(PIN_NTC_PLATE, INPUT);
  digitalWrite(PIN_FAN, LOW);
  digitalWrite(PIN_HEATER, LOW);
  digitalWrite(PIN_ERROR_LED, HIGH);

  air_init();

  const uint8_t reset_flags = GPIOR0;  // Reset-Ursache, vom Core beim Start gesichert
  faultlog_init();                     // EEPROM-Fehlerspeicher prüfen (vor dem Start des Watchdogs)
  if((reset_flags & RSTCTRL_WDRF_bm) != 0)
  {faultlog_add_watchdog();}           // Watchdog-Reset protokollieren
  debug_init();
  debug_banner(reset_flags);           // Startmeldung und Inhalt des Fehlerspeichers

  wdt_enable(WATCHDOG_PERIOD);         // Watchdog zuletzt starten, damit die Initialisierung ihn nicht auslöst
}


void loop()
{
  const unsigned long now = millis();

  sensors_update(now);                 // Messwerte
  faults_check(now);                   // Sicherheit direkt nach dem Einlesen: ein Fehler schaltet die Heizung sofort ab
  statemachine_update(now);            // Ablauf und Regelung
  psu_update(now);                     // Netzteil halten / selbst abschalten
  outputs_apply(now);                  // Ausgänge und Fehler-LED
  debug_update(now);                   // Debug-Ausgabe (non-blocking)
  faultlog_service();                  // EEPROM erst nach den Ausgängen beschreiben (höchstens ein Byte je Durchlauf)

#if TEST_HANG_AFTER_MS > 0
  if(now >= TEST_HANG_AFTER_MS)
  {while(true) {}}                     // Prüfstand: Watchdog-Test, der Controller setzt sich nach ca. 2 s zurück
#endif

  delay(1);
  wdt_reset();                         // nur hier zurücksetzen: hängt die Schleife, löst der Watchdog aus
}
