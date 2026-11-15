// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

/*
  Zustandsautomat (docs/verhalten.md, Abschnitt 3):
    START -> STANDBY -> FAN_PRERUN -> HEATING -> AFTERRUN -> STANDBY
    jeder Zustand -> FAULT (faults.ino)
  Heizen gilt nur im Zustand HEATING und nur mit Spannungsfreigabe (zusätzlich in outputs.ino abgesichert).
*/

// Solltemperatur der Logo-Platte aus der Kennlinie über der Außentemperatur
// (linear zwischen den Stützpunkten, außerhalb konstant, begrenzt auf SETPOINT_MIN_C/MAX_C).
int16_t setpoint_for(int16_t t_air_dc)
{
  int32_t sp = SETPOINT_PLATE_DC[SETPOINT_POINTS - 1];

  if(t_air_dc <= SETPOINT_AIR_DC[0])
  {
    sp = SETPOINT_PLATE_DC[0];
  }
  else if(t_air_dc < SETPOINT_AIR_DC[SETPOINT_POINTS - 1])
  {
    for(uint8_t i = 0; i < (SETPOINT_POINTS - 1); i++)
    {
      if(t_air_dc < SETPOINT_AIR_DC[i + 1])
      {
        const int32_t span_air = (int32_t)SETPOINT_AIR_DC[i + 1] - SETPOINT_AIR_DC[i];
        const int32_t span_plate = (int32_t)SETPOINT_PLATE_DC[i + 1] - SETPOINT_PLATE_DC[i];
        sp = SETPOINT_PLATE_DC[i] + (span_plate * ((int32_t)t_air_dc - SETPOINT_AIR_DC[i])) / span_air;
        break;
      }
    }
  }

  if(sp < SETPOINT_MIN_DC)
  {sp = SETPOINT_MIN_DC;}
  if(sp > SETPOINT_MAX_DC)
  {sp = SETPOINT_MAX_DC;}
  return (int16_t)sp;
}

// Wetterbedingung Variante A: kalt und feucht, mit Hysterese. Ohne gültigen Luftsensor: nicht erfüllt.
static bool weather_update()
{
  if(in.air_valid == false)
  {
    weather_active = false;
    return false;
  }

  if(weather_active == false)
  {
    if((in.t_air_dc <= WX_TEMP_ON_DC) && (in.rh_air_d10 >= WX_RH_ON_D10))
    {weather_active = true;}
  }
  else
  {
    if((in.t_air_dc >= (WX_TEMP_ON_DC + WX_TEMP_HYST_DC)) || (in.rh_air_d10 <= (WX_RH_ON_D10 - WX_RH_HYST_D10)))
    {weather_active = false;}
  }
  return weather_active;
}

static void enter_state(State next, unsigned long now)
{
  debug_state_change((uint8_t)state, (uint8_t)next);
  state = next;
  state_since_ms = now;
}

void statemachine_update(unsigned long now)
{
  if(state == ST_FAULT)
  {
    heater_cmd = false;
    fan_cmd = fault_fan_was_on && ((now - fault_since_ms) < FAN_AFTERRUN_MS);   // Nachlauf, danach aus
    return;
  }

  const bool wx = weather_update();
  const bool run_ok = (voltage_release == true) && (wx == true);

  switch(state)
  {
    case ST_START:
      heater_cmd = false;
      fan_cmd = false;
      if(in.ntc_valid == true)
      {enter_state(ST_STANDBY, now);}
      break;

    case ST_STANDBY:
      heater_cmd = false;
      fan_cmd = false;
      if(run_ok == true)
      {enter_state(ST_FAN_PRERUN, now);}
      break;

    case ST_FAN_PRERUN:
      heater_cmd = false;
      fan_cmd = true;
      if(run_ok == false)
      {
        fan_cmd = false;
        enter_state(ST_STANDBY, now);
      }
      else if((now - state_since_ms) >= FAN_PRERUN_MS)
      {
        weather_lost = false;
        enter_state(ST_HEATING, now);
      }
      break;

    case ST_HEATING:
    {
      fan_cmd = true;

      if(voltage_release == false)     // Motor aus: Heizung sofort aus
      {
        heater_cmd = false;
        enter_state(ST_AFTERRUN, now);
        break;
      }

      if(wx == true)
      {
        weather_lost = false;
      }
      else if(weather_lost == false)
      {
        weather_lost = true;           // Wetterbedingung fällt weg: noch WEATHER_LOST_MS weiter regeln
        weather_lost_ms = now;
      }
      else if((now - weather_lost_ms) >= WEATHER_LOST_MS)
      {
        heater_cmd = false;
        weather_lost = false;
        enter_state(ST_AFTERRUN, now);
        break;
      }

      const int16_t sp = setpoint_for(in.t_air_dc);    // Zweipunktregelung mit Hysterese
      if(in.t_plate_dc <= (sp - HEATER_HYST_DC))
      {heater_cmd = true;}
      else if(in.t_plate_dc >= (sp + HEATER_HYST_DC))
      {heater_cmd = false;}
      break;
    }

    case ST_AFTERRUN:
      heater_cmd = false;
      fan_cmd = true;
      if((now - state_since_ms) >= FAN_AFTERRUN_MS)
      {
        fan_cmd = false;
        enter_state(ST_STANDBY, now);
      }
      break;

    default:
      break;
  }
}
