// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

// Temperatur eines NTC in 0,1 Grad aus dem ADC-Rohwert (Tabelle ntc_table.h, lineare Interpolation).
// Die Tabelle fällt mit steigender Temperatur. Außerhalb der Tabelle gilt der Randwert.
static int16_t ntc_deci_celsius(uint16_t raw)
{
  if(raw >= NTC_TABLE_RAW[0])
  {return NTC_TABLE_T_START_DC;}

  for(uint8_t i = 1; i < NTC_TABLE_SIZE; i++)
  {
    if(raw >= NTC_TABLE_RAW[i])
    {
      const int32_t span = (int32_t)NTC_TABLE_RAW[i - 1] - (int32_t)NTC_TABLE_RAW[i];
      const int32_t offset = (int32_t)NTC_TABLE_RAW[i - 1] - (int32_t)raw;
      return (int16_t)(NTC_TABLE_T_START_DC + (int32_t)(i - 1) * NTC_TABLE_STEP_DC + (offset * NTC_TABLE_STEP_DC) / span);
    }
  }

  return (int16_t)(NTC_TABLE_T_START_DC + (int32_t)(NTC_TABLE_SIZE - 1) * NTC_TABLE_STEP_DC);
}

// Bordspannung in mV aus dem ADC-Rohwert (Spannungsteiler R10/R11, Referenz ADC_VREF_MV).
static uint16_t vbat_millivolt(uint16_t raw)
{
  const uint32_t pin_mv = ((uint32_t)raw * ADC_VREF_MV) / ADC_MAX;
  return (uint16_t)((pin_mv * (DIV_R_TOP_OHM + DIV_R_BOTTOM_OHM)) / DIV_R_BOTTOM_OHM);
}

// Bordspannung und NTC alle SENSE_INTERVAL_MS. Die Spannung wird auch im Fehlerzustand
// weiter gelesen, damit die Selbstabschaltung bei Motor aus funktioniert (psu.ino).
void sensors_update(unsigned long now)
{
  if((now - sense_timer_ms) >= SENSE_INTERVAL_MS)
  {
    sense_timer_ms = now;

    in.vbat_mv = vbat_millivolt(analogRead(PIN_VBAT));
    voltage_update(now);

    if(fault_code == 0)                // NTC und Luftsensor nur lesen, solange kein Fehler vorliegt
    {
      in.raw_heater = analogRead(PIN_NTC_HEATER);
      in.raw_plate = analogRead(PIN_NTC_PLATE);
      in.t_heater_dc = ntc_deci_celsius(in.raw_heater);
      in.t_plate_dc = ntc_deci_celsius(in.raw_plate);
      in.ntc_valid = true;             // ab jetzt sind die Rohwerte echte Messwerte (faults.ino)
    }
  }

  if(fault_code == 0)
  {air_update(now);}
}
