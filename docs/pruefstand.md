# Prüfstand: Checkliste für den ersten Test der Firmware

Diese Liste führt durch den Test der Firmware (Version
0.4.0-dev) auf der Platine V3.2 **ohne Fahrzeug**. Jeder Punkt hat ein erwartetes Ergebnis; in
die Spalte "Ergebnis" trägst du `ok`, `Abweichung` oder die Messwerte ein. Ergebnisse und Auffälligkeiten
gehören als Kommentar in Issue 1 (Prüfstandtest).

Alle Werte stammen aus den Entwurfsunterlagen und sind **nicht** erprobt. Weicht etwas ab, ist das ein
Befund, kein Fehler deinerseits. Siehe auch [`verhalten.md`](verhalten.md) (Soll-Verhalten) und
[`../DISCLAIMER.md`](../DISCLAIMER.md).

## 0. Sicherheit

- **Heizelement nicht anschließen.** Am Ausgang `2+`/`2-` einen **Lastwiderstand** verwenden, der nicht
  brennbar ist und nicht aufliegt (Kühlkörper, Abstand). Beispiel 4,7 Ω, mindestens 50 W: bei 14,2 V
  ca. 3 A und 43 W; die Last wird **heiß**. Betriebsdauer kurz halten.
- Netzteil mit **Strombegrenzung** (zuerst 0,5 A nur für die Elektronik, für den Lasttest bis 5 A).
- Gerät nicht unbeaufsichtigt laufen lassen. Kein Test im oder am Fahrzeug, bevor dieser Prüfstand
  vollständig bestanden ist.
- Ein defektes oder falsch gelötetes Bauteil kann zu Hitze führen. Bei Geruch oder Rauch sofort trennen.

## 1. Material

| Was | Wofür |
|---|---|
| Labornetzteil 10 bis 16 V, regelbar, Strombegrenzung | Bordspannung nachbilden |
| Multimeter | Spannungen und Widerstände prüfen |
| Lastwiderstand ca. 4,7 Ω / 50 W | Heizelement-Ersatz |
| Lüfter oder 100-Ω-Widerstand | Lüfter-Ersatz (`OUT1`) |
| Widerstände oder Potentiometer (siehe Abschnitt 2) | NTC-Ersatz an `IN1` und `IN2` |
| DHT11 an `IN3` | Luftsensor |
| USB-Seriell-Adapter (TTL, 3,3 oder 5 V) | Debug-Ausgang lesen |
| Dünner Draht (Kynar) am Controller-Pad PB2 (Pin 12 des QFN-20) und Masse | Debug-Ausgang (auf V3.2 nur dort erreichbar) |
| Arduino IDE 2 mit megaTinyCore, UPDI-Programmer | Flashen |
| optional: Oszilloskop oder LED-Prüfer | Schaltflanken, Watchdog |

## 2. Ersatzwiderstände für die NTC (10 kΩ, B = 3435)

Statt echter NTC einen Widerstand an `IN1` (Heizelement) bzw. `IN2` (Logo-Platte) legen. Die Anzeige
(`plate=`, `heat=`) soll dem Wert in der letzten Spalte entsprechen (±0,5 K).

| Temperatur | Widerstand | ADC-Rohwert | Erwartete Anzeige |
|---|---|---|---|
| −20 °C | 77,5 kΩ | 906 | −20,0 °C |
| 0 °C | 28,7 kΩ | 759 | 0,0 °C |
| 8 °C | 20,1 kΩ | 683 | 7,9 °C |
| 15 °C | 14,9 kΩ | 612 | 15,0 °C |
| 19,5 °C | 12,4 kΩ | 567 | 19,5 °C |
| 20 °C | 12,2 kΩ | 562 | 20,0 °C |
| 25 °C | 10,0 kΩ | 512 | 25,0 °C |
| 30 °C | 8,27 kΩ | 463 | 30,0 °C |
| 40 °C | 5,76 kΩ | 374 | 40,0 °C |
| 60 °C | 2,98 kΩ | 235 | 60,0 °C |
| offen (kein NTC) | ∞ | ≥ 981 | **Fehler** |
| Kurzschluss | 0 Ω | ≤ 31 | **Fehler** |

Ein Potentiometer (z. B. 100 kΩ mit Festwiderstand) erlaubt, Werte fließend einzustellen; vorher den
Widerstand mit dem Multimeter prüfen. Temperaturen unter ca. −39 °C zählen als Unterbrechung.

## 3. Aufbau und Flashen

1. `firmware/acc_radar_heizung` in der Arduino IDE öffnen (Board: megaTinyCore, ATtiny826, UPDI).
2. Bauen (`arduino-cli`-Befehl in [`../firmware/README.md`](../firmware/README.md)); der Build muss ohne Warnungen laufen.
3. Für den Test im Zimmer in `config.h` die **Testkonfiguration** setzen (danach wieder zurücksetzen!):

   | Konstante | Wert normal | Wert Test |
   |---|---|---|
   | `WX_TEMP_ON_DC` | `DC(8.0f)` | `DC(30.0f)` |
   | `WX_RH_ON_D10` | `PCT10(80.0f)` | `PCT10(20.0f)` |

4. Flashen, Terminal am Seriell-Adapter öffnen: **115200 Baud, 8N1**.
5. Die Startmeldung erscheint: `ACC-Radar-Heizung 0.4.0-dev reset=0x.. POWERON`. Danach einmal pro Sekunde
   eine Statuszeile (Felder siehe [`../firmware/README.md`](../firmware/README.md)).

## 4. Tests

Die Spalte "Soll" gilt mit Testkonfiguration. `rel` ist die Spannungsfreigabe, `s` der Zustand.

### 4.1 Start und Anzeige

| Nr. | Test | Soll | Ergebnis |
|---|---|---|---|
| T01 | Netzteil 14,2 V einschalten | Startmeldung mit `reset=0x.. POWERON`, danach Statuszeilen; Fehler-LED blinkt schnell (5 Hz) | ☐ |
| T02 | Heizelement- und Lüfterausgang im Stand | Lastwiderstand und Lüfter bleiben aus, solange `s=START`/`STANDBY` | ☐ |
| T03 | Strom bei 14,2 V im Leerlauf | notieren (Ruhestrom des Controllers mit PSU-Haltung) | ☐ |

### 4.2 Spannungsfreigabe (Audit F-01, F-06)

Ausgangslage: gültige Luftwerte (T/rF im Testbereich), NTC-Ersatz 25 °C an beiden Eingängen.

| Nr. | Test | Soll | Ergebnis |
|---|---|---|---|
| T10 | 12,2 V einstellen (Ruhespannung) | `rel=0`, `s=STANDBY`, kein Lüfter | ☐ |
| T11 | auf 14,2 V stellen | nach 5 s `rel=1`, Wechsel `STANDBY->FAN_PRERUN` | ☐ |
| T12 | Spannung auf 12,5 V senken | nach 3 s `rel=0`; Heizung sofort aus | ☐ |
| T13 | wieder 14,2 V | nach 5 s `rel=1` | ☐ |
| T14 | auf 15,8 V (über 15,5 V) | **sofort** `rel=0`; Heizung aus | ☐ |
| T15 | zurück auf 15,2 V | `rel=0` bleibt (Wiederfreigabe erst bis 15,0 V) | ☐ |
| T16 | auf 14,8 V | nach 5 s `rel=1` | ☐ |
| T17 | Messgenauigkeit: bei 12,0 / 13,5 / 14,2 / 15,5 V `v=` mit dem Multimeter vergleichen | Abweichung unter ca. 100 mV (notieren) | ☐ |

### 4.3 NTC und Fehler (Audit F-02, F-03)

| Nr. | Test | Soll | Ergebnis |
|---|---|---|---|
| T20 | Ersatzwiderstände nach Abschnitt 2 an `IN2` | `plate=` stimmt (±0,5 K) | ☐ |
| T21 | dasselbe an `IN1` | `heat=` stimmt (±0,5 K) | ☐ |
| T22 | `IN2` abziehen (offen) | innerhalb 0,3 s `FAULT code=3`; Heizung sofort aus; Fehler-LED blinkt 3-mal, Pause | ☐ |
| T23 | `IN2` kurzschließen | `FAULT code=3` | ☐ |
| T24 | `IN1` offen / Kurzschluss | `FAULT code=2` | ☐ |
| T25 | Fehler zurücksetzen: Netzteil aus, ein | Fehler ist weg, Startmeldung zeigt `LOG`-Zeile mit dem Fehler (Fehlerspeicher) | ☐ |
| T26 | während HEATING einen Fehler auslösen | Heizung geht **im selben Moment** aus (Oszilloskop/LED-Prüfer am Ausgang); Lüfter läuft danach 30 s nach und geht aus | ☐ |
| T27 | Übertemperatur: `IN1` = 30 °C-Widerstand (8,27 kΩ) | `FAULT code=4` | ☐ |
| T28 | Übertemperatur: `IN2` = 40 °C-Widerstand (5,76 kΩ) | `FAULT code=5` | ☐ |

### 4.4 Luftsensor (Audit F-09, Variante des Treibers)

| Nr. | Test | Soll | Ergebnis |
|---|---|---|---|
| T30 | `air=` mit Thermometer/Hygrometer vergleichen | Temperatur ±2 °C, Feuchte ±5 % (Toleranz des DHT11) | ☐ |
| T31 | auf den Sensor atmen | Feuchte steigt deutlich, kehrt zurück | ☐ |
| T32 | DHT11 abziehen | `air=--`, `e=` zählt hoch, nach ca. 10 s `FAULT code=1` | ☐ |
| T33 | DHT11 nach 4 Fehlversuchen wieder anstecken (vor Code 1) | `e=` sinkt wieder, kein Fehler | ☐ |
| T34 | **Vergleich der Treiber:** `AIR_DRIVER` auf `AIR_DRIVER_LIBRARY` stellen (Bibliotheken `DHT sensor library` und `Adafruit Unified Sensor` nötig; der Flash ist dann zu 96 % belegt, bei Platzproblemen `DEBUG_OUTPUT` auf 0), flashen, T30 wiederholen | gleiche Werte wie mit dem eigenen Treiber (±0,2 °C, ±1 %) | ☐ |

### 4.5 Ablauf (Zustandsautomat)

Testkonfiguration, 14,2 V, NTC-Ersatz an `IN2` = 8,27 kΩ (30 °C, Platte warm genug) und `IN1` = 12,4 kΩ.
Der Debug-Ausgang meldet jeden Zustandswechsel als `EVENT ...`.

| Nr. | Test | Soll | Ergebnis |
|---|---|---|---|
| T40 | Wetter erfüllt (feucht anhauchen, im Zimmer ggf. Schwelle weiter senken) und `rel=1` | `STANDBY->FAN_PRERUN`: Lüfter an, Heizung aus | ☐ |
| T41 | 5 s warten | `FAN_PRERUN->HEATING` | ☐ |
| T42 | `IN2` auf 15 °C (14,9 kΩ) stellen | Heizung **ein** (`H=1`), solange `plate <= sp-0,5` (sp = 19,5) | ☐ |
| T43 | `IN2` auf 20 °C (12,2 kΩ) | Heizung **aus** (`plate >= sp+0,5`) | ☐ |
| T44 | `IN2` auf 19,5 °C | Heizung behält den vorherigen Zustand (Hysterese) | ☐ |
| T45 | Wetter kurz (unter 10 s) wegnehmen | Heizung läuft weiter (`weather_lost`) | ☐ |
| T46 | Wetter länger als 10 s weg | `HEATING->AFTERRUN`, Lüfter 30 s weiter, dann `AFTERRUN->STANDBY` | ☐ |
| T47 | Spannung während HEATING auf 12,2 V | Heizung sofort aus, `AFTERRUN` | ☐ |

### 4.6 Netzteil-Selbsthaltung (Audit F-10)

| Nr. | Test | Soll | Ergebnis |
|---|---|---|---|
| T50 | Am Enable-Pin des Netzteils (`PSU1`.3) mit PA4 = HIGH messen | ca. 2,1 V oder mehr | ☐ |
| T51 | Spannung auf 12,2 V stellen, warten (Heizung und Lüfter aus) | nach 20 s `EVENT PSU_OFF`, danach **keine Ausgabe mehr** (Controller aus) | ☐ |
| T52 | Netzteil wieder auf 14,2 V | Controller startet neu, Startmeldung | ☐ |
| T53 | Ruhestrom bei ausgeschaltetem Controller und 12,2 V | notieren (Entwurf: ca. 10 µA, siehe Hardware-Analyse) | ☐ |
| T54 | Einschaltschwelle ohne Controller (Spannung langsam von 11 V steigern, Controller aus) | notieren, ab wann das Netzteil startet (Entwurf: typ. 13,3 V, Streuung 11,6 bis 15,3 V) | ☐ |
| T55 | Im Fehlerzustand bei 14,2 V: `FAULT` bleibt, **kein** Neustart | Fehlercode blinkt weiter | ☐ |

### 4.7 Watchdog und Fehlerspeicher (Audit F-04)

| Nr. | Test | Soll | Ergebnis |
|---|---|---|---|
| T60 | In `config.h` `TEST_HANG_AFTER_MS` auf `20000` setzen, flashen, 14,2 V | nach 20 s Ausgabe stoppt; nach ca. 2 s Neustart, Startmeldung mit `reset=0x.. WATCHDOG` | ☐ |
| T61 | Während des Hängens die Ausgänge messen (Oszilloskop/Multimeter) | Heizungs- und Lüfterausgang **aus** (Pulldowns) | ☐ |
| T62 | Fehlerspeicher nach dem Neustart | `LOG ... code=6 ...` (mehrere Watchdog-Resets hintereinander ergeben nur einen Eintrag) | ☐ |
| T63 | `TEST_HANG_AFTER_MS` wieder auf `0` setzen | Normalbetrieb | ☐ |
| T64 | Fehlerspeicher mit mehreren Fehlern füllen (T22, T24, T27, T28 mit Neustart dazwischen), Gerät aus und ein | Einträge in **zeitlicher Reihenfolge**, mit Zustand, Plattentemperatur und Spannung | ☐ |
| T65 | Frisch gelöschter Controller (oder anderer ATtiny826) | Fehlerspeicher wird beim ersten Start formatiert, keine `LOG`-Zeilen | ☐ |

## 5. Ergebnis eintragen

- Gesamtbewertung als Kommentar in Issue 1; Abweichungen als eigene Issues (Label `test`, `firmware`).
- Notiere Messwerte aus T03, T17, T53 und T54 (Ruhestrom, Spannungsgenauigkeit, Einschaltschwelle). Sie
  fließen in [`hardware-analyse-v3.2.md`](hardware-analyse-v3.2.md) und die Schwellen in `config.h` ein.
- Danach `config.h` auf die normalen Werte zurücksetzen (Abschnitt 3, Punkt 3).

## 6. Noch nicht Teil dieses Prüfstands

Fahrzeugtests (Spannungsverlauf bei Start-Stopp und Rekuperation, Wärmeabgabe am Emblem, Kennlinie der
Solltemperatur), die EMV und der Taupunkt-Betrieb (Variante B). Sie folgen erst nach bestandenem Prüfstand.
