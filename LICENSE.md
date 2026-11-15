# Lizenzen

Copyright (c) 2024–2026 ACC-Radar-Heizung contributors

Dieses Projekt ist in Bereiche mit jeweils eigener Lizenz aufgeteilt. Die
vollständigen Lizenztexte liegen im Ordner [`LICENSES/`](LICENSES/)
(Dateiname = SPDX-Kennung).

| Bereich | Pfad | Lizenz |
|---|---|---|
| **Hardware**: Schaltpläne, Layouts, Gerber-Dateien, Stückliste | `hardware/` | [CERN-OHL-S-2.0](LICENSES/CERN-OHL-S-2.0.txt) (CERN Open Hardware Licence v2, strongly reciprocal) |
| **Firmware**: Arduino-Sketche, Datenlogger | `firmware/` | [GPL-3.0-or-later](LICENSES/GPL-3.0-or-later.txt) |
| **Dokumentation, Messdaten, CAD/Mechanik** und Projektkonfiguration | alles übrige (u. a. `README.md`, `*.md`, `logs/`, `vehicles/`, `.claude/`) | [CC-BY-SA-4.0](LICENSES/CC-BY-SA-4.0.txt) |

Im Zweifel gilt die Lizenz des Bereichs, in dem die Datei liegt. Die
Firmware-Dateien tragen zusätzlich einen
`SPDX-License-Identifier`-Kommentar.

## Hinweise zu CERN-OHL-S

Die CERN-OHL-S verlangt, dass abgeleitete Hardware vollständig unter
derselben Lizenz offengelegt wird und dass die Fundstelle der Quellen
angegeben wird:

- **Source Location** (Fundstelle der Hardware-Quellen): https://github.com/sandboop/ACC-Radar-Heizung

## Nicht mitlizenziert

- Datenblätter und Bauteildaten der Hersteller (nicht im Repo),
  Bauteil-/Symbolbibliotheken von EasyEDA/LCSC innerhalb der
  Schaltplan-Exporte unterliegen ihren eigenen Bedingungen.
- Marken (z. B. Volkswagen, Golf) gehören ihren Inhabern; das Projekt
  ist nicht mit dem Hersteller verbunden.

## Haftung

Siehe [`DISCLAIMER.md`](DISCLAIMER.md). Zusätzlich gilt der
Gewährleistungs- und Haftungsausschluss der jeweiligen Lizenz.
