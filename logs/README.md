# Logs

Aufzeichnungen der Bordspannung im Golf 7 R (30.12.2024 bis
01.01.2025), vermutlich zur Festlegung der Schwelle "Motor läuft"
(`MINBOARDVOLTAGE`, siehe [`../firmware/README.md`](../firmware/README.md)).
Aufgezeichnet mit dem Sketch [`../firmware/datenlogger/`](../firmware/datenlogger/).

## Dateien

| Datei | Inhalt |
|---|---|
| `Golf7R30.12.2024Datalog.TXT` | Messreihe vom 30.12.2024 |
| `Golf7R31.12.2024Datalog.TXT`, `..._2.TXT` | Messreihen vom 31.12.2024 |
| `Golf7R01.01.2025 Ruhespannung.TXT` | Messreihe Ruhespannung |
| `Verlaufsdiagramm*.xlsx` | Diagramme zu den Messreihen vom 30. und 31.12.2024 |

## Messaufbau (aus dem Logger-Sketch)

- Arduino Nano mit SD-Kartenmodul (Chip-Select an Pin 7).
- Bordspannung über denselben Spannungsteiler wie in der Heizung
  (R10 = 5 kΩ, R11 = 2,2 kΩ) an Analogpin A4, Referenz 5 V.
- **Ereignis-Taster** an Pin 2 und **Stopp-Taster** an Pin 3 (gegen
  Masse), drei LEDs (Pin 8 = Logging, 9 = Init, 10 = Stopp).
- Alle 300 ms ein Messwert. Mit dem Stopp-Taster endet die Aufzeichnung.

## Format

Textzeilen der Form `Time <Zeit> <Spannung in V> <Flag>`, z. B.
`Time 1192 14.19 15`.

- **Zeit:** Millisekunden seit dem Start des Loggers (`millis()`).
  Die Messreihe vom 30.12.2024 weicht ab (kleine Zähler mit mehreren
  Werten pro Zahl): Sie stammt vermutlich von einer früheren
  Logger-Version.
- **Spannung:** Werte zwischen 9,73 V und 14,38 V (Ruhespannung um
  12 V, laufender Motor um 14 V).
- **Flag:** `15` bedeutet, dass der Ereignis-Taster gedrückt wurde
  (sonst `0`). Wofür der Taster jeweils gedrückt wurde, ist nicht
  dokumentiert; laut Owner vermutlich beim Motor-Aus durch das
  Start-Stopp-System, z. B. an einer Ampel.

## Offene Punkte

- Genauer Anlass der einzelnen Ereignismarken.
- Die frühere Logger-Version der Messreihe vom 30.12.2024 liegt nicht
  vor.
- Ob die Fahrzeugdaten (Fahrt, Zeitpunkte) personenbezogene
  Rückschlüsse zulassen (vor Veröffentlichung prüfen).
