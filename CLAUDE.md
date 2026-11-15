# ACC-Radar-Heizung

Autonome Logo-/Radarheizung gegen vereiste ACC-Sensoren (Front Assist / ACC) — Hobby- und Entwicklungsprojekt, Referenzfahrzeug VW Golf 7 Facelift. Open Source (Hardware, Firmware, CAD), nicht für den öffentlichen Straßenverkehr freigegeben.

## Start Here

1. `README.md` — Projektüberblick
2. `DECISIONS.md` — chronologisches Entscheidungsprotokoll
3. `DISCLAIMER.md` — Haftungs- und Sicherheitshinweise (gelten für
   alles, was hier entsteht)
4. `AGENTS.md` — tool-neutrale Kurzfassung der Regeln, auch für
   Mitwirkende mit anderen Coding-Agenten

## Routing Logic

- Einfache, einzeldateige, offensichtliche Aufgaben: direkt erledigen
- Domänenspezifische Arbeit: an den passenden Subagenten delegieren
  (siehe `.claude/agents/`): `firmware-dev` (ATtiny826-Sketch),
  `hardware-dev` (Schaltplan/Layout/Gerber/Stückliste), `cad-dev`
  (3D-Scan, Düse, Gehäuse). Jede Änderung an Sicherheitslogik oder
  Leistungsteil zusätzlich an `safety-reviewer`
- Persönliche Agenten von Mitwirkenden gehören nach `~/.claude/agents/`
  oder in `.claude/agents/local/` (gitignored), nicht ins geteilte Repo
- Vor jedem Commit/Push: Review an den
  `git-maintainer`-Subagenten delegieren (Diff-Check, Security-Check,
  Branch-Regel) — `git-maintainer` prüft und liefert ein
  Verdikt, committet/pusht aber nicht selbst (ein Subagent kann nicht
  auf eine von einem anderen Agenten weitergereichte "Nutzer hat
  bestätigt"-Aussage handeln, nur auf direkten Nutzerkontakt). Die
  Hauptsession committet gemäß Verdikt direkt auf `main`, wenn der
  Mitwirkende Maintainer ist, sonst über Feature-Branch + Pull Request

## Doc Inbox

`Doc_Inbox/` ist die Zwischenablage für eingehende Handoff-Dateien,
git-getrackt, damit sie über Sessions/Geräte hinweg sichtbar bleiben.
`git-maintainer` prüft es (gleiche Aufteilung wie überall: er
verifiziert, die Hauptsession committet gemäß Owner-/Branch-Regel).
Details: `.claude/rules/general.md`.

## Quality Standards

- Hardware: Änderungen am Schaltplan/Layout sind erst "fertig", wenn
  ERC/DRC sauber, Stückliste und Gerber neu exportiert und die
  Pinbelegung dokumentiert ist; "getestet" heißt auf echter Platine
  (Version angeben), nicht nur Simulation
- Firmware: baut für ATtiny826; Sicherheitsabschaltungen unverändert
  oder bewusst und begründet geändert (Review durch `safety-reviewer`)
- Aussagen zu Fahrzeugkompatibilität nur für tatsächlich getestete
  Fahrzeuge (aktuell: Golf 7 Facelift)

## Source of Truth

- `DECISIONS.md` = chronologisches Entscheidungsprotokoll
- Chat-Konversationen sind KEINE Quelle der Wahrheit — was nicht
  schriftlich festgehalten ist, ist nicht passiert
- Maßgeblich für den Hardware-Stand: die EasyEDA-Quellen und die
  Gerber-Dateien der jeweiligen Version in `hardware/`
- Maßgeblich für das Verhalten: der Firmware-Sketch in `firmware/`
  (aktuell Referenzstand: Platine V3.2 + Software V3.1_NewQFNDesign_V2)

## Working Principle

- IMMER eine Datei lesen, bevor sie geändert wird — nie den Inhalt aus
  dem Gedächtnis annehmen
- Nach Kontextverlust / neuer Session: diese Datei → README.md →
  relevante Docs neu lesen, bevor weitergearbeitet wird
- Entscheidungen in `DECISIONS.md` festhalten, wenn sie getroffen
  werden, nicht rückwirkend

Diese Datei schlank halten — sie wird jede Session gelesen.
