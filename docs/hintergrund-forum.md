# Hintergrund: Erfahrungen aus dem Forum

Zusammenfassung der Informationen aus dem Forenthread **"ACC/Front Assist & Schnee(-regen)"** auf
golf7freunde.de ([Link zum Thread](https://www.golf7freunde.de/index.php/Thread/7935-ACC-Front-Assist-Schnee-regen/)),
3 Seiten, 45 Beiträge aus Dezember 2017 und April 2024. Damit niemand zwischen den Quellen hin- und herspringen
muss, steht hier das Wesentliche in eigenen Worten; Nutzernamen sind weggelassen.

> **Status der Quellen:** Das sind Erfahrungen und Vermutungen einzelner Forenmitglieder, **nicht geprüft**.
> Die Zusammenfassung wurde aus den Seiten erstellt und sollte bei wichtigen Punkten gegen den Thread
> geprüft werden. Beitragsnummern (#) beziehen sich auf den Thread.

## 1. Wie sich das Problem zeigt

- Bei leichtem Schneeregen oder einer dünnen Schneeschicht meldet das Kombiinstrument, dass ACC und
  Front Assist nicht verfügbar sind, weil der Sensor "keine Sicht" hat (#1, #23, #27).
- Dabei fällt auch der normale Tempomat (GRA) aus (#1, #4, #23). Beschrieben wird zudem, dass der
  Spurhalteassistent bei seitlich liegendem Schnee ausfällt (#8).
- Auf dem VW-Logo liegt dabei meist nur eine dünne Schneekruste, das Zeichen ist noch erkennbar (#1).
  Mehrfach wird berichtet, dass Schnee auf dem Emblem haftet und den Sensor dahinter blockiert (#24, #26).
- Einmal wird von dreimal "keine Sicht" innerhalb von fünf Minuten bei Schneeregen berichtet, danach blieb
  die Warnleuchte an (#27).
- Das Problem besteht auch in späteren Wintern fort (#37, April 2024), dort besonders bei **Regen unter
  etwa 8 °C**.
- Ähnliche Meldungen werden von anderen Fahrzeugen genannt (VW Tiguan, Seat Ateca, Skoda Kodiaq,
  Passat B8, Mercedes Distronic; #19). Das spricht dafür, dass die Heizung **nicht golfspezifisch** sein
  muss.

## 2. Wo der Sensor sitzt und wie er funktioniert

- **Facelift:** Der Radarsensor sitzt **hinter dem VW-Logo** an der Front (#9, #26, #37).
- **Vor dem Facelift:** Der Sensor sitzt im Kühlergrill unter dem Kennzeichenhalter. Dort gab es laut
  mehreren Berichten nur bei starkem Schneetreiben Probleme (#5, #17, #37).
- **Sensorheizung:** Nach einem Beitrag soll ein Stromlaufplan für das Facelift eine Heizung des Sensors zeigen
  (#9); ein anderer berichtet, sie sei im ACC-Steuergerät deaktiviert (#21). Beobachtet wurde keine
  spürbare Wärme am Sensor (#2, #4). Eine Frage, ob sich die Heizung per Diagnosetool aktivieren lässt,
  blieb unbeantwortet (#22). Das ist **unbelegt**.
- **Frequenz:** Ein Beitrag nennt 24 GHz bei älteren Modellen und 77 bis 79 GHz aktuell, mit etwa 10 mW
  Sendeleistung (#20). Das ist eine Forenangabe, keine geprüfte Quelle.
- **Nicht beheizt:** Das Logo selbst hat keine Heizung (#9).

## 3. Ideen und Erfahrungen aus dem Thread

| Idee oder Erfahrung | Aussage im Forum | Bedeutung für dieses Projekt |
|---|---|---|
| Schnee von Hand entfernen | hilft kurzfristig (#23) | Ausgangslage, keine Lösung |
| Heizdraht oder Heizfolie am Emblem | wird als störend für die Radarwellen eingeschätzt (#28, #30) | Grund für den Ansatz **ohne Metall vor der Antenne** (Luftstrom) |
| Warmluft zum Emblem leiten | als Idee genannt (#28) | entspricht dem Ansatz dieses Projekts (Lüfter, Heizelement, Düse) |
| Kleine Glühlampe hinter dem Emblem | Vorschlag, keine Erfahrung (#31) | nicht verfolgt (Leistung und Wärmeverteilung unklar) |
| Wachs oder Nano-Versiegelung auf dem Emblem | eine Versiegelung soll eine Woche lang Schneehaftung verhindert haben (#35); Schlussfolgerung "hat funktioniert" ist bei einer Woche Beobachtung nicht belegt (#36) | **ergänzend testbar**: senkt die Haftung, ersetzt aber keine Heizung |
| Tönungsfolie oder Folierung | könnte helfen, wenn die Sicht des Sensors nicht leidet (#41); Folien können die Radardurchlässigkeit stören (#30) | Vorsicht, nicht ohne Test |
| Winter ohne VW-Emblem fahren | Vorschlag (#30) | keine Lösung für den Alltag |
| Nachrüstbare beheizte Kugel eines Anbieters | als Option erwähnt (#27) | zum Vergleich, nicht geprüft |
| Logoheizung als Eigenbau | Prototyp, geregelt, bis 40 W, schaltet selbständig, ohne CAN-Eingriff (#37, #42, #44) | **dieses Projekt** |
| PTC-Heizelement als einfache Steuerung | Vorschlag (#43), im Projekt verwendet (#44) | Stand der Hardware V3.x |

## 4. Warum das Eis haftet (Vermutungen)

Im Thread wird diskutiert, ob Schnee, Eis und Wasser die Radarwellen dämpfen und ob das Logo aus einem Material
besteht, auf dem Eis anders haftet als auf Wasser (#10, #11, #12). Ein Beitrag hält fest, dass Radarprobleme von
Frequenz und Bauteilen abhängen und sich nicht pauschal erklären lassen (#20). Die Abschaltung wird teils als
Absicherung gegen Fehlbremsungen verstanden (#14, Vermutung). **Aussagen von VW oder Werkstätten** (Kulanz,
Reparatur, offizielle Lösung) gibt es im Thread nicht.

## 5. Rechtliches (Hinweise aus dem Thread)

- Jedes fest verbaute Bauteil brauche eine Zulassung; für Fahrzeugteile gebe es keine CE-Konformitätserklärung
  im üblichen Sinn (#38). Das wird im Thread bestritten (#40), die Frage bleibt offen.
- Genannt werden CE-Kennzeichnung, ABE und Einzelabnahme als mögliche Wege (#37, #43). Dieses Projekt wird
  daher ausdrücklich als **nicht zugelassenes Entwicklungsprojekt** geführt, siehe
  [`../DISCLAIMER.md`](../DISCLAIMER.md). Die EMV ist noch nicht gemessen (#44).
- Der Ansatz, **nur die Spannungsversorgung** zu nutzen und nicht in die Fahrzeugkommunikation einzugreifen,
  stammt aus dem Thread (#42, #44).

## 6. Was daraus für das Projekt folgt

1. **Kein Metall vor der Antenne** bleibt der Kern der Anforderung (Heizfolien und Drähte gelten als riskant).
2. Der Auslöser "**Regen oder Feuchte bei Temperaturen um und unter 8 °C**" passt zu den Berichten; ob 8 °C die
   richtige Schwelle ist, gehört in den Fahrzeugtest ([`verhalten.md`](verhalten.md)).
3. Ein **Test mit Versiegelung** (Wachs, Nano-Versiegelung) des Emblems ist eine mögliche, günstige Ergänzung.
4. Die Heizung sollte **fahrzeugübergreifend** denkbar bleiben, die Mechanik aber je Fahrzeug getrennt
   behandelt werden ([`../vehicles/`](../vehicles/)).
5. Ob die **werkseitige Sensorheizung** vorhanden und im Steuergerät deaktiviert ist, ist eine offene,
   unbelegte Frage. Ein Eingriff in das Steuergerät ist nicht Teil dieses Projekts.

## 7. Offene Fragen

- Welche Sensorgeneration (Hersteller, Frequenz) verbaut ist, und wie die Meldung bei den Fahrzeugen genau
  lautet (Wortlaut für die Dokumentation).
- Temperatur und Feuchte, bei denen die Meldung typischerweise kommt (Messwerte aus dem Fahrzeug).
- Wie stark Folie oder Versiegelung das Radarsignal tatsächlich beeinflussen.
