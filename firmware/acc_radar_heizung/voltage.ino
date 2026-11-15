// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

/*
  Bordspannungs-Freigabe (voltage_release)

  Die Heizung darf nur laufen, wenn der Motor läuft. Aus den Messreihen in
  logs/ folgt: Ruhespannung ca. 12.2 V, Motor läuft ca. 14.2 V. Eine einzelne
  Schwelle unter 12.2 V (früher 11.7 V) unterscheidet das nicht.

  - Einschalten: Spannung zwischen VOLTAGE_ON_MV und VOLTAGE_RESTART_MAX_MV, und
    das ununterbrochen für VOLTAGE_ON_DELAY_MS.
  - Ausschalten: Spannung unter VOLTAGE_OFF_MV für VOLTAGE_OFF_DELAY_MS.
  - Überspannung (über VOLTAGE_MAX_MV): sofort keine Freigabe.
  - Beim Start ist die Freigabe gesperrt.
  - Zeitvergleiche über Differenzen, damit der millis()-Überlauf keine Rolle spielt.
*/
void voltage_update(unsigned long now)
{
  const uint16_t v = in.vbat_mv;

  if(voltage_release == false)
  {
    if((v >= VOLTAGE_ON_MV) && (v <= VOLTAGE_RESTART_MAX_MV))
    {
      if(voltage_timing == false)
      {
        voltage_timing = true;
        voltage_timer_ms = now;
      }
      else if((now - voltage_timer_ms) >= VOLTAGE_ON_DELAY_MS)
      {
        voltage_release = true;
        voltage_timing = false;
      }
    }
    else
    {
      voltage_timing = false;
    }
  }
  else
  {
    if(v > VOLTAGE_MAX_MV)
    {
      voltage_release = false;         // Überspannung: sofort sperren
      voltage_timing = false;
    }
    else if(v < VOLTAGE_OFF_MV)
    {
      if(voltage_timing == false)
      {
        voltage_timing = true;
        voltage_timer_ms = now;
      }
      else if((now - voltage_timer_ms) >= VOLTAGE_OFF_DELAY_MS)
      {
        voltage_release = false;
        voltage_timing = false;
      }
    }
    else
    {
      voltage_timing = false;
    }
  }
}
