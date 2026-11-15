# Sicherheits-Audit Firmware (Ist-Stand)

Gegenstand: `firmware/acc_radar_heizung/` (Referenzstand für Platine V3.2),
Stand 2026-11-15. Reine Code- und Log-Durchsicht, **nicht** auf der Hardware
getestet. Der Zweck ist, die Schwächen vor einer Veröffentlichung und vor
jedem Fahrzeugeinsatz sichtbar zu machen; die Firmware ist in diesem Zustand
**nicht für den Einsatz freigegeben** (siehe [`../DISCLAIMER.md`](../DISCLAIMER.md)).

Schweregrad: **Hoch** = kann zu Überhitzung, Batterieentladung oder
ungewolltem Dauerbetrieb führen. **Mittel** = Fehlverhalten/Funktionslücke.
**Niedrig** = Robustheit/Qualität.

## Befunde

| ID | Grad | Befund | Fundstelle |
|---|---|---|---|
| F-01 | Hoch | **Spannungsschwelle trennt "Motor läuft" nicht von "Motor aus".** `MINBOARDVOLTAGE` = 11,7 V. Die eigenen Logs zeigen Ruhespannung ≈ 12,2 V (Median), bei laufendem Motor ≈ 14,2–14,3 V; unter 11,7 V fällt die Spannung nur beim Start. Die Heizung würde also bei eingeschalteter Zündung und stehendem Motor aus der Batterie heizen. Der Kommentar nennt 13,8 V ("Messung steht noch aus"). | `acc_radar_heizung.ino:40`, `process.ino:25`, `heating.ino:7` |
| F-02 | Hoch | **Kurzschluss eines NTC wird nicht erkannt.** Geprüft wird nur `VRT ≥ 4,7 V` (Bruch). Bei Kurzschluss nach Masse ist `VRT = 0` → Widerstand 0 → berechnete Temperatur −273 °C: Die Platte "ist immer zu kalt" (Heizung dauerhaft an), und die Übertemperaturprüfung dieses Sensors greift nie. Schutz bleibt nur über den anderen NTC; bei zwei defekten Sensoren oder falsch montierten NTC gibt es keine Grenze. | `safety.ino:17-39`, `get_inputs.ino:23-32` |
| F-03 | Hoch | **Heizung bleibt nach Fehlererkennung noch eine Weile an.** Reihenfolge in `loop()`: `standby()` vor `safety()`; im Fehlertakt schreibt `write_output()` zuerst die alten Ausgänge (Heizung noch an) und blockiert dann mit dem Blinkcode (`delay`, bis ca. 5 s). Erst im nächsten Durchlauf greift `shutdown_release`. | `acc_radar_heizung.ino:111-117`, `write_output.ino:24-37`, `standby.ino:27-38` |
| F-04 | Hoch | **Kein Watchdog.** Hängt der Controller, bleibt der Ausgangszustand stehen. Ob das Gate des MOSFET (`M2`/`MD1`) hardwareseitig per Pulldown nach Masse gezogen wird (Reset-/Hänger-Zustand), ist im Schaltplan zu prüfen. | gesamte Firmware, `hardware/V3.2` |
| F-05 | Mittel | **Lüfter-Nachlauf nach Fehler endet nie.** `heating_act <= (OVERTEMPHEATING - 40)` ist −10 °C, und im Fehlerzustand werden die Sensorwerte nicht mehr aktualisiert (`no_error`-Bedingung in `get_inputs`). Lief der Lüfter beim Fehler, läuft er bis zum Spannungsentzug. | `standby.ino:32-36`, `get_inputs.ino:8,40` |
| F-06 | Mittel | **Keine Überspannungs-/Plausibilitätsprüfung der Bordspannung.** Der Spannungsteiler (5 kΩ / 2,2 kΩ) sättigt den ADC ab ca. 16,4 V; Überspannung wird nicht behandelt. | `get_inputs.ino:12-14` |
| F-07 | Mittel | **Keine Funktionsüberwachung.** Weder Lüfter (kein Rückmelder) noch Heizelement (Strommessung ACS711 ungenutzt, kein Temperaturanstieg-Test) noch eine Maximallaufzeit werden geprüft. | gesamte Firmware |
| F-08 | Mittel | **Zeitverhalten weicht von der Absicht ab:** der Standby-Timer läuft frei (Abschaltverzögerung real 0–10 s statt 10 s; Lüfter aus nochmals 0–10 s); der Lüfter startet sofort statt nach 5 s Verzögerung; Temperatur-(8 °C) und Feuchteschwelle (30 %) haben keine Hysterese, bei Messwerten um die Schwelle pendelt der Zustand. | `standby.ino:8-22`, `fan.ino`, `process.ino:21-22` |
| F-09 | Mittel | **DHT11-Ausfall:** Fehlercode 1 nach 5 Fehlversuchen (≈ 10 s) ohne Wiederanlauf; ein kurzer Steckerwackler schaltet dauerhaft ab (nur Neustart). Gelatchte Fehler sind für eine Heizung vertretbar, sollten aber bewusst und dokumentiert sein. Außerdem werden "stehende" gültige Werte nicht erkannt. | `get_inputs.ino:40-57`, `safety.ino:11` |
| F-10 | Mittel | **`PSUENABLE` (PA4) und `SHUTDOWNDELAY` sind toter Code.** `psuenable` wird nie auf `true` gesetzt, der Pin ist immer LOW; `SHUTDOWNDELAY` wird nicht benutzt. Unklar, was PA4 auf der Platine tut (Selbsthaltung? Abschaltung?). | `write_output.ino:20-22`, `acc_radar_heizung.ino:34,62` |
| F-11 | Niedrig | **Testwerte als Schwellen:** Feuchte 30 % (Kommentar: "später 80 %"); Platten-Abschaltung bei 40 °C (`MAXTEMPPLATE + 20`) bei Regelung auf 19–20 °C; Heizelement-Grenze 30 °C. Die Auslegung (was misst der "NTC Heizelement" tatsächlich?) ist nicht dokumentiert. | `acc_radar_heizung.ino:28-40`, `safety.ino:29-37` |
| F-12 | Niedrig | **Referenzspannung fest eingetragen** (`VCC 4.9`), tatsächliche Versorgung kann abweichen; ADC-Referenz ist VDD. Systematischer Messfehler im %-Bereich. | `acc_radar_heizung.ino:14` |
| F-13 | Niedrig | **Zeitvergleiche nicht überlaufsicher** (`t + Δ <= millis()`); Überlauf nach 49,7 Tagen Dauerbetrieb (im Fahrzeug unwahrscheinlich). | alle Timer |
| F-14 | Niedrig | **Code-Qualität:** Makros mit Ein-Buchstaben-Namen (`R`, `B`) und fragile Include-Reihenfolge; viele globale Variablen; `float` als Zähler; Namen `readtimedelay0..5`; Schwellen als Magic Numbers im Code; Mischung aus Deutsch/Englisch; Zustandslogik über viele Boolean-Flags statt Zustandsautomat. | gesamte Firmware |

## Umsetzungsstand

| ID | Stand |
|---|---|
| F-01 | **Im Code umgesetzt** (nicht am Fahrzeug getestet): neue Spannungsfreigabe mit Hysterese und Entprellung (`firmware/acc_radar_heizung/voltage.ino`). Prüfung per Nachrechnung an den Logs: bei der Ruhespannungs-Messreihe wird erst freigegeben, als die Spannung nach ca. 10,6 min steigt (0 Messpunkte freigegeben unter 12,6 V); in den Fahrtreihen erfolgt die Freigabe nach 5–7 s. Die Freigabe wird beim Start-Stopp bis zu 3 s verzögert entzogen. Schwellen (13,5 / 13,0 V) noch am Fahrzeug zu bestätigen, auch ob die Spannung bei Ladestrategie des Fahrzeugs (Rekuperation) zeitweise unter 13,0 V fällt. |
| F-06 | **Teilweise umgesetzt:** Überspannung über 15,5 V entzieht sofort die Freigabe (Wiederfreigabe erst unter 15,0 V). Unterspannung ist durch die Freigabe mit abgedeckt. Dauer-Überspannung über ca. 16,4 V ist wegen ADC-Sättigung nicht messbar (Hardware-Schutz prüfen). |
| F-02 | **Im Code umgesetzt** (nicht am Gerät getestet): NTC-Spannung ≤ 0,15 V gilt als Kurzschluss (Fehlercode 2 bzw. 3); erst nach der ersten Messung aktiv, damit der Start nicht fälschlich auslöst. |
| F-04 | **Im Code umgesetzt** (kompiliert, nicht am Gerät getestet): Watchdog ca. 2 s (`WDT_PERIOD_2KCLK_gc`), Zurücksetzen nur am Ende von `loop()`. Hardware ([Analyse](hardware-analyse-v3.2.md)): Pulldowns `RN2` an PC2/PC3 sind vorhanden, Heizung und Lüfter sind bei Reset aus. `wdt_enable(WDT_PERIOD_2KCLK_gc)` übersetzt mit megaTinyCore 2.6.11. |
| F-03 | **Im Code umgesetzt** (kompiliert, nicht am Gerät getestet): `safety()` läuft direkt nach `get_inputs()` und schaltet den Heizungspin sofort ab; Fehler-LED blinkt zeitgesteuert ohne `delay` (gleiches Muster wie vorher, per Simulation geprüft). |
| F-05 | **Im Code umgesetzt** (nicht getestet): zeitgesteuerter Lüfter-Nachlauf von 30 s nach einem Fehler (Wert vorläufig). |
| F-10 | **Im Code umgesetzt** (nicht getestet): PA4 hält das Netzteil (Selbsthaltung) und der Controller schaltet sich nur bei Motor aus nach 20 s selbst ab (`psu.ino`). Mit den Datenblattwerten ist belegt, dass PA4 = LOW das Netzteil bei normaler Bordspannung abschaltet; der vorherige Sketch konnte so nicht stabil laufen. Siehe [Analyse](hardware-analyse-v3.2.md). Hardware-Toleranz der Enable-Schwelle (Einschalten 11,6–15,3 V) bleibt ein Platinenthema. |
| F-08 | **Im Code umgesetzt** (gebaut, nicht am Gerät getestet): Zustandsautomat; Abschaltverzögerung zeitgenau (10 s), Lüfter-Vorlauf gezählt ab Bedingung, Hysterese an Temperatur- und Feuchteschwelle. |
| F-09 | teilweise: Fehlerzähler des Luftsensors klingt bei gültigen Messwerten ab (vorher hing er bei 4); Erkennung stehender Werte offen. |
| F-11 | teilweise: Feuchteschwelle jetzt 80 % (war Testwert 30 %); Übertemperaturgrenzen (30/40 °C) weiter zu prüfen. |
| F-13 | **Im Code umgesetzt**: alle Zeitvergleiche rechnen mit Differenzen. |
| F-14 | **teilweise umgesetzt**: Zustandsautomat, `config.h`, Tabs, Ein-Buchstaben-Makros entfernt; Fließkomma und `log()` sind entfernt (Tabelle, Ganzzahlen). |
| F-12 | **für die NTC erledigt:** Rohwert-Tabelle, die Versorgungsspannung kürzt sich heraus. Offen bleibt die feste Referenz `ADC_VREF_MV` = 4900 für die Bordspannung. |
| F-07 | offen |

Hinweis: Die Änderungen **bauen** (megaTinyCore 2.6.11, ATtiny826, keine Fehler und
keine Warnungen im Sketch; Stand 2026-11-15), sind aber nicht am Gerät getestet. Vor jedem Flashen bauen und
auf dem Prüfstand testen.

## Was gut ist (beibehalten)

- Sichere Startwerte: Heizung nur nach gültigem DHT-Wert (`dht_t` startet bei
  100 °C, `dht_h` bei 0 %) und gültiger Spannungsmessung (`boardvoltage` = 0).
- Lüfter läuft vor und nach der Heizung; Heizung erst nach Lüfter-Freigabe.
- Zwei unabhängige NTC (Heizelement, Platte) und Übertemperaturabschaltung.
- Regelung mit Hysterese (19–20 °C) und Sensorbruch-Erkennung (`VRT ≥ 4,7 V`).
- Fehlercodes per Blinkcode.

## Hardware-Punkte zur Klärung (für `hardware-dev`)

- Gate-Pulldown an `M2`/Eingang `MD1` (definierter Zustand bei Reset/Hänger).
- Sicherung/Strombegrenzung im Heizkreis; Verpolschutz und Schutz vor
  Spannungsspitzen am Eingang (Bordnetz).
- Funktion von PA4 (`PSUENABLE`) auf der Platine.
- Einsatzort der NTC ("Heizelement" und "Platte"), Montage und Zuordnung zu
  `IN1`/`IN2`.
- Woher kommt die Versorgung (Dauerplus oder geschaltetes Plus/Zündung)?
  Das entscheidet über Ruhestrom und über die Frage, wie "Motor läuft"
  erkannt werden muss (F-01).

## Empfohlene Umsetzung (Vorschlag, wird im Repo als einzelne Commits gemacht)

1. **Spannungslogik (F-01, F-06):** Ein-/Ausschaltschwelle mit Hysterese und
   Entprellung auf Basis der Logs (Ruhe ≈ 12,2 V, Fahrt ≈ 14,2 V →
   z. B. ein ab ≈ 13,5 V, aus unter ≈ 13,0 V; Wert ist zu bestätigen),
   getrennt davon eine Unterspannungsabschaltung; Überspannungsabschaltung.
2. **Sensorik (F-02, F-12):** Plausibilitätsprüfung beider Seiten
   (Kurzschluss und Bruch), bei beiden NTC gültiger Bereich; zentrale
   Konstanten.
3. **Sicherheitsstruktur (F-03, F-04, F-05):** Hardware-Watchdog; Sicherheit
   zuerst in der Schleife und sofortiges Abschalten; Fehleranzeige ohne
   blockierendes `delay`; definierter Fehlerzustand (Heizung aus, Lüfter-
   Nachlauf mit Ende).
4. **Ablauf als Zustandsautomat (F-08):** Zustände wie `STANDBY`,
   `VORLAUF_LUEFTER`, `HEIZEN`, `NACHLAUF`, `FEHLER` mit klaren Zeiten,
   Hysterese an den Wetter-Schwellen, überlaufsichere Timer.
5. **Aufräumen (F-10, F-11, F-14):** toten Code entfernen oder klären,
   Konstanten in eine Konfiguration, einheitliche Namen, Kommentare.
6. **Funktionsüberwachung (F-07):** erst nach Klärung der Hardware (Strom,
   Temperaturanstieg).

Jede dieser Änderungen ist vor dem Einsatz am Gerät zu prüfen (Bench-Test
mit Lastwiderstand/Netzteil, danach Fahrzeug).
