# Mitmachen

Schön, dass du helfen willst! Dieses Projekt ist ein **Hobby- und Entwicklungsprojekt** ohne
Zulassung und ohne Gewähr. Bitte lies zuerst den [`DISCLAIMER.md`](DISCLAIMER.md). Wer hier mitarbeitet,
arbeitet an Hardware und Software, die eine Heizung am 12-V-Bordnetz steuert. Sorgfalt geht vor
Geschwindigkeit.

## Wobei Hilfe willkommen ist

- **Prüfstand und Fahrzeugtests:** Die Checkliste [`docs/pruefstand.md`](docs/pruefstand.md) nutzen und
  Ergebnisse berichten (Vorlage "Prüfstandbericht").
- **Firmware-Review:** besonders die Sicherheitslogik (`firmware/acc_radar_heizung/faults.ino`,
  `statemachine.ino`, `psu.ino`, `voltage.ino`) und das Soll-Verhalten in [`docs/verhalten.md`](docs/verhalten.md).
- **Platine (V4):** Layout, Schutzbeschaltung, kleinere Bauteile, siehe [`hardware/README.md`](hardware/README.md)
  und die offenen Punkte in [`docs/hardware-analyse-v3.2.md`](docs/hardware-analyse-v3.2.md).
- **Mechanik/CAD:** Düse und Gehäuse auf Basis eines 3D-Scans.
- **Andere Fahrzeuge:** Messungen und Konstruktion für weitere Modelle (Kompatibilität wird nur für getestete
  Fahrzeuge behauptet).
- **Dokumentation:** Aufbau- und Einbauanleitung, Korrekturen.

## Ablauf

1. **Issue anlegen oder kommentieren**, bevor du größere Änderungen angehst. Wähle die passende Vorlage.
2. **Fork** des Repos, darin ein Branch (`feat/...`, `fix/...`, `docs/...`).
3. **Pull Request** gegen `main`. Beschreibe, was du geändert hast, **was du wie geprüft hast** und was
   ungeprüft ist. Der Maintainer entscheidet über das Zusammenführen.
4. Änderungen an **Sicherheitslogik** (Abschaltungen, Grenzwerte, Leistungsteil, Netzteil-Haltung) bitte
   im PR deutlich hervorheben und begründen. Sie werden besonders streng geprüft.

## Regeln

- **Sprache:** Dokumentation, Issues und Pull Requests auf Deutsch; Commit-Nachrichten auf Englisch im Stil
  von [Conventional Commits](https://www.conventionalcommits.org/) (`feat:`, `fix:`, `docs:`, `refactor:`).
- **Lizenz:** Dein Beitrag steht unter der Lizenz des Bereichs, in dem er liegt (siehe [`LICENSE.md`](LICENSE.md):
  Hardware CERN-OHL-S-2.0, Firmware GPL-3.0-or-later, Dokumentation/CAD/Messdaten CC-BY-SA-4.0). Mit dem
  Einreichen bestätigst du, dass du die Rechte daran hast. Empfohlen: `git commit -s` (Sign-off).
- **Keine Geheimnisse und keine persönlichen Daten:** keine Zugangsdaten, keine `arduino_secrets.h`, keine
  Klarnamen in EasyEDA-Zeichnungsrahmen, keine Fahrzeugidentifikation (FIN, Kennzeichen) in Logs.
- **Keine großen Dateien:** keine 3D-Scans, STL-/OBJ-/3MF-Dateien oder CAD-Quellen im Git. Dafür gibt es
  Release-Anhänge oder Download-Links. Bilder vor dem Hochladen auf höchstens ca. 500 KB komprimieren.
- **Herstellerdatenblätter** nicht ins Repo legen (Urheberrecht); die LCSC-Nummer in der Stückliste genügt.
- **Nichts behaupten, was nicht getestet ist:** Status "gebaut", "getestet", "im Fahrzeug erprobt" immer
  angeben.

## Firmware lokal bauen

Mit dem `arduino-cli` (oder der Arduino IDE 2) und megaTinyCore 2.6.11, Board ATtiny826:

```
arduino-cli compile -b megaTinyCore:megaavr:atxy6:chip=826 --warnings all firmware/acc_radar_heizung
```

Der Build muss **ohne Warnungen** laufen; das prüft auch der automatische Build (GitHub Actions). Die
NTC-Tabelle `ntc_table.h` ist eine erzeugte Datei: nicht von Hand ändern, sondern `tools/gen_ntc_table.py`
anpassen und neu ausführen.

## KI-Coding-Agenten: ausdrücklich erlaubt und erwünscht

Die Mitarbeit **mit** KI-Coding-Agenten (Claude Code, GitHub Copilot, Cursor, Codex und andere) ist hier **ausdrücklich
erlaubt und erwünscht**. Das Projekt wurde selbst mit einem Agenten aufgearbeitet. Gerade wer wenig Zeit oder wenig
Erfahrung mit Firmware, CAD oder Platinen hat, kann mit Agentenhilfe viel beitragen.

**Wie das Repo Agenten unterstützt:**
- [`AGENTS.md`](AGENTS.md) fasst die Regeln tool-neutral zusammen; [`CLAUDE.md`](CLAUDE.md) und `.claude/` enthalten
  zusätzlich Agenten für Firmware (`firmware-dev`), Hardware (`hardware-dev`), Mechanik (`cad-dev`) und die
  Sicherheitsprüfung (`safety-reviewer`) sowie einen Prüfer für Änderungen (`git-maintainer`).
- Die Doku (`docs/`) ist so geschrieben, dass ein Agent das Soll-Verhalten, die bekannten Schwächen und die
  Prüfstand-Checkliste lesen kann, bevor er etwas ändert.

**Spielregeln:**
- **Du bleibst verantwortlich** für das, was du einreichst, auch wenn ein Agent es geschrieben hat. Lies und verstehe es,
  bevor du es einreichst.
- **Kennzeichne im Pull Request**, ob und wofür KI genutzt wurde und **was du geprüft hast** (Build, Prüfstand,
  Review). Ungeprüftes bitte ausdrücklich so benennen.
- **Sicherheitslogik** (Abschaltungen, Grenzwerte, Leistungsteil, Netzteil-Haltung) nie ungeprüft übernehmen. Ein
  Agent kann plausibel klingende, aber falsche Werte liefern, besonders bei Bauteildaten und Grenzwerten: Datenblatt
  und Messung gehen vor.
- **Nichts erfinden lassen:** Keine Aussagen zu Zulassung, Fahrzeugkompatibilität oder Tests, die nicht belegt sind.
- **Keine Geheimnisse und keine persönlichen Daten** in Prompts oder Dateien (Zugangsdaten, `arduino_secrets.h`,
  Klarnamen, Fahrzeugidentifikation).
- **Persönliche Agenten, Prompts und Einstellungen** gehören nicht ins Repo: lokal halten (`~/.claude/agents/` bzw.
  `.claude/agents/local/`, steht in der `.gitignore`). Gemeinsame Projektagenten in `.claude/agents/` werden wie Code
  über Pull Requests geändert.
- **Lizenz:** Von einem Agenten erzeugte Änderungen stehen unter der Lizenz des jeweiligen Bereichs wie jeder andere
  Beitrag (siehe [`LICENSE.md`](LICENSE.md)). Prüfe, dass der Agent keinen fremden, anders lizenzierten Code
  eingefügt hat.

## Verhalten

Bitte halte dich an den [Verhaltenskodex](CODE_OF_CONDUCT.md).
