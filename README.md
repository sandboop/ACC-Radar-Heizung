# ACC-Radar-Heizung

Autonome Heizung für das VW-Logo über dem ACC-/Front-Assist-Radar. Bei Schnee und Eis fällt der Radarsensor
hinter dem Emblem aus (Meldung im Kombiinstrument, ACC und Front Assist stehen nicht zur Verfügung). Diese
Heizung soll das verhindern, **ohne Metall vor die Radarantenne zu setzen**.

> ⚠️ **Pre-Alpha, ungetestet, nicht zugelassen.**
>
> - Das ist ein **privates Hobby- und Entwicklungsprojekt**, kein Produkt. Es ist **nicht für den öffentlichen
>   Straßenverkehr freigegeben**: keine Zulassung, keine ABE, keine CE-Kennzeichnung, **EMV ungeprüft**.
> - Die **aktuelle Firmware ist neu und wurde noch nicht auf Hardware getestet.** Sie baut, und die Logik ist
>   per Modell und Simulation geprüft, mehr nicht. Der Prüfstandtest steht aus ([`docs/pruefstand.md`](docs/pruefstand.md)).
> - Die Anlage schaltet eine **Heizung am 12-V-Bordnetz**. Fehler können zu Überhitzung, Brand oder einer
>   leeren Batterie führen. Die Funktion von Assistenzsystemen kann beeinträchtigt werden.
> - Nachbau und Nutzung **auf eigene Gefahr**. Bitte zuerst [`DISCLAIMER.md`](DISCLAIMER.md) lesen.

Referenzfahrzeug: **VW Golf 7 Facelift** (Radar hinter dem Logo). Die Elektronik ist fahrzeugunabhängig gedacht;
für andere Modelle braucht es eine eigene Mechanik. Kompatibilität wird nur für getestete Fahrzeuge behauptet.

## Funktionsprinzip

Ein ATtiny826 arbeitet als autonomer Regler und braucht nur die Bordspannung (kein Eingriff in das
Kommunikationssystem des Fahrzeugs, kein CAN-Bus):

- **Eingänge:** Luftsensor (DHT11) für Außentemperatur und Feuchte, NTC am Heizelement, NTC an der
  Logo-Platte, Spannungsteiler für die Bordspannung
- **Ausgänge:** Lüfter, Heizelement (PTC, bis ca. 40 W, unter 7 A), Fehler-LED
- **Ablauf:** Motor läuft (aus der Bordspannung erkannt) und kalt und feucht → Lüfter an → nach kurzer
  Verzögerung Heizelement, Zweipunktregelung der Plattentemperatur → Nachlauf und Standby, wenn die
  Bedingungen entfallen. Das Soll-Verhalten steht in [`docs/verhalten.md`](docs/verhalten.md).
- **Sicherheit:** sofortige Abschaltung bei Sensorfehlern und Übertemperatur, Watchdog, Fehlercode per
  LED, Fehlerspeicher im EEPROM. Bekannte Schwächen stehen offen in
  [`docs/sicherheits-audit-firmware.md`](docs/sicherheits-audit-firmware.md).

## Stand

| Bereich | Stand |
|---|---|
| Hardware | V3.1 (PSU + CM) und V3.2 (4 Lagen) gebaut; V3.3 nur Layoutversuch; V4 nur Schaltplan-Entwurf; V2 nur Fertigungsdaten (Archiv) |
| Firmware | neu strukturiert (Zustandsautomat, ohne Fließkomma, eigener DHT11-Treiber): **baut, nicht auf Hardware getestet** |
| Mechanik | Prototyp V1 (3D-Druck), Düse in Arbeit; Dateien (Scans, STL) noch nicht veröffentlicht |
| Fahrzeug | keine dokumentierten Fahrzeugergebnisse |

Das Gerät hat laut Projektinhaber mit einem früheren Firmware-Stand im Grundsatz funktioniert, ist aber weit
von einbaufertig entfernt.

## Repository

| Ordner | Inhalt |
|---|---|
| [`hardware/`](hardware/) | Schaltpläne und Layouts (EasyEDA), Gerber, Stückliste, Versionsübersicht |
| [`firmware/`](firmware/) | Firmware (ATtiny826, Arduino/megaTinyCore), Datenlogger, Archiv älterer Stände |
| [`docs/`](docs/) | Verhaltensspezifikation, Sicherheits-Audit, Hardware-Analyse, Prüfstand-Checkliste, Hintergrund aus dem Forum |
| [`logs/`](logs/) | Messreihen der Bordspannung (Golf 7 R) |
| [`tools/`](tools/) | Werkzeuge, z. B. Generator der NTC-Tabelle |
| [`vehicles/`](vehicles/) | Fahrzeugbezogene Doku (Golf 7 Facelift) |

## Loslegen

- **Firmware bauen:** [`firmware/README.md`](firmware/README.md) (`arduino-cli`, megaTinyCore 2.6.11)
- **Testen:** [`docs/pruefstand.md`](docs/pruefstand.md): Aufbau ohne Fahrzeug mit Lastwiderstand, Ersatzwiderständen
  und Debug-Ausgang. Berichte bitte über die Issue-Vorlage "Prüfstandbericht".
- **Hintergrund:** Erfahrungen und Ideen aus dem Forum in [`docs/hintergrund-forum.md`](docs/hintergrund-forum.md)
- **Hardware verstehen:** [`hardware/README.md`](hardware/README.md) und
  [`docs/hardware-analyse-v3.2.md`](docs/hardware-analyse-v3.2.md)

## Mitmachen

**Hilfe ist ausdrücklich erwünscht**, auch von Leuten, die gerade erst anfangen. Der Projektinhaber kann schon einiges
liefern (Scan der Front mit Emblem und Radar, erste Konstruktionsstände, Platinen-Quellen, Messreihen, Doku), braucht
aber Verstärkung:

| Aufgabe | Was du mitbringen solltest | Einstieg |
|---|---|---|
| Düse und Gehäuse konstruieren (CAD, 3D-Druck) | Fusion oder ähnliches CAD; Scan-Daten und bisherige Entwürfe werden bereitgestellt | [#32](https://github.com/sandboop/ACC-Radar-Heizung/issues/32), [#31](https://github.com/sandboop/ACC-Radar-Heizung/issues/31) |
| Platine V4: Layout, Schutzbeschaltung, kleinere Bauteile | EasyEDA oder KiCad | [#15](https://github.com/sandboop/ACC-Radar-Heizung/issues/15) bis [#19](https://github.com/sandboop/ACC-Radar-Heizung/issues/19) |
| Firmware-Review, besonders Sicherheitslogik | C/C++ (AVR) | [#2](https://github.com/sandboop/ACC-Radar-Heizung/issues/2) bis [#14](https://github.com/sandboop/ACC-Radar-Heizung/issues/14) |
| Prüfstandtests | Netzteil, Multimeter, Lust zu messen | [#1](https://github.com/sandboop/ACC-Radar-Heizung/issues/1) |
| Einbau- und Aufbauanleitung, Fotos | Schreiben, Fotografieren | [#20](https://github.com/sandboop/ACC-Radar-Heizung/issues/20) |
| Messungen an anderen Fahrzeugen | Fahrzeug, Datenlogger | [#29](https://github.com/sandboop/ACC-Radar-Heizung/issues/29) |

Aufgaben mit dem Label [help wanted](https://github.com/sandboop/ACC-Radar-Heizung/labels/help%20wanted) und [good first issue](https://github.com/sandboop/ACC-Radar-Heizung/labels/good%20first%20issue)
eignen sich zum Einstieg. Wie es geht: [`CONTRIBUTING.md`](CONTRIBUTING.md). Bitte beachte den
[Verhaltenskodex](CODE_OF_CONDUCT.md) und [`SECURITY.md`](SECURITY.md) für sicherheitsrelevante Meldungen.

### Mit KI-Coding-Agenten mitmachen: ausdrücklich erlaubt und erwünscht

Du darfst und sollst gern deinen eigenen KI-Coding-Agenten einsetzen (zum Beispiel Claude Code, GitHub Copilot, Cursor
oder Codex), um das Projekt zu verstehen, Code oder Doku zu schreiben, Schaltungen zu prüfen oder Tests vorzubereiten.
Das Repo ist darauf vorbereitet: [`AGENTS.md`](AGENTS.md) (tool-neutral), [`CLAUDE.md`](CLAUDE.md) und `.claude/`
(Agenten für Firmware, Hardware, CAD und Sicherheitsprüfung). Die Spielregeln stehen in
[`CONTRIBUTING.md`](CONTRIBUTING.md): Du bleibst für deinen Beitrag verantwortlich, kennzeichnest im Pull Request, was
geprüft ist, und achtest bei Sicherheitslogik besonders auf Sorgfalt.

### Transparenz

Das Projekt wurde mit Unterstützung eines KI-Coding-Agenten (Claude Code) aufgearbeitet und überarbeitet: Doku,
Sicherheitsanalyse, große Teile der Firmware. Das Ergebnis ist **nicht automatisch richtig**. Deshalb sind Prüfstand
und Review so wichtig, und deshalb steht der Hinweis "ungetestet" ganz oben.

## Lizenz

Getrennt je Bereich, Details in [`LICENSE.md`](LICENSE.md):

- Hardware (`hardware/`): CERN-OHL-S-2.0
- Firmware (`firmware/`): GPL-3.0-or-later
- Dokumentation, Messdaten, CAD und Konfiguration: CC-BY-SA-4.0

Es werden keine fertigen oder teilweise bestückten Platinen oder Bausätze verkauft; veröffentlicht werden nur
Quellcode, Konstruktions- und Fertigungsdaten und Dokumentation.
