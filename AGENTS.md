# AGENTS.md — ACC-Radar-Heizung

Tool-neutrale Kurzfassung für Coding-Agenten und Mitwirkende (Claude
Code liest zusätzlich `CLAUDE.md` und `.claude/`).

## Worum es geht
Autonome Heizung für das VW-Logo über dem ACC-/Front-Assist-Radar:
Ein ATtiny826 regelt über DHT11 (Luft), zwei NTC (Heizelement,
Logo-Platte) und Bordspannungsmessung ein PTC-Heizelement mit Lüfter.
Kein CAN-Bus, nur Spannungsversorgung. Siehe `README.md` und
`DISCLAIMER.md`.

## Regeln
- Sprache: Deutsch (Doku/Issues); Commit-Messages Englisch
  (Conventional Commits).
- Vor jeder Änderung die Datei lesen; Schwellwerte und Pinbelegung nicht
  raten, in Doku/Kommentaren nachlesen.
- Sicherheitslogik (Abschaltungen, Grenzwerte) und Leistungsteil nur
  mit besonderer Sorgfalt ändern, begründen und im PR hervorheben.
- Keine Secrets, keine `arduino_secrets.h`, keine großen Binärdateien
  (3D-Scans, STL, 3MF) committen.
- Änderungen von Mitwirkenden über Branch + Pull Request.

## Eigene Agenten
Persönliche Agenten/Prompts bitte lokal halten (`~/.claude/agents/`
bzw. `.claude/agents/local/`, gitignored). Geteilte Projektagenten
liegen in `.claude/agents/` und werden über PR geändert.
