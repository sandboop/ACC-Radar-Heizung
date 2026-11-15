// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

/*
  Debug-Ausgang: Textausgabe für den Prüfstand über USART0 TX (Pin PB2), 115200 Baud, 8N1, nur senden.
  Anschluss: PB2 (Pin 12 des QFN-20) und Masse an einen USB-Seriell-Adapter (TTL, 5 V ok).

  - Die Register des USART werden direkt gesetzt, die Serial-Klasse des Cores wird nicht benötigt.
  - Alle Ausgaben gehen in einen Ringpuffer; debug_update() schickt die Zeichen non-blocking ab. Ist der
    Puffer voll, werden Zeichen verworfen: der Debug-Ausgang kann die Regelung nie anhalten.
  - Ausgaben: Startmeldung mit Reset-Ursache, Inhalt des EEPROM-Fehlerspeichers, jeder Zustandswechsel,
    jeder Fehler und einmal pro Sekunde eine Statuszeile.
  - Mit DEBUG_OUTPUT = 0 in config.h entfällt der gesamte Code.

  Statuszeile (Felder durch Leerzeichen getrennt):
    t=<ms> s=<Zustand> f=<Fehlercode> v=<mV> rel=<Spannungsfreigabe> air=<Grad>/<%rF> e=<Luftsensor-Fehler>
    plate=<Grad> heat=<Grad> sp=<Soll Grad> H=<Heizung> F=<Lüfter> psu=<PA4>
*/

#if DEBUG_OUTPUT

static const char * const DEBUG_STATE_NAMES[] = {"START", "STANDBY", "FAN_PRERUN", "HEATING", "AFTERRUN", "FAULT"};

static char dbg_buf[DEBUG_BUF_SIZE];
static uint8_t dbg_head = 0;
static uint8_t dbg_tail = 0;
static unsigned long dbg_status_ms = 0;
static uint8_t dbg_dump_pos = 0;               // Ausgabe des Fehlerspeichers: nächster Eintrag
static bool dbg_dump_active = false;

static uint8_t dbg_free()
{
  return (uint8_t)((dbg_tail + DEBUG_BUF_SIZE - dbg_head - 1) % DEBUG_BUF_SIZE);
}

static void dbg_put(char c)
{
  const uint8_t next = (uint8_t)((dbg_head + 1) % DEBUG_BUF_SIZE);
  if(next == dbg_tail)
  {return;}                                    // Puffer voll: Zeichen verwerfen
  dbg_buf[dbg_head] = c;
  dbg_head = next;
}

static void dbg_str(const char *s)
{
  while(*s != '\0')
  {dbg_put(*s++);}
}

static void dbg_int(int32_t value)
{
  if(value < 0)
  {
    dbg_put('-');
    value = -value;
  }
  char digits[11];
  uint8_t n = 0;
  do
  {
    digits[n++] = (char)('0' + (value % 10));
    value /= 10;
  } while(value > 0);
  while(n > 0)
  {dbg_put(digits[--n]);}
}

// Wert in Zehnteln, z. B. -48 -> "-4.8"
static void dbg_tenths(int16_t value)
{
  if(value < 0)
  {
    dbg_put('-');
    value = -value;
  }
  dbg_int(value / 10);
  dbg_put('.');
  dbg_put((char)('0' + (value % 10)));
}

static void dbg_state(uint8_t s)
{
  if(s <= ST_FAULT)
  {dbg_str(DEBUG_STATE_NAMES[s]);}
  else
  {dbg_put('?');}
}

static void dbg_hex(uint8_t value)
{
  const char *hex = "0123456789ABCDEF";
  dbg_str("0x");
  dbg_put(hex[value >> 4]);
  dbg_put(hex[value & 0x0F]);
}

void debug_init()
{
  PORTB.OUTSET = PIN2_bm;                      // Leitung im Ruhezustand High
  PORTB.DIRSET = PIN2_bm;                      // PB2 = USART0 TX (Standardzuordnung)
  USART0.BAUD = (uint16_t)((4UL * F_CPU + (DEBUG_BAUD / 2)) / DEBUG_BAUD);
  USART0.CTRLC = USART_CHSIZE_8BIT_gc;
  USART0.CTRLB = USART_TXEN_bm;
}

// Startmeldung: Version, Reset-Ursache (Reset-Flags des Cores aus GPIOR0) und der Fehlerspeicher.
void debug_banner(uint8_t reset_flags)
{
  dbg_str("\r\nACC-Radar-Heizung ");
  dbg_str(FIRMWARE_VERSION);
  dbg_str(" reset=");
  dbg_hex(reset_flags);
  if((reset_flags & RSTCTRL_WDRF_bm) != 0)
  {dbg_str(" WATCHDOG");}
  if((reset_flags & RSTCTRL_BORF_bm) != 0)
  {dbg_str(" BROWNOUT");}
  if((reset_flags & RSTCTRL_PORF_bm) != 0)
  {dbg_str(" POWERON");}
  dbg_str("\r\n");
  dbg_dump_pos = 0;
  dbg_dump_active = true;
}

// Zustandswechsel melden
void debug_state_change(uint8_t from, uint8_t to)
{
  dbg_str("EVENT ");
  dbg_state(from);
  dbg_str("->");
  dbg_state(to);
  dbg_str("\r\n");
}

void debug_fault(uint8_t code)
{
  dbg_str("FAULT code=");
  dbg_int(code);
  dbg_str("\r\n");
}

void debug_event(const char *text)
{
  dbg_str("EVENT ");
  dbg_str(text);
  dbg_str("\r\n");
}

// Pumpt Zeichen in den USART, gibt den Fehlerspeicher Eintrag für Eintrag aus und schreibt die Statuszeile.
void debug_update(unsigned long now)
{
  while(((USART0.STATUS & USART_DREIF_bm) != 0) && (dbg_tail != dbg_head))
  {
    USART0.TXDATAL = (uint8_t)dbg_buf[dbg_tail];
    dbg_tail = (uint8_t)((dbg_tail + 1) % DEBUG_BUF_SIZE);
  }

  if((dbg_dump_active == true) && (dbg_free() > 50))
  {
    if(dbg_dump_pos >= FAULTLOG_ENTRIES)
    {
      dbg_dump_active = false;
    }
    else
    {
      const uint8_t slot = (uint8_t)((faultlog_next_index() + dbg_dump_pos) % FAULTLOG_ENTRIES);   // ältester zuerst
      FaultEntry e;
      if(faultlog_read(slot, e) == true)
      {
        dbg_str("LOG slot=");
        dbg_int(slot);
        dbg_str(" code=");
        dbg_int(e.code);
        dbg_str(" state=");
        dbg_state(e.state);
        dbg_str(" plate=");
        dbg_int(e.t_plate_c);
        dbg_str(" v=");
        dbg_tenths(e.vbat_100mv);
        dbg_str("\r\n");
      }
      dbg_dump_pos++;
    }
  }

  if(((now - dbg_status_ms) >= DEBUG_STATUS_MS) && (dbg_free() > 120))
  {
    dbg_status_ms = now;
    dbg_str("t=");
    dbg_int((int32_t)now);
    dbg_str(" s=");
    dbg_state(state);
    dbg_str(" f=");
    dbg_int(fault_code);
    dbg_str(" v=");
    dbg_int(in.vbat_mv);
    dbg_str(" rel=");
    dbg_put(voltage_release ? '1' : '0');
    dbg_str(" air=");
    if(in.air_valid == true)
    {
      dbg_tenths(in.t_air_dc);
      dbg_put('/');
      dbg_tenths(in.rh_air_d10);
    }
    else
    {dbg_str("--");}
    dbg_str(" e=");
    dbg_int(in.air_errors);
    dbg_str(" plate=");
    dbg_tenths(in.t_plate_dc);
    dbg_str(" heat=");
    dbg_tenths(in.t_heater_dc);
    dbg_str(" sp=");
    dbg_tenths(setpoint_for(in.t_air_dc));
    dbg_str(" H=");
    dbg_put(heater_cmd ? '1' : '0');
    dbg_str(" F=");
    dbg_put(fan_cmd ? '1' : '0');
    dbg_str(" psu=");
    dbg_put(psu_hold ? '1' : '0');
    dbg_str("\r\n");
  }
}

#else   // DEBUG_OUTPUT = 0: alle Aufrufe entfallen

void debug_init() {}
void debug_banner(uint8_t) {}
void debug_state_change(uint8_t, uint8_t) {}
void debug_fault(uint8_t) {}
void debug_event(const char *) {}
void debug_update(unsigned long) {}

#endif
