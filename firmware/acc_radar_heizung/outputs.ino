// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

/*
  Fehlercode per Blinken der Fehler-LED, ohne die Schleife zu blockieren.
  Ablauf: fault_code Impulse (LED_BLINK_MS an, LED_BLINK_MS aus), danach LED_PAUSE_MS Pause, dann von vorn.
*/
static void error_led_update(unsigned long now)
{
  static bool started = false;
  static unsigned int step = 0;               // gerade: LED an, ungerade: LED aus, ab blink_steps: Pause
  static unsigned long step_timer_ms = 0;

  const unsigned int blink_steps = (unsigned int)fault_code * 2;
  const unsigned long duration = (step < blink_steps) ? LED_BLINK_MS : LED_PAUSE_MS;

  if(started == false)
  {
    started = true;
    step = 0;
    step_timer_ms = now;
    digitalWrite(PIN_ERROR_LED, HIGH);
    return;
  }

  if((now - step_timer_ms) >= duration)
  {
    step_timer_ms = now;
    step++;
    if(step > blink_steps)
    {step = 0;}
    digitalWrite(PIN_ERROR_LED, ((step < blink_steps) && ((step % 2) == 0)) ? HIGH : LOW);
  }
}

// Normalbetrieb: schnelles Blinken zeigt, dass die Schleife läuft.
static void heartbeat_led_update(unsigned long now)
{
  static unsigned long timer_ms = 0;
  static bool level = false;

  if((now - timer_ms) >= LED_HEARTBEAT_MS)
  {
    timer_ms = now;
    level = !level;
    digitalWrite(PIN_ERROR_LED, level ? HIGH : LOW);
  }
}

// Sollwerte an die Pins schreiben. Die Heizung bekommt zusätzlich eine eigene Freigabeprüfung:
// nur im Zustand HEATING, ohne Fehler und mit Spannungsfreigabe.
void outputs_apply(unsigned long now)
{
  const bool heater_allowed = (fault_code == 0) && (state == ST_HEATING) && (voltage_release == true);

  digitalWrite(PIN_HEATER, (heater_cmd && heater_allowed) ? HIGH : LOW);
  digitalWrite(PIN_FAN, fan_cmd ? HIGH : LOW);
  digitalWrite(PIN_PSU_HOLD, psu_hold ? HIGH : LOW);

  if(fault_code != 0)
  {error_led_update(now);}
  else
  {heartbeat_led_update(now);}
}
