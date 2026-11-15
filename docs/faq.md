# Häufige Fragen (FAQ)

Kurze Antworten mit Verweis auf die ausführlichen Dokumente. **Stand:** Pre-Alpha, die Firmware ist neu und
**nicht auf Hardware getestet**. Wo Aussagen aus dem Forum stammen, ist das gekennzeichnet; sie sind nicht
geprüft. Bitte zuerst [`DISCLAIMER.md`](../DISCLAIMER.md) lesen.

## Zulassung, Recht, Haftung

### Darf ich das einbauen? Ist das erlaubt?
**Das Projekt hat keine Zulassung und ist nicht für den öffentlichen Straßenverkehr freigegeben.** Es gibt keine
ABE und keine CE-Kennzeichnung, die EMV ist nicht gemessen. Ob ein nachgerüstetes Teil dieser Art zulässig ist,
hängt vom Einzelfall ab (Zulassungsweg, Einzelabnahme, Auswirkungen auf die Betriebserlaubnis des Fahrzeugs).
Im Forum wird das kontrovers diskutiert, geklärt ist es nicht
([`hintergrund-forum.md`](hintergrund-forum.md), Abschnitt Rechtliches). Wer es trotzdem baut, tut das **auf eigene
Gefahr** und trägt die Verantwortung selbst. Das ist keine Rechtsberatung.

### Kann ich das kaufen?
**Derzeit nicht.** Es werden keine Platinen, Bausätze oder Geräte verkauft oder abgegeben. Ob und in welcher Form
das später möglich ist (zum Beispiel unbestückte Platinen), hängt von Tests und rechtlichen Fragen ab. Sollte sich
das ändern, wird es im README angekündigt. Wer Hardware abgibt, trägt die Herstellerverantwortung. Siehe
[`DISCLAIMER.md`](../DISCLAIMER.md).

### Wer haftet, wenn etwas passiert?
Wer Hardware baut, einbaut oder weitergibt, trägt die Verantwortung dafür selbst. Die Anlage schaltet eine Heizung am
Bordnetz; Fehler können zu Überhitzung, Brand, einer leeren Batterie oder einer Beeinträchtigung von Assistenzsystemen
führen. Die Autoren und Mitwirkenden übernehmen keine Haftung, soweit gesetzlich zulässig.

## Stand und Funktion

### Funktioniert das schon?
**Teilweise und ungeprüft.** Die Platine V3.2 ist gebaut und hat laut Projektinhaber mit einem früheren
Firmware-Stand im Grundsatz funktioniert. Die **aktuelle Firmware** (Zustandsautomat, Fehlerspeicher,
Debug-Ausgang) baut ohne Warnungen und ist per Modell und Simulation geprüft, aber **nicht am Gerät getestet**.
Es gibt **keine dokumentierten Fahrzeugergebnisse**. Der Test am Prüfstand steht aus
([`pruefstand.md`](pruefstand.md)).

### Wie funktioniert die Heizung?
Ein Luftstrom (kleiner Lüfter und ein PTC-Heizelement, bis ca. 40 W und unter 7 A) erwärmt das Emblem von hinten.
Ein ATtiny826 regelt das: Er heizt nur, wenn der Motor läuft (aus der Bordspannung erkannt), die Luft kalt und feucht
ist und alle Sensoren plausibel sind. Der Lüfter läuft vor und nach der Heizung. Das Soll-Verhalten steht in
[`verhalten.md`](verhalten.md).

### Greift das in mein Auto ein?
**Nein.** Das Gerät braucht nur die Spannungsversorgung. Es hört nicht am CAN-Bus mit und sendet nichts an das
Fahrzeug. Der Ansatz stammt aus dem Forenthread: Eingriffe ins Bordnetz-Kommunikationssystem sollen vermieden
werden.

### Warum nicht einfach einen Heizdraht oder eine Heizfolie auf das Emblem?
Weil **Metall vor der Radarantenne** die Radarwellen stören kann. Im Forum wird das für Heizdrähte und Heizfolien
als wahrscheinliches Problem genannt. Deshalb arbeitet dieses Projekt mit Luft und ohne Metall im Strahlengang.
Ob das Material von Düse und Emblem selbst unkritisch ist, ist ungeprüft.

### Warum meldet das Auto "keine Sicht", obwohl das Logo noch erkennbar ist?
Laut Forum genügt schon eine **dünne Schneekruste oder Schneeregen** auf dem Emblem. Die Forumsberichte nennen
besonders **Regen und Schneeregen bei Temperaturen um und unter etwa 8 °C**. Auch Schnee, Eis und Wasser könnten die
Radarwellen beeinflussen; die genaue Ursache ist im Thread nicht geklärt
([`hintergrund-forum.md`](hintergrund-forum.md)).

### Fällt dabei auch der normale Tempomat aus?
Laut Forumsberichten **ja**: Bei der Meldung "keine Sicht" fällt neben ACC und Front Assist auch der normale
Tempomat (GRA) aus. In einzelnen Berichten auch der Spurhalteassistent. Das gilt als Erfahrung einzelner Nutzer,
nicht als Herstellerangabe.

## Fahrzeug

### Funktioniert das an meinem Fahrzeug?
Das **Referenzfahrzeug ist der VW Golf 7 Facelift**, bei dem der Radar hinter dem Logo sitzt. Getestet ist **kein**
Fahrzeug. Kompatibilität wird nur für Fahrzeuge behauptet, die tatsächlich getestet wurden.

- **Vor-Facelift:** Der Sensor sitzt im Kühlergrill unter dem Kennzeichenhalter, dieser Fall ist nicht
  abgedeckt (Forum: Probleme meist nur bei starkem Schneetreiben).
- **Andere Modelle:** Die Elektronik ist fahrzeugunabhängig gedacht, die Mechanik (Düse, Halterung) nicht. Aus dem
  Forum werden ähnliche Probleme bei anderen Fahrzeugen genannt (VW Tiguan, Seat Ateca, Skoda Kodiaq, Passat B8,
  Mercedes Distronic). Wer ein anderes Modell hat, kann mit Messungen und Konstruktion helfen
  ([`CONTRIBUTING.md`](../CONTRIBUTING.md)).

### Hat das Facelift nicht schon eine Sensorheizung?
Im Forum wird berichtet, dass der Sensor selbst eine Heizung haben soll, das **Logo davor aber nicht** beheizt ist,
und die Sensorheizung im ACC-Steuergerät sogar **deaktiviert** sein könnte. Das ist **unbelegt**: Eine Frage,
ob sich das per Diagnosetool ändern lässt, blieb im Thread unbeantwortet. Ein Eingriff in das Steuergerät ist **nicht**
Teil dieses Projekts.

### Hilft Wachs, Imprägnierung oder eine Nano-Versiegelung auf dem Emblem?
Im Forum berichtet ein Nutzer, eine Versiegelung habe **eine Woche lang** die Schneehaftung verringert. Das ist
eine einzelne, kurze Beobachtung und nicht belegt. Es wäre eine **günstige, testbare Ergänzung**, ersetzt aber keine
Heizung.

### Und eine Tönungsfolie?
Eine Folie könnte die Sicht oder das Radarsignal beeinträchtigen. Ohne Test würde ich sie nicht aufbringen. Im Forum
wird beides genannt: als Idee und als mögliches Problem.

### Hilft Codieren (VCDS) oder ein Software-Eingriff?
Dazu gibt es keine belegten Erfahrungen. Das Projekt greift bewusst **nicht** in die Fahrzeugsoftware ein.

## Nachbau

### Was brauche ich zum Nachbauen?
- Die **Platine** (4 Lagen, Gerber-Dateien in [`hardware/V3.2/`](../hardware/V3.2/)) fertigen lassen und die
  Bauteile nach der Stückliste ([`BOM_V3.2.csv`](../hardware/V3.2/BOM_V3.2.csv), mit LCSC-Nummern) beschaffen. Der
  verwendete 10-kΩ-NTC ist bei LCSC nicht mehr gelistet, ein Ersatz muss geprüft werden.
- **SMD-Löten** (u. a. ATtiny826 im QFN-20, Bauteile der Größe 0603), ein **UPDI-Programmer** zum Flashen.
- Für den Test ein Labornetzteil, einen Lastwiderstand und ein Multimeter ([`pruefstand.md`](pruefstand.md)).
- **Heizelement, Lüfter, Düse und Gehäuse:** Düse und Gehäuse sind noch in Arbeit
  (Aufgabe: [Issue #32](https://github.com/sandboop/ACC-Radar-Heizung/issues/32)).

**Wichtig:** Bitte nicht einbauen, bevor der Prüfstandtest bestanden ist. Das Gerät ist nicht einbaufertig.

### Was kostet das?
Es gibt noch **keine belastbare Kostenübersicht**. Die Stückliste nennt die Bauteile, Preise sind nicht erfasst.
Eine Kostenauswertung wäre eine gute Mitmach-Aufgabe.

### Wo bekomme ich die Platine her?
Aus den Gerber-Dateien selbst bei einem Platinenfertiger bestellen. Das Projekt verkauft nichts (siehe oben). Eine
gemeinsame Bestellung durch Dritte aus der Community ist denkbar, aber nicht Teil des Projekts.

## Sicherheit und Betrieb

### Was passiert bei einem Fehler?
Die Heizung wird **sofort abgeschaltet**, ein laufender Lüfter läuft noch 30 Sekunden nach, die Fehler-LED blinkt
den Fehlercode (1 bis 5), und der Fehler wird im EEPROM gespeichert. Abgesichert sind Sensorfehler (NTC offen oder
Kurzschluss, Luftsensor), Übertemperatur und ein Hänger des Controllers (Watchdog). Details:
[`../firmware/README.md`](../firmware/README.md) und [`sicherheits-audit-firmware.md`](sicherheits-audit-firmware.md).

### Welche Schwächen sind bekannt?
Das Sicherheits-Audit nennt sie offen mit Schweregrad, zum Beispiel: keine Funktionsüberwachung des Heizelements,
nicht abgesicherte Zuleitung (**keine Sicherung, kein Überspannungs- und Verpolschutz auf der Platine V3.2**),
ungenaue Erkennung "Motor läuft" durch die Toleranz des Netzteil-Enable. Lies bitte
[`sicherheits-audit-firmware.md`](sicherheits-audit-firmware.md) und
[`hardware-analyse-v3.2.md`](hardware-analyse-v3.2.md), bevor du etwas baust.

### Zieht das meine Batterie leer?
**Das soll es nicht, ist aber ungeprüft.** Die Heizung läuft nur bei erkanntem laufendem Motor (Bordspannung
etwa 13,5 V oder mehr). Bei Motor aus schaltet sich der Controller nach dem Nachlauf und 20 Sekunden selbst ab. Der
Ruhestrom im abgeschalteten Zustand ist nur **berechnet** (ca. 10 µA), nicht gemessen. Die Einschaltschwelle des
Netzteils streut bauteilbedingt stark (11,6 bis 15,3 V); im ungünstigen Fall kann das Gerät schon bei der
Ruhespannung anlaufen. Das ist ein offener Punkt für die nächste Platine
([Issue #17](https://github.com/sandboop/ACC-Radar-Heizung/issues/17)).

### Warum 8 °C und 80 % Feuchte?
Das sind Startwerte: Die Forumsberichte nennen Regen unter etwa 8 °C, die Feuchteschwelle war im alten Code ein
Testwert. Ob die Werte stimmen, muss der Fahrzeugtest zeigen. Sie stehen in `firmware/acc_radar_heizung/config.h`
und lassen sich ändern.

## Mitmachen

### Kann ich mitmachen, auch ohne viel Erfahrung?
**Ja.** Es gibt Aufgaben für Einsteiger (Label "good first issue") und Tabellen, wo Hilfe gesucht wird. Siehe
[`../README.md`](../README.md) und [`../CONTRIBUTING.md`](../CONTRIBUTING.md).

### Darf ich dafür KI-Coding-Agenten nutzen?
**Ja, ausdrücklich erwünscht.** Das Projekt wurde selbst mit einem Agenten aufgearbeitet. Die Regeln (Verantwortung,
Kennzeichnung im Pull Request, Sorgfalt bei Sicherheitslogik) stehen in [`../CONTRIBUTING.md`](../CONTRIBUTING.md),
tool-neutral in [`../AGENTS.md`](../AGENTS.md).

### Wo kann ich Fragen stellen oder Ergebnisse melden?
Als Issue auf GitHub (Vorlagen für Fehler, Ideen und Prüfstandberichte) oder im [Forenthread](https://www.golf7freunde.de/index.php/Thread/20027-Update-zur-Logo-Radarheizung-Das-Projekt-ist-jetzt-Open-Source-auf-GitHub-und-ic/). Sicherheitsrelevante
Funde bitte nach [`../SECURITY.md`](../SECURITY.md) melden.
