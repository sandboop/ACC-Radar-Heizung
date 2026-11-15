# Security Rules — ACC-Radar-Heizung

## Secrets
- NIEMALS Secrets (API-Keys, Tokens, Zugangsdaten, Passwörter) ins
  Repo committen
- Nur `.env.example` (mit Platzhalterwerten) wird committet, falls
  zutreffend
- Die echte `.env`/Konfiguration mit echten Werten liegt in
  `.gitignore`, wird manuell auf dem Zielsystem gepflegt
- **Betrifft:** `arduino_secrets.h` (von der Arduino-IDE/Cloud
  automatisch in jedem Sketch-Ordner unter `_/` angelegt, gitignored),
  GitHub-Tokens und -Passwörter, persönliche Daten in
  EasyEDA-Exporten (Account-/Projektnamen, E-Mail, Bestelldaten) und
  in Fahrzeug-Logs (z. B. FIN/Kennzeichen, falls je aufgezeichnet).
  Aktuell ist keine WLAN-/Cloud-Funktion in der Firmware aktiv.

## Pre-Commit Check (durchgesetzt von git-maintainer)
Enthält ein Diff etwas, das wie ein Secret aussieht (Token, Key,
Passwort-Muster), STOPPEN und melden, bevor committet wird — keine
Ausnahmen, auch nicht bei scheinbaren Platzhaltern oder "nur fürs
lokale Testen".

## Sicherheit der Anlage (Safety, nicht nur Secrets)
Dieses Projekt schaltet eine Heizung am 12-V-Bordnetz. Änderungen an
Abschaltlogik, Grenzwerten oder Leistungsteil laufen immer zusätzlich
über `safety-reviewer`. Siehe `DISCLAIMER.md`.
