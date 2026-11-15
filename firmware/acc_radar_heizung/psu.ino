// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

/*
  Selbsthaltung und Selbstabschaltung des Netzteils (PA4, siehe docs/hardware-analyse-v3.2.md)

  Der Enable-Pin des Netzteils (LMR14020) hängt an einem Teiler aus der Bordspannung und über
  R12 (56 kOhm) an PA4:
  - PA4 = HIGH: Netzteil bleibt an (Selbsthaltung), auch bei sinkender Bordspannung.
  - PA4 = LOW:  Netzteil schaltet ab (bei normaler Bordspannung unter ca. 15 V sicher).
  - PA4 hochohmig (Controller aus): das Netzteil startet erst oberhalb von ca. 13.3 V (typisch).

  Deshalb:
  - PA4 wird beim Start sofort auf HIGH gelegt (setup()).
  - Der Controller schaltet sich nur ab, wenn der Motor nicht läuft (keine Spannungsfreigabe),
    Lüfter und Heizung aus sind (Nachlauf beendet) und SHUTDOWN_DELAY_MS abgelaufen ist. Im Fehlerfall bleibt er an (Fehlercode per LED),
    solange der Motor läuft; sonst würde die Abschaltung über PA4 den Fehler löschen und das
    Gerät bei laufendem Motor sofort neu starten.
  - Nach dem Abschalten ist der Controller stromlos; ein Wiedereinschalten ist nicht nötig.
*/
void psu_update(unsigned long now)
{
  if((voltage_release == true) || (fan_cmd == true) || (heater_cmd == true))
  {
    psu_timing = false;                // Motor läuft oder Lüfter/Heizung noch aktiv (Nachlauf): Netzteil gehalten
    return;
  }

  if(psu_timing == false)
  {
    psu_timing = true;
    psu_timer_ms = now;
  }
  else if((now - psu_timer_ms) >= SHUTDOWN_DELAY_MS)
  {
    if(psu_hold == true)
    {debug_event("PSU_OFF");}          // einmalig melden, danach verliert der Controller die Versorgung
    psu_hold = false;                  // PA4 = LOW: Netzteil schaltet ab, der Controller verliert die Versorgung
  }
}
