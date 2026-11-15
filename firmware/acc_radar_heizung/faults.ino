// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

/*
  Fehlererkennung. Ein Fehler wird festgehalten (nur ein Neustart löscht ihn) und die
  Heizung wird sofort direkt am Pin abgeschaltet, nicht erst über outputs_apply(); so wirkt
  die Abschaltung im selben Schleifendurchlauf. Ein laufender Lüfter läuft für den Nachlauf
  weiter (statemachine.ino).

  Codes: 1 Luftsensor, 2 NTC Heizelement, 3 NTC Logo-Platte,
         4 Übertemperatur Heizelement, 5 Übertemperatur Logo-Platte
*/

// NTC-Rohwert außerhalb des gültigen Bereichs: Unterbrechung (hoch) oder Kurzschluss (niedrig).
// Der Kurzschluss wird erst nach der ersten Messung geprüft, sonst löst der Startwert 0 aus.
static bool ntc_raw_bad(uint16_t raw)
{
  return (raw >= NTC_RAW_OPEN) || ((in.ntc_valid == true) && (raw <= NTC_RAW_SHORT));
}

void fault_trip(uint8_t code, unsigned long now)
{
  if(fault_code != 0)
  {return;}

  const uint8_t previous_state = (uint8_t)state;

  fault_code = code;
  fault_since_ms = now;
  fault_fan_was_on = fan_cmd;
  state = ST_FAULT;
  state_since_ms = now;

  heater_cmd = false;
  digitalWrite(PIN_HEATER, LOW);       // sofort am Pin

  debug_fault(code);                   // erst danach: Ausgabe und Fehlerspeicher
  debug_state_change(previous_state, ST_FAULT);
  faultlog_add(code, previous_state);
}

void faults_check(unsigned long now)
{
  if(fault_code != 0)
  {return;}

  if(in.air_errors >= AIR_ERROR_LIMIT)
  {fault_trip(1, now);}                // Luftsensor liefert keine gültigen Werte
  else if(ntc_raw_bad(in.raw_heater))
  {fault_trip(2, now);}                // NTC Heizelement defekt (Unterbrechung oder Kurzschluss)
  else if(ntc_raw_bad(in.raw_plate))
  {fault_trip(3, now);}                // NTC Logo-Platte defekt (Unterbrechung oder Kurzschluss)
  else if(in.t_heater_dc >= HEATER_OVERTEMP_DC)
  {fault_trip(4, now);}                // Übertemperatur Heizelement
  else if(in.t_plate_dc >= PLATE_OVERTEMP_DC)
  {fault_trip(5, now);}                // Übertemperatur Logo-Platte
}
