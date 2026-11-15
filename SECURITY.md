# Sicherheit

Dieses Projekt ist ein **Entwicklungsstand ohne Zulassung** (siehe [`DISCLAIMER.md`](DISCLAIMER.md)). Hier geht
es vor allem um die **Sicherheit der Anlage**: Überhitzung, Brandgefahr, ungewollter Dauerbetrieb,
Batterieentladung und Beeinträchtigung von Assistenzsystemen. Klassische IT-Sicherheit spielt eine kleine Rolle,
das Gerät hat keine Funkschnittstelle und kein Netzwerk.

## Etwas Sicherheitsrelevantes gefunden?

- Einen **Fehler in der Sicherheitslogik oder im Leistungsteil** (zum Beispiel eine Abschaltung, die nicht
  greift, ein falscher Grenzwert, ein Bauteil im Schaltplan, das im Fehlerfall überlastet) bitte **zeitnah
  melden**.
- Nutze bevorzugt die vertrauliche Meldung über den Reiter "Security" des Repos ("Report a vulnerability"),
  wenn sie aktiviert ist. Sonst ein Issue mit dem Label `sicherheit` anlegen. Beschreibe, **was passiert**,
  **unter welchen Bedingungen** und **wie du es geprüft hast**.
- Bei akuter Gefahr (Rauch, Hitze, Geruch) das Gerät sofort von der Spannung trennen.

## Was unterstützt wird

Unterstützt wird nur der jeweils aktuelle Stand des Hauptzweigs (`main`). Es gibt keine Sicherheitsupdates für
ältere Stände und keine Gewähr. Wer Hardware baut oder weitergibt, trägt die Verantwortung dafür selbst.

## Offene bekannte Punkte

Die bekannten Schwächen stehen offen im Repo: [`docs/sicherheits-audit-firmware.md`](docs/sicherheits-audit-firmware.md)
und [`docs/hardware-analyse-v3.2.md`](docs/hardware-analyse-v3.2.md). Bitte dort zuerst nachsehen.
