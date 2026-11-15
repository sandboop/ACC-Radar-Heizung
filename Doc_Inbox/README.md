# Doc_Inbox

Eingangs-Postfach für neue oder aktualisierte Dokumente (Notizen,
Handoff-Dateien, Entwürfe, Messprotokolle als Text), die noch nicht an
ihrem endgültigen Platz im Repo liegen.

## So funktioniert es

1. **Ablegen:** Neue Datei hier hineinlegen und committen (bei
   Mitwirkenden über Branch + Pull Request). Das ist der
   *Stage-Commit*, noch ist nichts verarbeitet.
2. **Einsortieren:** Ein Mensch oder ein Coding-Agent gleicht den Inhalt
   mit dem aktuellen Stand ab und verschiebt ihn an seinen echten
   Zielort (z. B. `hardware/`, `firmware/`, `vehicles/`, `docs/`) oder
   übernimmt die Inhalte in bestehende Dokumente. Das ist der
   *Merge-Commit*.
3. **Leeren:** Danach ist der Ordner wieder leer. Hier bleibt nichts
   liegen, nur diese `README.md` und die `.gitkeep`.

## Regeln

- Nur Text/Markdown, **keine** großen Binär- oder Bilddateien (keine
  Scans, STL, 3MF, Fotos in Originalgröße).
- Keine Secrets, Zugangsdaten oder persönlichen Daten.
- Kein Ort für fertigen Inhalt: Was hier liegt, ist "noch zu
  verarbeiten".
- Braucht eine Datei einen dauerhaften Platz, bitte nachfragen, statt
  sie hier liegen zu lassen.
