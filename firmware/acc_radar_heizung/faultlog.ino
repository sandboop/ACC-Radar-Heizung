// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

#include <avr/eeprom.h>

/*
  Fehlerspeicher im EEPROM (128 Byte beim ATtiny826): Ringspeicher der letzten Fehler. Er bleibt nach
  einem Neustart erhalten und lässt sich über den Debug-Ausgang auslesen (debug.ino).

  Aufbau:
    Adresse 0       Kennung (EE_MAGIC)
    Adresse 1       Formatversion
    Adresse 2       Index des nächsten Eintrags (0 bis FAULTLOG_ENTRIES - 1)
    Adresse 4 ...   FAULTLOG_ENTRIES Einträge zu je 4 Byte: Code, Zustand, Plattentemperatur, Bordspannung

  Schonender Umgang mit dem EEPROM (ca. 100000 Schreibzyklen je Zelle):
  - Es wird nur bei einem neuen Fehler und nach einem Watchdog-Reset geschrieben, nicht bei jedem Start.
  - Aufeinanderfolgende Watchdog-Einträge werden zu einem zusammengefasst.
  - eeprom_update_byte() schreibt nur geänderte Bytes.
  - Geschrieben wird erst nach dem Ansteuern der Ausgänge und höchstens ein Byte je Schleifendurchlauf,
    da ein EEPROM-Schreibzugriff den Controller einige Millisekunden anhalten kann.
*/

constexpr uint8_t EE_MAGIC = 0xA7;
constexpr uint8_t EE_VERSION = 1;
constexpr uint8_t EE_ADDR_MAGIC = 0;
constexpr uint8_t EE_ADDR_VERSION = 1;
constexpr uint8_t EE_ADDR_INDEX = 2;
constexpr uint8_t EE_ADDR_ENTRIES = 4;
constexpr uint8_t EE_ENTRY_SIZE = 4;
constexpr uint8_t EE_EMPTY = 0xFF;

static uint8_t fl_index = 0;           // Index des nächsten Eintrags
static FaultEntry fl_pending;          // Eintrag, der noch geschrieben werden muss
static uint8_t fl_step = 0;            // 0 = nichts zu schreiben, 1 bis 5 = Schreibschritt

static uint8_t ee_read(uint8_t address)
{
  return eeprom_read_byte((const uint8_t *)(uintptr_t)address);
}

static void ee_write(uint8_t address, uint8_t value)
{
  eeprom_update_byte((uint8_t *)(uintptr_t)address, value);
}

// Beim Start: Kennung prüfen, bei ungültigem Inhalt (neuer Controller) formatieren.
// Läuft vor dem Start des Watchdogs, das Formatieren dauert einige hundert Millisekunden.
void faultlog_init()
{
  if((ee_read(EE_ADDR_MAGIC) != EE_MAGIC) || (ee_read(EE_ADDR_VERSION) != EE_VERSION))
  {
    for(uint8_t i = 0; i < (FAULTLOG_ENTRIES * EE_ENTRY_SIZE); i++)
    {ee_write(EE_ADDR_ENTRIES + i, EE_EMPTY);}
    ee_write(EE_ADDR_INDEX, 0);
    ee_write(EE_ADDR_VERSION, EE_VERSION);
    ee_write(EE_ADDR_MAGIC, EE_MAGIC);
  }

  fl_index = ee_read(EE_ADDR_INDEX);
  if(fl_index >= FAULTLOG_ENTRIES)
  {fl_index = 0;}                      // beschädigter Index
}

// Liest den Eintrag "slot" (0 bis FAULTLOG_ENTRIES - 1). Gibt false zurück, wenn der Eintrag leer ist.
bool faultlog_read(uint8_t slot, FaultEntry &entry)
{
  const uint8_t base = EE_ADDR_ENTRIES + slot * EE_ENTRY_SIZE;
  entry.code = ee_read(base);
  entry.state = ee_read(base + 1);
  entry.t_plate_c = (int8_t)ee_read(base + 2);
  entry.vbat_100mv = ee_read(base + 3);
  return entry.code != EE_EMPTY;
}

// Index des nächsten Eintrags = Position des ältesten Eintrags (Reihenfolge beim Auslesen).
uint8_t faultlog_next_index()
{
  return fl_index;
}

// Merkt einen Eintrag zum Schreiben vor (geschrieben wird in faultlog_service()).
void faultlog_add(uint8_t code, uint8_t state_at_event)
{
  if(fl_step != 0)
  {return;}                            // es läuft noch ein Schreibvorgang

  int16_t t_plate_c = in.t_plate_dc / 10;
  if(t_plate_c < -128) {t_plate_c = -128;}
  if(t_plate_c > 127) {t_plate_c = 127;}
  uint16_t vbat = in.vbat_mv / 100;
  if(vbat > 255) {vbat = 255;}

  fl_pending.code = code;
  fl_pending.state = state_at_event;
  fl_pending.t_plate_c = (int8_t)t_plate_c;
  fl_pending.vbat_100mv = (uint8_t)vbat;
  fl_step = 1;
}

// Protokolliert einen Watchdog-Reset, aufeinanderfolgende werden zusammengefasst.
void faultlog_add_watchdog()
{
  const uint8_t last = (fl_index + FAULTLOG_ENTRIES - 1) % FAULTLOG_ENTRIES;
  FaultEntry previous;
  if(faultlog_read(last, previous) && (previous.code == FAULT_CODE_WATCHDOG))
  {return;}
  faultlog_add(FAULT_CODE_WATCHDOG, ST_START);
}

// Schreibt einen vorgemerkten Eintrag, höchstens ein Byte je Aufruf. Der Code wird zuletzt geschrieben,
// danach der Index: ein unterbrochener Schreibvorgang hinterlässt keinen halben Eintrag mit gültigem Code.
void faultlog_service()
{
  if(fl_step == 0)
  {return;}

  const uint8_t base = EE_ADDR_ENTRIES + fl_index * EE_ENTRY_SIZE;

  switch(fl_step)
  {
    case 1: ee_write(base + 1, fl_pending.state); break;
    case 2: ee_write(base + 2, (uint8_t)fl_pending.t_plate_c); break;
    case 3: ee_write(base + 3, fl_pending.vbat_100mv); break;
    case 4: ee_write(base, fl_pending.code); break;
    default:
      fl_index = (fl_index + 1) % FAULTLOG_ENTRIES;
      ee_write(EE_ADDR_INDEX, fl_index);
      fl_step = 0;
      return;
  }
  fl_step++;
}
