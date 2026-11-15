# Hardware

Schaltpläne und Layouts im EasyEDA-Format (JSON-Export, in EasyEDA über
*Datei → Öffnen → EasyEDA…* ladbar), Gerber-Dateien zur Fertigung und
die Stückliste. Fertigung und Bestückung bisher über JLCPCB/LCSC.

## Versionsübersicht

| Version | Ordner | Status | Beschreibung |
|---|---|---|---|
| **V3.2** | [`V3.2/`](V3.2/) | **gebaut und getestet, aktueller Stand** | Eine 4-Lagen-Platine mit PSU und Controlmodul. Referenz für die Firmware. |
| V3.3 | [`V3.3/`](V3.3/) | nur Layout-Test, nicht gefertigt | Gleicher Schaltplan wie V3.2, platzsparender angeordnet. |
| V3.1 | [`V3.1/`](V3.1/) | gebaut und getestet (Versuch) | Zwei getrennte Platinen (PSU und CM). Ein erster Versuch. Der kombinierte Schaltplan gehört zu V3.1, V3.2 und V3.3. |
| V4 | [`V4/`](V4/) | nur Schaltplan-Entwurf | Noch kein Layout. |
| V2 | [`archiv/V2/`](archiv/V2/) | nur Fertigungsdaten, laut Owner nicht gebaut/getestet | Einzelplatine, ATtiny3224. |
| V1 und frühe V3 | [`archiv/`](archiv/) | Archiv | Erste Entwürfe. |
| ATtiny826-Evaluation | [`Evaluation-ATtiny826/`](Evaluation-ATtiny826/) | Testplatinen | Gehäuseformen (u. a. VQFN) für den Mikrocontroller. |

Hinweis: Der Schaltplan liegt als `V3.2_Schaltplan.easyeda.json` unter
V3.2. Er ist derselbe wie der V3.1-Schaltplan (kombiniertes Blatt
PSU/CM), das Layout unterscheidet sich.

## Aufbau der Schaltung (V3.2)

- **Netzteil (PSU):** Stepdown **LMR14020** (TI) mit 5,6-µH-Drossel und
  Diode SS34 erzeugt aus der Bordspannung die 5 V des Controlmoduls
  (Netz `+5VCM`).
- **Controlmodul (CM):** **ATtiny826** (QFN-20), Programmierung über UPDI
  (Header `PROG`).
- **Leistungsteil:** Gate-Treiber **IX4427** (`MD1`) und
  N-MOSFET **MS15N10** (`M2`, 100 V, TO-252) für das Heizelement. Ein
  kleiner N-MOSFET `M1` (SOT-23) schaltet eine weitere Last
  (vermutlich Lüfter, noch zu bestätigen).
- **Strommessung:** **ACS711KEXLT-31AB** (`CM2`, 31 A) ist bestückt,
  aber ohne Funktion: Die Firmware liest ihn nicht aus. Die Messung
  hat sich laut Owner als nicht besonders brauchbar erwiesen. Die
  -31AB-Variante wurde verbaut, weil die -15AB damals nicht
  lieferbar war.
- **Sensorik:** Steckverbinder `IN1`, `IN2` (2-polig, NTC Heizelement
  und NTC Logo-Platte), `IN3` (4-polig, DHT11), Pull-ups `R8`/`R9`
  (10 kΩ, 0,1 %), Spannungsteiler Bordspannung `R10` 5 kΩ / `R11`
  2,2 kΩ. Ausgang `OUT1` (2-polig).
- **Anzeigen:** Fehler-LED (`ERROR`) und drei weitere LEDs.

## Offene Punkte

- Zuordnung Pin ↔ Steckverbinder ↔ Funktion: aus dem Schaltplan abgeleitet
  und mit der Firmware abgeglichen, siehe
  [`../docs/hardware-analyse-v3.2.md`](../docs/hardware-analyse-v3.2.md);
  noch nicht gegen die gebaute Platine bestätigt.
- **Debug-Ausgang:** PB2 (USART0 TX) ist auf V3.2 nur am Controller-Pad erreichbar
  (Pin 12 des QFN-20), also nur per dünnem Draht. Für V4 ein eigenes Pad oder
  eine Stiftleiste (TX, GND) vorsehen.
- **I2C am ATtiny826:** Das Hardware-I2C (TWI0) liegt nur auf **PA5 (SDA)** und
  **PA6 (SCL)** (Datenblatt, Tabelle "PORT Function Multiplexing"). Dort sitzen
  auf V3.2 der Spannungsteiler (PA5) und die Fehler-LED (PA6). Ein I2C-Sensor
  braucht in V3.2 Software-I2C auf den freien Pins PB0/PB1; für V4 sollten
  PA5/PA6 freigemacht werden. Flash des ATtiny826: 8 KB (Build mit 78 %
  belegt); ein Typ mit mehr Flash (z. B. ATtiny1626) ist zu erwägen.
- Versorgung des ACS711 über einen Spannungsteiler (R15/R16), vermutlich
  zu schwach (siehe Analyse). Keine Sicherung/TVS/Verpolschutz im
  Schaltplan.
- PA4 (Selbsthaltung des Netzteils) wird vom Referenz-Sketch dauerhaft auf
  LOW gezogen (siehe Analyse und Audit F-10).
- ACS711: bestückt ist der **-31AB**; das Footprint im Schaltplan
  heißt **-15AB** (gleiche Bauform, nur anderer Messbereich). Das ist
  bewusst so, siehe oben.
- `M1` hat im Schaltplan keinen konkreten Typ (nur "N_MOSFET_1").
- **NTC:** In V3.x ist ein **10-kΩ-NTC** verbaut (passt zu den
  10-kΩ-Pull-ups und zur Firmware, B = 3435). Das Teil ist bei LCSC
  nicht mehr gelistet; ein Ersatz mit 10 kΩ muss geprüft werden (B-Wert
  in der Firmware anpassen). Die Datenblätter der V2 gehörten zu einem
  100-kΩ-NTC (B3950, MF52A) und gelten für V3 nicht.
- Frühe Datenblätter nannten den MOSFET HXY3422MI. Der Schaltplan
  V3.1/V3.2 enthält den **MS15N10**.

## Stückliste

[`V3.2/BOM_V3.2.csv`](V3.2/BOM_V3.2.csv), aus dem Schaltplan erzeugt
(Referenz, Wert, Gehäuse, Herstellernummer, LCSC-Nummer). Bei einigen
Passiven (generische 0603-Bauteile) steht im Schaltplan keine LCSC-Nummer;
die müssen vor einer Bestellung gewählt werden.

## Datenblätter

Die Datenblätter der Hersteller liegen nicht im Repo (Urheberrecht und
Größe). Über die LCSC-Nummern der Stückliste sind sie bei LCSC
abrufbar.

## Gerber-Dateien

Je Version als ZIP (`Gerber_*.zip`) im jeweiligen Ordner. Beim Bestellen
beachten: V3.2 ist eine 4-Lagen-Platine (Innenlagen enthalten).

## Hinweis

Alle Hardware ist ein Entwicklungsstand. Siehe
[`../DISCLAIMER.md`](../DISCLAIMER.md).
