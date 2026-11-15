# General Rules — ACC-Radar-Heizung

## Git Workflow
- Conventional Commits, auf Englisch geschrieben
- Keine Versionsnummern in Dateinamen — die Git-Historie ist der
  Versionsstand (Ausnahme nur, wenn beim Setup explizit begründet,
  z. B. Hardware-Druckdateien mit Versionsstand im Namen). **Hier
  begründete Ausnahme:** Hardware-Revisionen (Platine V3.1, V3.2, ...)
  sind echte, parallel existierende Produktstände und stehen im
  Ordner-/Dateinamen unter `hardware/`
- Sprache des Repos: Deutsch (README, Doku, Issues); Commit-Messages
  bleiben Englisch (Conventional Commits)
- **Eigentümer-/Branch-Regel:** Direkt auf `main` committen dürfen nur die Maintainer
  (siehe `.github/CODEOWNERS`); alle anderen arbeiten über Fork,
  Feature-Branch und Pull Request, die Merge-Entscheidung liegt bei den
  Maintainern (Details in `.claude/agents/git-maintainer.md`)
- Der `git-maintainer`-Subagent prüft jede Änderung (Spec-Abgleich,
  Security-Check, Eigentümer-/Branch-Regel) und liefert ein Verdikt —
  er committet/pusht nicht selbst. Die Hauptsession committet gemäß
  Verdikt direkt auf `main` (Owner) oder legt bei Bedarf einen Branch
  an, committet, pusht und öffnet/aktualisiert den Pull Request (alle
  anderen)
- `CLAUDE.md` muss jederzeit den aktuellen Projektstand widerspiegeln

## Working Principle
- IMMER eine Datei lesen, bevor sie geändert wird — nie den Inhalt aus
  dem Gedächtnis annehmen
- Projektspezifische, empirisch ermittelte oder vereinbarte Werte
  (Konfiguration, Kalibrierung, Datenmodell, o. Ä.) nicht raten oder
  aus Erinnerung ändern — in den zuständigen Docs/Kommentaren nachlesen
- Nach Kontextverlust/neuer Session: `CLAUDE.md` → `README.md` →
  relevante Docs neu lesen, bevor weitergearbeitet wird
- Entscheidungen in `DECISIONS.md` festhalten, wenn sie getroffen
  werden, nicht rückwirkend

## Bilddateien / Assets

- **Jedes Bild vor dem Commit komprimieren** (z. B. squoosh.app oder
  TinyPNG, oder per Skript/Pillow) — Faustregel: einzelne Bilddatei
  nicht über ~300–500 KB. Nie einen rohen Kamera-/Tool-Export 1:1
  committen.
- Hintergrund: Das Repo soll nicht durch große Bilddateien aufgebläht
  werden; ein Force-Push zum Aufräumen wäre aufwendig.
- Bei Unsicherheit vorher fragen (Maintainer oder `git-maintainer`-
  Review), statt einfach zu committen.

## Scope Discipline
Innerhalb der zugewiesenen Aufgabe bleiben. Fällt etwas auf, das
anderswo geändert werden sollte, melden — nicht stillschweigend
außerhalb des eigenen Scopes fixen.

## Doc_Inbox (Eingangs-Postfach)
- `Doc_Inbox/` ist ein Staging-Bereich für eingehende/aktualisierte
  externe Dokumente — nie ein Ort, an dem fertiger Inhalt liegen
  bleibt.
- Git-getrackt — committet und gepusht wie alles andere, aber NUR über
  die `git-maintainer`-Review + Hauptsession-Aufteilung (inkl.
  Eigentümer-/Branch-Regel oben).
- Zwei unterschiedliche Commit-Arten:
  - **Stage-Commit**: ein neuer Drop ist gerade gelandet, noch nichts
    verarbeitet. `git-maintainer` führt nur den Secrets-Check aus,
    dann Verdikt mit Commit-Message `chore: stage new Doc_Inbox drop
    for merge` für die Hauptsession — direkt auf `main` beim Owner,
    sonst auf einem Feature-Branch mit PR.
  - **Merge-Commit**: voller Ablauf — mit aktuellem Stand abgleichen,
    alles an seinen echten Zielort verschieben, dann Verdikt für die
    Hauptsession. Die Dateiverschiebungen selbst sind keine
    Git-Operation, die kann `git-maintainer` direkt machen — nur
    Commit/Push warten auf die Hauptsession.
- Nach einem Merge muss der Ordner wieder leer sein — das schließt
  Meta-/Erklärdateien ein. Diese Leerung wird im selben Merge-Commit
  erfasst. **Projekt-Ausnahme (2026-11-15):** `Doc_Inbox/README.md`
  (erklärt Mitwirkenden den Ablauf, wird auch auf GitHub übernommen)
  und `Doc_Inbox/.gitkeep` bleiben dauerhaft liegen und zählen nicht
  als "Rest".
- **Keine großen Binär-/Bilddateien** — nur Text/Markdown-Staging.
  Braucht so etwas einen Platz im Repo, braucht es einen bewusst
  entschiedenen eigenen Ort — flaggen, nicht in `Doc_Inbox/` ablegen.

## Große Dateien (Projektregel)
- Keine 3D-Scans, STL-, OBJ-, 3MF-, ASC-Dateien oder Fusion-Quellen im
  Git — die liegen nur lokal bei den Maintainern (durchgesetzt über
  `.gitignore`). Optimierte Varianten kommen später separat auf GitHub,
  Rohdaten als Download-Link in der README.
- Gerber-Zips, EasyEDA-Exporte und Datenblätter sind klein genug und
  gehören ins Repo.
