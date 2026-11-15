# Firmware

Arduino-Sketch für den **ATtiny826** (megaTinyCore), Referenzstand für
die Platine V3.2: [`acc_radar_heizung/`](acc_radar_heizung/).

## Bauen und flashen

- Arduino IDE mit **megaTinyCore**, Board ATtiny826, Programmierung über
  **UPDI**. Laut Owner läuft das UPDI-Programmieren mit einem Arduino
  Uno als Programmer (Pin 6, siehe auch die ReadMe aus V3.2, hier
  übernommen in diese Datei).
- Bibliotheken: **keine nötig** (Standard). Nur wenn in `config.h`
  `AIR_DRIVER_LIBRARY` gewählt wird (Vergleichsvariante), braucht es
  `DHT sensor library` und `Adafruit Unified Sensor`.
- **Kommandozeile** (mit dem `arduino-cli`, der in der Arduino IDE 2 liegt;
  megaTinyCore und beide Bibliotheken müssen installiert sein):

  ```
  arduino-cli compile -b megaTinyCore:megaavr:atxy6:chip=826     --build-path <Build-Ordner> firmware/acc_radar_heizung
  ```

  Stand 2026-11-15 (megaTinyCore 2.6.11), keine Fehler und keine Warnungen im
  Sketch (`--warnings all`):

  | Variante | Flash | RAM |
  |---|---|---|
  | Eigener DHT11-Treiber, ohne Debug-Ausgang (`DEBUG_OUTPUT = 0`) | 3810 von 8192 Byte (46 %) | 78 von 1024 Byte (8 %) |
  | Eigener DHT11-Treiber mit Debug-Ausgang (Standard) | 5388 Byte (65 %) | 246 Byte (24 %) |
  | Adafruit-DHT-Bibliothek (`AIR_DRIVER_LIBRARY`) mit Debug-Ausgang | 7940 Byte (96 %) | |
  | Stand vor dem Umbau (frühere Fassung des Sketches) | 6412 Byte (78 %) | 227 Byte (22 %) |

  Der Build-Ordner gehört nicht ins Repo.
- Achtung Pinnamen: In der IDE heißen die Pins `PIN_PA6` usw. Auf der
  Platine ist `PA6` die Error-LED.
- `#define DEBUG` aktiviert die serielle Ausgabe (9600 Baud).

## Pinbelegung (laut Firmware)

| Pin | Funktion |
|---|---|
| PB3 | DHT11 (Daten) |
| PA7 | NTC Heizelement (`NTCHEATING`, Eingang 1) |
| PB4 | NTC Logo-Platte (`NTCPLATE`, Eingang 2) |
| PA5 | Spannungsteiler Bordspannung (`GENERATOR`) |
| PC2 | Heizelement (Ausgang) |
| PC3 | Lüfter (Ausgang) |
| PA4 | Selbsthaltung/Abschaltung des Netzteils (`PSUENABLE`, HIGH = halten) |
| PA6 | Fehler-LED |

Die Zuordnung zu den Platinen-Steckverbindern ist noch nicht gegen das
Layout geprüft (siehe [`../hardware/README.md`](../hardware/README.md)).

## Aufbau (Arduino-Tabs)

| Datei | Aufgabe |
|---|---|
| `acc_radar_heizung.ino` | Globale Zustände, `setup()`, `loop()` |
| `config.h` | Alle Pins, Zeiten und Grenzwerte (Einheit im Namen) |
| `types.h` | Zustände (`State`) und Messwert-Struktur (`Sensors`) |
| `ntc_table.h` | NTC-Tabelle, **erzeugt** mit `../tools/gen_ntc_table.py` |
| `sensors.ino` | Bordspannung und NTC einlesen |
| `air_dht11.ino` | Luftsensor-Schicht (nur diese Datei kennt den DHT11): eigener Treiber oder Bibliothek |
| `voltage.ino` | Spannungsfreigabe "Motor läuft" |
| `faults.ino` | Fehlererkennung, sofortiges Abschalten |
| `statemachine.ino` | Zustandsautomat, Wetterbedingung, Regelung |
| `psu.ino` | Netzteil halten / selbst abschalten (PA4) |
| `outputs.ino` | Ausgänge und Fehler-LED |
| `faultlog.ino` | EEPROM-Fehlerspeicher (Ringspeicher der letzten 24 Fehler) |
| `debug.ino` | Debug-Ausgang: Textausgabe über USART0 TX (PB2) |

Die Verhaltensbeschreibung steht in [`../docs/verhalten.md`](../docs/verhalten.md).

## Fehlerspeicher und Debug-Ausgang

**EEPROM-Fehlerspeicher** (`faultlog.ino`): Ringspeicher mit 24 Einträgen zu je 4 Byte (Fehlercode,
Zustand, Plattentemperatur, Bordspannung), 100 von 128 Byte. Er bleibt nach einem Neustart erhalten.
Geschrieben wird nur bei einem neuen Fehler und nach einem Watchdog-Reset (Code 6, aufeinanderfolgende
Einträge werden zusammengefasst), nicht bei jedem Start; das schont die Schreibzyklen (ca. 100000 je
Zelle). Das Schreiben erfolgt erst nach dem Ansteuern der Ausgänge, höchstens ein Byte je
Schleifendurchlauf. Ein neuer Controller formatiert den Speicher beim ersten Start.

**Debug-Ausgang** (`debug.ino`): serielle Textausgabe, **115200 Baud, 8N1, nur senden**, am Pin
**PB2** (USART0 TX). Auf der Platine V3.2 ist PB2 nur am Controller-Pad (Pin 12 des QFN-20) erreichbar,
für V4 ist ein eigenes Pad vorgesehen. Der Anschluss braucht einen USB-Seriell-Adapter (TTL) und Masse.
Die Ausgabe geht in einen Ringpuffer und wird ohne Blockieren gesendet; bei vollem Puffer werden Zeichen
verworfen, die Regelung wird nie angehalten. Mit `DEBUG_OUTPUT = 0` in `config.h` entfällt der Code.

- Startmeldung mit Version, Reset-Ursache (`POWERON`, `WATCHDOG`, `BROWNOUT`) und dem Fehlerspeicher
  (`LOG slot=.. code=.. state=.. plate=.. v=..`, ältester Eintrag zuerst)
- `EVENT <alt>-><neu>` bei jedem Zustandswechsel, `FAULT code=..` bei einem Fehler, `EVENT PSU_OFF`
- einmal pro Sekunde eine Statuszeile: `t=<ms> s=<Zustand> f=<Fehler> v=<mV> rel=<Freigabe>
  air=<Grad>/<%rF> e=<Sensorfehler> plate=<Grad> heat=<Grad> sp=<Soll> H=<Heizung> F=<Lüfter> psu=<PA4>`

Für den Test steht [`../docs/pruefstand.md`](../docs/pruefstand.md) bereit. Dort ist auch der
Watchdog-Test beschrieben (`TEST_HANG_AFTER_MS` in `config.h`).

## Rechnen ohne Fließkomma

Die Firmware rechnet in Ganzzahlen (Temperatur in 0,1 °C, Luftfeuchte in
0,1 %, Spannung in mV). Das spart die Fließkomma-Bibliothek des Compilers.

- **NTC:** Die Temperatur kommt aus einer Tabelle über dem 10-Bit-ADC-Rohwert
  (`ntc_table.h`, 33 Stützstellen von −40 bis 120 °C, lineare Interpolation,
  größter Fehler 0,19 K). Pull-up und ADC-Referenz liegen beide an VDD, die
  Versorgungsspannung kürzt sich heraus. **Anderer NTC oder anderes
  Teilerverhältnis:** Parameter an `tools/gen_ntc_table.py` übergeben
  (`--r0`, `--b`, `--rseries`), Skript ausführen, die erzeugte `ntc_table.h`
  ersetzen; am Code ändert sich nichts. Die Grenzen für Unterbrechung und
  Kurzschluss (`NTC_RAW_OPEN`, `NTC_RAW_SHORT`) stehen in `config.h`.
- **Eigener DHT11-Treiber** (`air_dht11.ino`): direkter Portzugriff auf PB3
  (`AIR_VPORT`/`AIR_BM` in `config.h`). Ein Bit zählt als 1, wenn der High-Impuls
  länger als der Low-Impuls davor ist; das ist unabhängig vom Takt. Prüfsumme und
  Messbereich werden geprüft. Der Treiber ist mit der Bibliothek **nicht** auf
  einem echten Sensor verglichen; dafür lässt sich in `config.h` auf
  `AIR_DRIVER_LIBRARY` umschalten.

## Ablauf

Hauptschleife: `sensors_update()` → `faults_check()` → `statemachine_update()`
→ `psu_update()` → `outputs_apply()`. `faults_check()` läuft direkt nach dem
Einlesen und schaltet die Heizung im Fehlerfall sofort am Pin ab. Ein
Hardware-Watchdog (ca. 2 s) wird nur am Ende der Schleife zurückgesetzt.

Zustände: `START` → `STANDBY` → `FAN_PRERUN` → `HEATING` → `AFTERRUN` →
`STANDBY`, aus jedem Zustand `FAULT`.

1. **Einschaltbedingung** (`STANDBY` → `FAN_PRERUN`): **Spannungsfreigabe**
   (`voltage.ino`) und **Wetterbedingung** (Variante A: Lufttemperatur ≤ 8 °C
   und Luftfeuchte ≥ 80 %, aus bei ≥ 9 °C oder ≤ 75 %). Die Freigabe: Bordspannung
   5 s lang zwischen 13,5 V und 15,0 V ("Motor läuft"; Ruhe ca. 12,2 V, Fahrt
   ca. 14,2 V laut Logs), Entzug nach 3 s unter 13,0 V und sofort über
   15,5 V; nach dem Start gesperrt. Werte noch am Fahrzeug zu bestätigen.
2. **Lüfter-Vorlauf** 5 s (`FAN_PRERUN_MS`), dann `HEATING`.
3. **Regelung** (`HEATING`): Zweipunktregelung der Logo-Platte um die
   Solltemperatur ± 0,5 K. Die Solltemperatur kommt aus einer Kennlinie über
   der Lufttemperatur (`SETPOINT_*` in `config.h`, zunächst konstant 19,5 °C).
4. **Ende** (`HEATING` → `AFTERRUN`): sofort bei fehlender Spannungsfreigabe,
   sonst 10 s nach Wegfall der Wetterbedingung. Lüfter-Nachlauf 30 s, dann
   `STANDBY`.
5. **Netzteil:** PA4 wird beim Start sofort auf HIGH gelegt
   (Selbsthaltung). Besteht 20 s lang keine Spannungsfreigabe, und sind
   Lüfter und Heizung aus, schaltet sich der Controller über PA4 = LOW selbst ab
   (`psu.ino`). Im Fehlerfall bleibt er bei laufendem Motor an. Für
   Prüfstandtests mit Labornetzteil mindestens 13,5 V einstellen. Außerdem
   braucht die Wetterbedingung 80 % Luftfeuchte; im Zimmer startet die Heizung
   deshalb nicht (zum Test `WX_RH_ON_PCT` in `config.h` senken).

## Sicherheit und Fehlercodes

Bei einem Fehler wird die Heizung sofort abgeschaltet, ein laufender Lüfter
läuft 30 s nach (`FAN_AFTERRUN_DELAY`), die Fehler-LED blinkt den Code
(Anzahl Blinks = Code, ohne die Schleife zu blockieren). Der Fehler wird
erst durch einen Neustart gelöscht:

| Code | Ursache |
|---|---|
| 1 | DHT11 nicht lesbar (Fehlerzähler ≥ 5) |
| 2 | NTC Heizelement defekt (Spannung ≥ 4,7 V = Unterbrechung oder ≤ 0,15 V = Kurzschluss) |
| 3 | NTC Logo-Platte defekt (gleiche Grenzen) |
| 4 | Übertemperatur Heizelement (≥ `OVERTEMPHEATING`, 30 °C) |
| 5 | Übertemperatur Logo-Platte (≥ `MAXTEMPPLATE` + 20 °C) |

## Bekannte Schwächen / Review-Liste

Das ist der dokumentierte Ist-Stand, **nicht** freigegeben für den
Fahrzeugeinsatz. Die vollständige Liste mit Schweregraden steht im
[Sicherheits-Audit](../docs/sicherheits-audit-firmware.md); hier eine
Kurzfassung der noch offenen Punkte (umgesetzt: F-01 bis F-05, F-10, jeweils
kompiliert, aber nicht am Gerät getestet):

- Schwellwerte und Zeiten sind Vorschläge und am Fahrzeug zu bestätigen.
- Die Fehler-Abschaltung der Platte greift erst bei `MAXTEMPPLATE + 20`
  (40 °C), die Regelung schaltet bei 20 °C ab.
- Der Fehlerzustand hat keinen Reset außer Neustart.
- Der Strom (ACS711) wird nicht ausgelesen (Messung laut Owner wenig
  brauchbar, auf der Platine nur vorgehalten).
- Der DHT11-Fehlerzähler und die Schwellen sind nicht dokumentiert
  kalibriert.
- NTC im Code: 10 kΩ, B = 3435 (passt zum verbauten 10-kΩ-NTC);
  Versorgung als `VCC 4.9` fest eingetragen.

## Datenlogger

[`datenlogger/`](datenlogger/) ist der Sketch, mit dem die Messreihen in
[`../logs/`](../logs/) aufgezeichnet wurden (Arduino Nano mit SD-Modul,
siehe dort).

## Archiv

[`archiv/`](archiv/) enthält frühere Entwicklungsstände (V1-Entwurf,
V2, V3.1 vor der Aufteilung in Tabs). `v2_variante_beitrag` enthält laut
Projektinhaber eine Variante mit Ideen eines Beitragenden (kein Code von ihm). Die leeren
`arduino_secrets.h` aus den Originalordnern wurden bewusst nicht
übernommen; der `#include` darauf wurde entfernt.
