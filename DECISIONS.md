# Entscheidungsprotokoll — ACC-Radar-Heizung

Chronologisches Protokoll aller Architektur-/Konventions-
Entscheidungen. Historische Einträge werden bei späteren Änderungen
nicht rückwirkend angepasst — dieses Dokument ist ein Protokoll, kein
lebendes Spec-Dokument.

## 2026-11-15 — Repo angelegt

- Als privates Arbeitsrepo angelegt. Anlass:
  Ein Forenmitglied (golf7freunde.de) hat nach dem Projekt
  gefragt; das seit 2024 ruhende Hobbyprojekt wird aufgearbeitet,
  dokumentiert und soll später als Community-Projekt auf GitHub
  öffentlich werden.
- Beteiligt: der Projektinhaber, ein Interessent aus dem Forum (möglicher
  Mitwirkender) und ein Beitragender, der Ideen beigesteuert hat (kein Code,
  Namensnennung offen).
- Name `ACC-Radar-Heizung` statt `ACC-Heizung-VW-ETC`: Zweck und
  Radarbezug klar, modellneutral. Golf 7 Facelift ist Referenzfahrzeug,
  weitere Fahrzeuge nur nach tatsächlichem Test.
- Sprache des Repos: Deutsch (Commit-Messages Englisch).
- Lizenzen getrennt je Bereich geplant (Hardware CERN-OHL, Firmware
  GPL-3.0 oder MIT, CAD/Doku CC BY-SA 4.0). Konkrete Auswahl und
  LICENSE-Dateien **vor** der Veröffentlichung noch festzulegen.
- Keine Scans/STL/3MF/OBJ/ASC/Fusion-Dateien im Git
  (`.gitignore`). Optimierte Varianten später auf GitHub, Rohdaten per
  Download-Link.
- Referenzstand: Platine V3.2 + Firmware V3.1_NewQFNDesign_V2. Gebaut
  und getestet laut Owner: V3.1 (PSU + CM) und V3.2; V2 nur
  Fertigungsdaten (Archiv).
- Agenten: `git-maintainer`, `firmware-dev`, `hardware-dev`, `cad-dev`
  und zusätzlich `safety-reviewer` (Ergänzung zum Template, weil das
  Projekt eine Heizung am Bordnetz steuert). Persönliche Agenten von
  Mitwirkenden bleiben lokal (`.claude/agents/local/`); `AGENTS.md`
  bietet eine tool-neutrale Kurzfassung.
- Plattform-Plan: bis zur Veröffentlichung alles im privaten Arbeitsrepo; danach
  GitHub als öffentliche Heimat (Issues, Releases, Discussions); das
  Arbeitsrepo spiegelt nur Git-Inhalt, nicht Issues/Releases/Wiki.

## 2026-11-15 — Inhalte aus dem Altverzeichnis übernommen

- Struktur: `hardware/` (je Version ein Ordner, Archiv für V1/V2/frühe
  V3), `firmware/` (Referenzsketch `acc_radar_heizung`, Altstände in
  `archiv/`), `logs/`, `vehicles/golf7-facelift/` (nur Doku, Dateien
  liegen lokal).
- Laut Owner: V3.1 war ein Versuch (zwei Platinen PSU/CM), **V3.2 ist
  die getestete und gefertigte Platine**, V3.3 ist derselbe Schaltplan
  mit platzsparender Anordnung (nicht gefertigt). Der Schaltplan
  `Radarheating_V3.1` gilt für V3.1, V3.2 und V3.3.
- Referenz-Sketch umbenannt (`RadarHeatingV3.1_NewQFNDesign_V2` →
  `acc_radar_heizung`, Arduino verlangt gleichen Ordner-/Dateinamen);
  Kodierung der Kommentare repariert (doppelt kodiertes UTF-8);
  `#include "arduino_secrets.h"` entfernt, die (leeren)
  `arduino_secrets.h` und Compiler-Artefakte (`.lst`, `.map`) nicht
  übernommen. Logik unverändert.
- Herstellerdatenblätter (PDF) nicht ins Repo übernommen (Urheberrecht,
  Größe); Stückliste mit LCSC-Nummern als Ersatz.
- `Kostenauswertung.xlsx` und die leere `BOM_..._2024-04-06.csv` aus V2
  nicht übernommen.
- Bekannte Review-Punkte der Firmware stehen in `firmware/README.md`
  (nicht geändert, nur dokumentiert).

## 2026-11-15 — Klarstellungen des Owners, Logger, Name entfernt

- NTC in V3.x: 10 kΩ (bei LCSC nicht mehr gelistet). ACS711: bestückt
  ist der -31AB (-15AB war nicht lieferbar), Strommessung auf der
  Platine vorhanden, aber ohne Funktion und laut Owner wenig brauchbar.
- Logger-Sketch (`firmware/datenlogger/`) übernommen; er klärt
  Zeitformat (ms), Flag (Ereignis-Taster = 15) und Messaufbau der Logs.
- Der Klarname des Owners im Zeichnungsrahmen aller EasyEDA-Schaltpläne
  wurde durch "ACC-Radar-Heizung" ersetzt (Vorbereitung der
  Veröffentlichung). Die alte Historie des Arbeitsrepos enthielt den Namen und eine
  Autor-E-Mail; deshalb wird dieses Repo ohne alte Historie veröffentlicht
  (siehe Eintrag zur Veröffentlichung).

## 2026-11-15 — Lizenzen festgelegt

- Hardware: **CERN-OHL-S-2.0** (starkes Copyleft, abgeleitete Hardware
  muss offen bleiben). Firmware: **GPL-3.0-or-later**. Dokumentation,
  Messdaten, CAD und Projektkonfiguration: **CC-BY-SA-4.0**.
- Urheberzeile neutral: "ACC-Radar-Heizung contributors" (kein
  Klarname). Lizenztexte aus der SPDX-Lizenzliste in `LICENSES/`,
  Übersicht in `LICENSE.md`, SPDX-Kopfzeilen in den aktuellen Sketchen.
- Offen: "Source Location" für die CERN-OHL-S in `LICENSE.md` nach
  der Veröffentlichung eintragen.
- "GPL-3.0" wurde als "or-later" umgesetzt; falls nur "only" gewollt
  ist, SPDX-Kennung und Datei anpassen.

## 2026-11-15 — Firmware-Sicherheitsumbau (Audit F-01 bis F-05, F-10)

- Umgesetzt (alles **nicht kompiliert und nicht auf Hardware getestet**):
  Spannungsfreigabe mit Hysterese (F-01/F-06), NTC-Kurzschlusserkennung
  (F-02), `safety()` direkt nach `get_inputs()` mit sofortigem Abschalten
  des Heizungspins (F-03), zeitgesteuerte Fehler-LED statt `delay()`
  (F-03), Lüfter-Nachlauf 30 s (F-05, Wert vorläufig), Hardware-Watchdog
  ca. 2 s (F-04), PA4 als Netzteil-Selbsthaltung (F-10).
- Entscheidung PA4: Mit den Datenblattwerten (LMR14020: EN-Schwelle
  1,05/1,20/1,38 V, 1/4,6 µA EN-Strom; Teiler 220 k/21,5 k, R12 56 k)
  hält PA4 = LOW das Netzteil nur über 14,9–19,9 V; der Referenz-Sketch
  konnte so nicht stabil laufen. Daher: PA4 sofort HIGH beim Start,
  Selbstabschaltung nur bei Motor aus (20 s ohne Spannungsfreigabe), im
  Fehlerfall bleibt der Controller bei laufendem Motor an (Fehlercode
  sichtbar, kein Neustart-Zyklus). Spannungsmessung läuft deshalb auch im
  Fehlerzustand weiter.
- Hardware-Hinweis: Das Einschalten der Hardware ohne Controller hängt an
  der Toleranz der Enable-Schwelle (11,6–15,3 V). Für V4 prüfen.
- ACS711 liegt über den Teiler R15/R16 unter seiner Mindestversorgung
  (3,0 V); Behebung nur per Platinenänderung.

## 2026-11-15 — Build-Nachweis der Firmware

- Der Referenz-Sketch wurde mit dem `arduino-cli` der Arduino IDE 2
  (megaTinyCore 2.6.11, Board `megaTinyCore:megaavr:atxy6:chip=826`, DHT
  sensor library 1.4.7, Adafruit Unified Sensor 1.1.15) übersetzt: keine
  Fehler, keine Warnungen im Sketch (`--warnings all`), 6412/8192 Byte Flash
  (78 %), 227/1024 Byte RAM (22 %). Damit sind die Änderungen aus dem
  Sicherheitsumbau (Eintrag oben) **gebaut, aber weiterhin nicht auf
  Hardware getestet**. Der Befehl steht in `firmware/README.md`.

## 2026-11-15 — Verhaltensspezifikation und Konzeptentscheidungen

- Neu: `docs/verhalten.md` (Zustandsautomat, Zeiten, Wetterbedingung,
  Kennlinie, Sicherheitsergänzungen). Grundlage für den Umbau der Firmware.
- Entscheidungen des Owners: Auslösung per **Taupunkt/Plattentemperatur**
  (Variante B); Solltemperatur **abhängig von der Außentemperatur**;
  Sicherheitsergänzungen **Thermosicherung/Thermostat**, **Eingangssicherung**,
  **EEPROM-Fehlerspeicher**, **Debug-Ausgang**; **Sensorwechsel erst mit V4**.
- Umsetzung dieser Entscheidungen (Abweichungen bewusst und begründet):
  - Taupunkt mit dem DHT11 (±5 % rF, ±2 °C) ist um mehrere Kelvin ungenau.
    Daher bleibt **Variante A (kalt + feucht, mit Hysterese) der Standard**,
    bis der bessere Sensor (V4) und Messdaten vorliegen; Variante B ist
    spezifiziert und wird als Experiment umgesetzt.
  - Die Solltemperatur wird als **Kennlinie** über der Außentemperatur
    spezifiziert (Stützpunkte), nicht als "Außentemperatur + Differenz", das
    würde bei milder Witterung wärmer heizen als bei Frost. Startwerte
    konstant 19,5 °C (Verhalten wie bisher), Werte nach Fahrzeugtest.
- Hardware-Erkenntnis: Hardware-I2C des ATtiny826 liegt auf PA5/PA6 (belegt);
  V3.2 nur Software-I2C auf PB0/PB1; für V4 Pins verlegen. Flash ist mit 78 %
  knapp; ein Typ mit mehr Flash erwägen.
- 8 neue Issues zu Spezifikation, Umbau, Thermosicherung, Fehlerspeicher,
  Debug-Ausgang, Taupunkt-Experiment, Kennlinie und I2C-Sensor.

## 2026-11-15 — Firmware-Umbau auf eigenem Branch

- Versionierung: Der Stand vor dem Umbau wurde im Arbeitsrepo getaggt
  (Rücksprungpunkt); der Umbau lief auf einem eigenen Branch und über einen
  Pull Request.
- Umgesetzt: Zustandsautomat nach `docs/verhalten.md`, zentrale `config.h`,
  Luftsensor-Schicht, Wetterbedingung mit Hysterese, Kennlinie der
  Solltemperatur (zunächst konstant), Netzteil-Selbstabschaltung erst nach
  Ende des Nachlaufs. Baut ohne Warnungen: 6794 Byte Flash (82 %), 218 Byte RAM.
- Prüfung der Logik per Python-Modell der Szenarien Kaltstart, Motor aus,
  NTC-Kurzschluss, Luftsensor-Ausfall, kurzer Wetterwechsel, warm/trocken.
  Dabei gefunden und behoben: die Selbstabschaltung (20 s) hätte den
  Lüfter-Nachlauf (30 s) abgeschnitten.
- Neu: Feuchteschwelle 80 % (statt Testwert 30 %); zum Test im Zimmer
  `WX_RH_ON_PCT` senken.

## 2026-11-15 — Flash sparen: Festkomma, NTC-Tabelle, eigener DHT11-Treiber

- Messung einzelner Bausteine (Testsketche): `analogRead` ca. 90 Byte, Watchdog
  ca. 0, `log()` und Fließkomma ca. 1,4 KB, **DHT-Bibliothek ca. 3 KB** und
  zog zudem `HardwareSerial` mit ein. Die Standardfunktionen `pinMode`,
  `digitalWrite`, `analogRead` kosten zusammen nur ca. 300 Byte; direkter
  Registerzugriff lohnt sich nur dort, wo er Timing oder Genauigkeit bringt.
- Umgesetzt: Festkomma (0,1 °C, 0,1 %, mV), NTC über Tabelle
  (`tools/gen_ntc_table.py`, anderer NTC = neue Tabelle, kein Codewechsel),
  eigener DHT11-Treiber mit direktem Portzugriff, die Bibliothek bleibt als
  Vergleichsvariante in `config.h` wählbar.
- Ergebnis: 6794 Byte (82 %) → 5786 Byte (70 %, nur Festkomma/Tabelle) →
  **3274 Byte (39 %)** mit eigenem Treiber; RAM 218 → 72 Byte. Der eigene
  Treiber ist per Simulation der Bitauswertung geprüft, nicht an einem echten
  Sensor.
- Ausgänge über `digitalWriteFast`/Register (ca. 150 bis 250 Byte) bewusst noch
  nicht umgesetzt: kleiner Gewinn gegen Lesbarkeit; bei Bedarf in einer
  `hal.h` bündeln.

## 2026-11-15 — EEPROM-Fehlerspeicher, Debug-Ausgang, Prüfstand-Checkliste

- Umgesetzt (Issues 26 und 27, auf dem Branch): Ringspeicher mit 24 Einträgen
  im EEPROM (`faultlog.ino`), Debug-Ausgang als USART0-TX auf PB2 mit Direktzugriff
  auf die Register (`debug.ino`, ohne die Serial-Klasse), Watchdog-Reset als Code 6.
- Entscheidungen: Es wird nur bei neuem Fehler/Watchdog-Reset geschrieben
  (kein Boot-Zähler: ein Neustart-Zyklus durch die Toleranz der Enable-Schwelle
  würde das EEPROM verschleißen); Schreiben erst nach den Ausgängen und ein Byte je
  Durchlauf (ein EEPROM-Zugriff kann den Controller anhalten); Reset-Ursache aus
  `GPIOR0` (der Core sichert die Reset-Flags dort); Debug-Puffer verwirft Zeichen
  statt zu blockieren.
- Speicher: Fehlerspeicher ca. 0,5 KB, Debug-Ausgang ca. 1,6 KB; Standard
  5388 Byte (65 %), mit `DEBUG_OUTPUT = 0` 3810 Byte (46 %).
- Hardware: PB2 hat auf V3.2 nur das Controller-Pad (Pin 12). Für V4 ein Pad
  für TX vorsehen.
- Prüfstand: `docs/pruefstand.md` mit Material, Ersatzwiderständen für die NTC und 45
  Testpunkten (T01 bis T65) mit Sollwerten; Ergebnisse als Kommentar in Issue 1.

## 2026-11-15 — Veröffentlichung

- Entscheidung des Projektinhabers: **jetzt veröffentlichen**, mit deutlichem Hinweis "Pre-Alpha, ungetestet";
  **frischer Start mit einem Commit** ohne die alte Historie (sie enthielt Klarname und E-Mail) und mit
  neutraler Identität; **GitHub ist die Heimat** (Issues, Pull Requests, Discussions, Releases).
- Öffentlich: Hardware (Schaltpläne, Gerber, Stückliste), Firmware und Datenlogger, Messreihen. Optimierte
  STL-Dateien folgen später als Release-Anhang, Rohdaten des 3D-Scans per Download-Link.
- Bereinigt für die Veröffentlichung: Personennamen und interne Infrastruktur aus Doku und Agentendateien
  entfernt, der Archiv-Sketch `v2_variante_beitrag` ohne Namen, `git-maintainer` auf GitHub ausgerichtet.
- Neu: `CONTRIBUTING.md`, `SECURITY.md`, `CODE_OF_CONDUCT.md`, Issue-Vorlagen, Pull-Request-Vorlage und ein
  automatischer Build (GitHub Actions, noch nicht ausgeführt).

## 2026-11-15 — Hintergrund aus dem Forum zusammengetragen

- `docs/hintergrund-forum.md` fasst den Forenthread "ACC/Front Assist & Schnee(-regen)" (45 Beiträge,
  2017 und 2024) in eigenen Worten und ohne Nutzernamen zusammen (Problem, Sensorposition, Ideen,
  Rechtliches), damit nicht überall hin verlinkt werden muss. Die Zusammenfassung stammt aus
  maschinell erzeugten Auszügen und ist gegen den Thread zu prüfen; die Quellen sind Meinungen und
  Erfahrungen, nicht geprüft.

## 2026-11-15 — Hilfegesuche, Scan-Daten und KI-Mitarbeit

- Die Mitarbeit mit KI-Coding-Agenten ist **ausdrücklich erlaubt und erwünscht** (README, CONTRIBUTING, AGENTS.md,
  Pull-Request-Vorlage mit Feld zur KI-Nutzung). Das Projekt weist transparent aus, dass es mit Unterstützung
  eines KI-Coding-Agenten aufgearbeitet wurde.
- Das README führt eine Tabelle "Hilfe gesucht" mit Einstiegen; die Issues wurden mit `help wanted` und
  `good first issue` gekennzeichnet; neue Issues #31 (Scan- und CAD-Daten bereitstellen) und #32 (Düse und Gehäuse
  konstruieren), neues Label `mechanik`.
- Scan- und CAD-Daten sind sehr groß (einzelne Dateien bis 550 MB, ca. 3,7 GB), die "optimierte" Variante
  immer noch 330 MB. Geplant: reduzierter Ausschnitt (Emblem und Radarbereich) als Release-Anhang, Rohdaten per Link;
  bis dahin Bereitstellung auf Anfrage.

## 2026-11-15 — Aussage zur Herstellerrolle offen formuliert

- Die absolute Aussage "es werden keine Platinen oder Bausätze verkauft" wurde durch "derzeit werden keine
  Platinen oder Bausätze verkauft oder abgegeben" ersetzt (DISCLAIMER.md, README.md). Sollte sich das ändern,
  wird es angekündigt, und wer Hardware abgibt, trägt die Herstellerverantwortung. Vor einer Abgabe sind
  Produktsicherheit, EMV, Haftung und Steuern zu klären (rechtliche Beratung, Versicherung). Eine
  Interessenabfrage im Forum ist kein Angebot.

## 2026-11-15 — FAQ

- `docs/faq.md` beantwortet häufige Fragen zu Zulassung und Recht, Verkauf (derzeit keiner, offen formuliert),
  Stand, Funktion, Fahrzeugen, Nachbau, Sicherheit und Mitmachen. Fragen aus dem Forenthread (Tempomat-Ausfall,
  Sensorheizung, Versiegelung, Tönungsfolie, VCDS) sind als Forenaussagen gekennzeichnet und ungeprüft.

## 2026-11-15 — Scan-Daten veröffentlicht

- Release `scan-daten-v1` auf GitHub mit `Radar-3D-Scan.stl` (84 MB, 1,77 Mio. Dreiecke, aufbereitet zum Konstruieren) und
  `prototyp-v1-druckdateien.zip`; Beschreibung in `vehicles/golf7-facelift/scan-daten.md`.
- Einheit und Ausrichtung wurden gegen die Dateien geprüft: `Radar-3D-Scan.stl` ist in Millimetern (251 x 232 x 237 mm,
  Radarmodul ca. 70 bis 75 mm breit), Fahrtrichtung −Y, oben +Z. Die älteren Teil-Scans und der Gesamtscan liegen dagegen
  in **Metern** vor (Front 1,5 Einheiten breit); das ist dokumentiert, damit niemand mit falschem Maßstab startet.
- Die Datei ist nicht repariert (Löcher, nicht auf Wasserdichtheit geprüft); eine reparierte Fassung ist eine Mitmach-Aufgabe.
- Fahrzeug: Golf 7 R Variant (Facelift), HSN/TSN 0603/BRM (Fahrzeugtyp, nicht individuell), Baujahr 12/2019. Scanner-Einstellungen
  und Datum sind nicht mehr bekannt.
- Rechtlicher Hinweis zu Marken (VW-Emblem) im Release und in `scan-daten.md`.

## 2026-11-15 — Forenbeitrag verlinkt

- Der Beitrag zur Veröffentlichung wurde im Forum gepostet (eigener Thread) und ist im README, in der FAQ und in
  `docs/hintergrund-forum.md` verlinkt. Der Link enthält bewusst keine Sitzungskennung (`?s=...`).

## 2026-11-17 — Erster Pull Request, neutrale Autor-Adresse, Historie bereinigt

- Erster externer Pull Request (#33, überlaufsichere `millis()`-Vergleiche in Archiv-Sketchen und Datenlogger, schließt
  Issue #13) nach Review durch `git-maintainer` per **Squash-Merge** übernommen (Squash, damit die Platzhalter-Adresse
  des Commits nicht in die Historie gelangt). Die aktive Firmware war nicht betroffen, ein `safety-reviewer` war nicht nötig.
- Problem: Die neutrale Commit-Identität `noreply@users.noreply.github.com` ist für GitHub die Adresse des **realen Kontos**
  `noreply`. Dieses Konto wurde dadurch als einziger Contributor des Repos geführt.
- Entscheidung des Projektinhabers: neutrale Autor-Adresse künftig `contributors@acc-radar-heizung.invalid` (kein Konto kann
  sie beanspruchen), Name unverändert "ACC-Radar-Heizung contributors". Die bestehenden 8 Commits wurden dafür **umgeschrieben**
  (Force-Push auf `main`, Inhalt unverändert, Commit-SHAs neu). Das Release-Tag `scan-daten-v1` wurde auf den inhaltsgleichen
  neuen Commit gesetzt (Release und Dateien unverändert). Vorher bestehende SHAs sind ungültig.
- Maßgeblich für Beiträge Externer bleibt: Fork, Branch, Pull Request; Maintainer committen direkt auf `main`.

## 2026-11-17 — Luftführung und Düse dokumentiert

- Rückmeldung aus dem Forum (Frage zur Befestigung, Sorge wegen Fahrtwind und Temperatur, Vorschlag Flächenheizung) war
  Anlass, die Luftführung schriftlich festzuhalten: `docs/luftfuehrung.md` mit vier Skizzen in `docs/bilder/` (aus der
  Fusion-Konstruktion v17 über dem Scan, Oberfläche entfernt, komprimiert).
- Festgehalten: Es wird **nur die Emblemplatte** angeblasen, nicht der Radarsensor. Abluft ist nicht gezielt geführt. Die Düse ist
  in der jetzigen Form **nicht druckbar** (Streben), sie wird auf den Radarhalter geklebt. Die **Befestigung des Gehäuses ist
  offen** (Haubenverschluss mit mehreren Schraubpunkten oder ein geklemmter Halter).
- Einziger Erfahrungswert ist ein früherer Einzeltestlauf (Radiallüfter, 40 W, bei −3 °C Ansaugluft etwa 40 °C Ausblasluft,
  Heizelement etwa 100 °C, PETG-Halterung nicht geschmolzen), nicht protokolliert. Er wird so ausgewiesen, nicht als Nachweis.
- Regelgröße: Die Plattentemperatur gilt als Stellvertreter für die zugeführte Energie, nicht als Ziel an sich. Alle Grenz- und
  Sollwerte im Code sind Versuchswerte und werden später mit Versuchen angepasst und belegt (empirischer Temperaturkoeffizient
  denkbar). Kontaktlose Messung per Thermopile (z. B. MLX90614) neben der Düse ist eine **Idee, nicht entschieden**, damit der
  Aufbau fehlerärmer wird (NTC-Position beim Selbstbau streut). Position unter oder seitlich der Düse, MLX90614 nur als
  Beispiel (auch ein einzelnes Thermopile mit Auswertung auf der Platine denkbar). Bewusst **keiner Platinenversion
  zugeordnet**, ob sich der Aufwand lohnt, ist offen.
- Das Emblem-Material wird erst genannt, wenn es geprüft und fotografisch belegt ist.
- Warum Luft statt Flächenheizung: Metall und Wasser dämpfen Radarstrahlen. Die Flächenheizung ist nicht ausgeschlossen, ihre
  Dämpfung ist nicht gemessen, und die Steuerung ließe sich anpassen.
- Forumsantworten mit Nutzernamen gehören nicht ins Repo und nicht in `Doc_Inbox/`-Commits.
