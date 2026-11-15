# Hardware-Analyse V3.2 (aus dem Schaltplan abgeleitet)

Grundlage: der EasyEDA-Schaltplan `hardware/V3.2/V3.2_Schaltplan.easyeda.json`
(zwei Blätter: "PSU/CM" und "I/O"; gilt laut Owner für V3.1, V3.2 und V3.3).
Die Netzliste wurde automatisch aus Leitungen, Anschlusspunkten und
Netznamen erzeugt, **nicht** am Layout oder an der gebauten Platine
geprüft. Alles hier ist vom Owner zu bestätigen. Die Zahlen zum Enable-Pin
des LMR14020 (Abschnitt 3) und zur Versorgung des ACS711 (Abschnitt 5) wurden
gegen die Datenblätter im Ordner V3.1/DataSheets des Altprojekts geprüft.

## 1. Signalwege (bestätigen die Pinbelegung der Firmware)

| MCU-Pin | Netz | Weg auf der Platine | Funktion |
|---|---|---|---|
| PC2 | PC2 | → Treiber `MD1`.INB → `MD1`.OUTB → Gate `M2` (MS15N10) | **Heizelement** (Low-Side-Schalter) |
| PC3 | PC3 | → `MD1`.INA → `MD1`.OUTA → Gate `M1` (SOT-23) | **Lüfter** (Low-Side-Schalter), Anschluss `OUT1` |
| PA7 | PA7 | `IN1`.2 (NTC nach Masse) mit Pull-up `R8` 10 kΩ nach +5 V | NTC Heizelement |
| PB4 | PB4 | `IN2`.1 (NTC nach Masse) mit Pull-up `R9` 10 kΩ nach +5 V | NTC Logo-Platte |
| PB3 | PB3 | `IN3`.2 mit Pull-up `R7` 4,7 kΩ nach +5 V (`IN3`: 1 = +5 V, 4 = Masse) | DHT11 |
| PA5 | PA5 | Teiler `R10` 5 kΩ (von +14 V) / `R11` 2,2 kΩ (nach Masse) | Bordspannung |
| PA6 | PA6 | über `R14` 620 Ω zur Fehler-LED | Fehler-LED |
| PA4 | PA4 | über `R12` 56 kΩ an den Enable-Pin (`EN`) des Netzteils `PSU1` | **Selbsthaltung des Netzteils** (siehe 3) |
| PA2 | PA2 | Ausgang `VIOUT` des Stromsensors `CM2` (ACS711) | Strom (ungenutzt) |
| PA3 | PA3 | `#FAULT` des Stromsensors `CM2` | Strom-Fehler (ungenutzt) |
| PA0 | UPDI | über `R13` 4,7 kΩ zum Programmierheader `PROG` | Programmierung |

Alle Zuordnungen der Firmware (`PIN_PA7`, `PIN_PB4`, `PIN_PB3`, `PIN_PA5`,
`PIN_PC2`, `PIN_PC3`, `PIN_PA6`, `PIN_PA4`) stimmen mit dem Schaltplan überein.
Damit ist auch `M1` geklärt: er schaltet den Lüfter. `OUT1` = Lüfter
(+14 V an Pin 1, Drain von `M1` an Pin 2). Das Heizelement hängt zwischen den
Pads `2+` (+14 V) und `2-` (Drain von `M2`).

## 2. Leistungsteil

- Der gesamte Strom läuft von Pad `IN+` durch den **Strommessleiter des
  ACS711 (`CM2`, IP+ → IP-)** auf die Schiene `+14V`. Der Sensor liegt also
  im Hauptstrompfad (Netzteil, Lüfter und Heizung), nicht nur im Heizkreis.
- **Pulldowns:** `RN2` (20-kΩ-Array) zieht die Treibereingänge **PC2 und PC3
  nach Masse**. Ist der Controller im Reset oder die Pins sind hochohmig,
  bleiben Heizung und Lüfter damit aus. (Die Prüfung der Netzliste hatte
  zunächst einen Pin übersehen, die Leitungen am Array wurden danach
  einzeln nachgesehen: Pins 1–4 sind verbunden und liegen auf Masse.)
- Ein Pulldown direkt an den MOSFET-Gates gibt es nicht; der Treiber `MD1`
  (IX4427, Versorgung `VCCMD` = +14 V) übernimmt die Gate-Ansteuerung.
  Bleibt der Controller **hängen und hält PC2 auf HIGH**, bleibt die Heizung
  an (Watchdog siehe Audit F-04).
- Im Schaltplan sind **keine Sicherung, kein Überspannungsschutz (TVS) und
  kein Verpolschutz** zu finden (nur `D1` als Freilaufdiode im Netzteil).
  Falls solche Schutzelemente extern (am Kabelbaum) liegen, sollte das
  dokumentiert werden.

## 3. Netzteil-Enable und PA4 (Selbsthaltung)

Beschaltung am Enable-Pin (`EN`) des LMR14020: `R1` 220 kΩ von +14 V, `R2`
21,5 kΩ nach Masse, dazu `R12` 56 kΩ vom Controller-Pin PA4.

Werte aus dem LMR14020-Datenblatt (TI, im Repo-Original unter V3.1/DataSheets
gelesen): Enable-Schwelle `V_EN_TH` = **1,05 / 1,20 / 1,38 V** (min / typ /
max). Der EN-Pin hat eine interne Stromquelle von **1,0 µA** unterhalb und
**4,6 µA** oberhalb der Schwelle (3,6 µA Hysteresestrom). Damit ergeben sich
aus der Widerstandsschaltung folgende Bordspannungen:

| Fall (Schwelle) | Netzteil **ein**, PA4 offen | **aus**, PA4 offen | Halten mit **PA4 = LOW** erst ab | Halten mit **PA4 = HIGH** |
|---|---|---|---|---|
| min (1,05 V) | 11,6 V | 10,8 V | 14,9 V | immer |
| typ (1,20 V) | 13,3 V | 12,5 V | 17,2 V | immer |
| max (1,38 V) | 15,3 V | 14,5 V | 19,9 V | ab 0,3 V |

Was daraus folgt:

- **PA4 = HIGH hält das Netzteil sicher an** (Selbsthaltung), auch wenn die
  Bordspannung sinkt. **PA4 = LOW schaltet es bei jeder normalen Bordspannung
  (unter ca. 15 V) ab**: Der Controller schaltet sich damit selbst ab.
- **Ohne Controller** (PA4 offen) arbeitet der Teiler als grobe
  "Motor läuft"-Erkennung: Einschalten typisch bei **13,3 V**. Wegen der
  Toleranz der Enable-Schwelle liegt dieser Punkt aber zwischen **11,6 V und
  15,3 V**: Im ungünstigen Fall startet das Gerät schon bei Ruhespannung
  (12,2 V) und fährt sich nach dem Abschalten wiederholt neu hoch, im anderen
  Fall startet es auch bei laufendem Motor (14,4 V) nie. Für V4 wäre ein
  genauerer Komparator oder eine engere Auslegung sinnvoll.
- Die frühen Sketche (V2, V3.1 vor dem Aufteilen in Tabs) halten PA4 beim
  Start auf HIGH (`psuenable = true`, `digitalWrite(PSUENABLE, HIGH)` in
  `setup()`) und schalten nach `SHUTDOWNDELAY` über PA4 = LOW ab. Das passt
  zur Hardware.

## 4. Befund: Regression im Referenz-Sketch (PA4), inzwischen behoben

Der übernommene Referenz-Sketch hatte `psuenable = false` am Start, das
`digitalWrite(PSUENABLE, ...)` in `setup()` war auskommentiert, und
`write_output()` schrieb PA4 bei `psuenable == false` alle 100 ms auf LOW.
`psuenable` wurde nirgends auf `true` gesetzt. Nach der Tabelle oben kann das
Netzteil mit PA4 = LOW bei normaler Bordspannung nicht an bleiben. Der Sketch
in dieser Form hätte sich nach dem Start also immer wieder neu gestartet; er
kann so nicht stabil gelaufen sein.

Behebung im Code (siehe `firmware/acc_radar_heizung/psu.ino`): PA4 wird als
Erstes in `setup()` auf HIGH gelegt; der Controller schaltet sich nur ab, wenn
20 s lang keine Spannungsfreigabe besteht (Motor aus). Im Fehlerfall bleibt er
an, solange der Motor läuft, damit der Fehlercode angezeigt wird und sich das
Gerät nicht ungewollt neu startet. Beim Prüfstandtest mit Labornetzteil
deshalb mindestens 13,5 V einstellen (sonst schaltet sich der Controller nach
20 s ab).

## 5. Stromsensor ACS711 (`CM2`): Versorgung

Der VCC-Pin des Sensors hängt an `R15` (1 kΩ) nach +5 V und `R16` (2 kΩ) nach
Masse, also an einem **Spannungsteiler** (leerlaufend 3,33 V, Innenwiderstand
ca. 667 Ω). Laut Datenblatt (ACS711, Allegro) braucht der Sensor eine
Versorgung von **3,0 bis 5,5 V** und nimmt **typ. 4 mA (max. 5,5 mA)** auf.
Bei 4 mA Last bricht die Spannung an diesem Teiler auf deutlich unter 3,0 V
ein (grobe Abschätzung mit dem Sensor als Widerstand: ca. 1,8 V). Der Sensor
liegt damit **unter seiner Mindestversorgung**; da sein Ausgang außerdem
ratiometrisch zu VCC arbeitet, ist die Messung unbrauchbar. Das erklärt, warum
die Strommessung "nicht prickelnd" war. Behebung auf der Platine: Sensor
direkt aus +5 V oder einer 3,3-V-Schiene versorgen (der Ausgangsbereich
passt zum ADC mit 5 V nur eingeschränkt, bei 3,3 V gilt die Empfindlichkeit
aus der Tabelle des Datenblatts).

## 6. Antworten auf die offenen Hardware-Fragen (vom Owner zu bestätigen)

1. **Versorgung:** Dauerplus von der Batterie (Netz `+14VBatt`), das
   Netzteil wird über den Enable-Teiler spannungsabhängig aktiv (siehe 3).
2. **PA4:** Selbsthaltung/Abschaltung des Netzteils (siehe 3, 4).
3. **Gate-Pulldown:** Pulldowns an den Treibereingängen (`RN2`, PC2/PC3), kein
   direkter Gate-Pulldown; definierter Aus-Zustand bei Reset ist gegeben,
   bei einem hängenden Controller mit PC2 = HIGH nicht.
4. **NTC-Montage** ("Heizelement" und "Platte"): geht aus dem Schaltplan nicht
   hervor, nur dass `IN1` = Heizelement und `IN2` = Platte.

## 7. Weitere Auffälligkeiten

- `LED2`/`LED3` (Lüfter-/Heizungs-Anzeige) sind mit Anode am MOSFET-Drain
  gezeichnet; das Symbol benennt die Pins A/C offenbar anders als die
  Footprint-Pads. Polarität auf der Platine prüfen.
- Freie Controller-Pins: PA1, PB0, PB1, PB2, PB5, PC0, PC1. PA2/PA3 sind für
  den Stromsensor reserviert.
