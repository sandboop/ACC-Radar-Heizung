# Scan- und Konstruktionsdaten (Golf 7 Facelift)

Daten für alle, die **Düse und Gehäuse** konstruieren wollen (Aufgabe: [Issue #32](https://github.com/sandboop/ACC-Radar-Heizung/issues/32)).
Download als GitHub-Release: [**scan-daten-v1**](https://github.com/sandboop/ACC-Radar-Heizung/releases/tag/scan-daten-v1).
Die Dateien sind groß und liegen deshalb nicht im Git.

## Fahrzeug und Herkunft

| | |
|---|---|
| Fahrzeug | VW Golf 7 R Variant (Facelift), HSN/TSN 0603/BRM, Baujahr 12/2019 |
| Scanner | Einstar (Handscanner) |
| Scan-Einstellungen, Scan-Datum | nicht mehr bekannt, ggf. in der Projektdatenbank der Rohdaten abrufbar |
| Genauigkeit | unbekannt, **Maße vor dem Bauen am eigenen Fahrzeug nachmessen** |

## Datei 1: `Radar-3D-Scan.stl`

Ausschnitt des Scans, der zum Konstruieren benutzt wurde.

| Eigenschaft | Wert |
|---|---|
| Format | binäres STL, 1.765.197 Dreiecke, 84 MB |
| **Einheit** | **Millimeter, Maßstab 1:1** |
| Ausdehnung | 251,4 × 231,8 × 237,0 mm (X × Y × Z) |
| Lage | X und Y um den Nullpunkt zentriert (X ±125,7 mm, Y ±115,9 mm), Z von −68,5 bis +168,5 mm |
| **Fahrtrichtung** | **−Y** (die Front mit der Emblem-Schale liegt bei den kleinen Y-Werten) |
| **Oben** | **+Z** (der Schlossträger liegt über dem Radar) |
| Zustand | **nicht repariert**: Löcher im Scan, Netz nicht als wasserdicht geprüft |
| SHA-256 | `711e7d23d5b225821b0035f33e059ecd8522600cae32a55332dbff888f08504d` |

**Zu sehen sind:** das Radarmodul (ca. 70 bis 75 mm breit), die Schale des Emblems (ca. 150 mm Durchmesser), der
Schlossträger, ein Teil des Motorkühlers und der Haubenverschluss.

### Import in Fusion (Autodesk Fusion 360)

1. **Einfügen → Netz einfügen** (Insert Mesh), die Datei wählen.
2. Als Einheit **mm** einstellen. Kontrolle: Das Radarmodul ist ca. 70 bis 75 mm breit.
3. Bei Bedarf im Netz-Arbeitsbereich **Reduzieren** (Reduce) und **Ebenenschnitt** (Plane Cut) nutzen, um die Datei zu
   verkleinern oder auf den Bereich um das Emblem zuzuschneiden.

Andere Programme (Blender, MeshLab, FreeCAD, ...) lesen STL ebenfalls; dort die Einheit auf mm prüfen. Eine
STL-Datei enthält selbst keine Einheitenangabe.

## Datei 2: `prototyp-v1-druckdateien.zip`

STL-Dateien des Prototyps V1 und die Druckparameter (Bambu Lab, 06.04.2025), ein früher Konstruktionsstand, **nicht im
Fahrzeug erprobt**:

- Gehäuse Heizelement, Verschluss Heizelement
- Platinengehäuse und -deckel
- DHT11-Gehäuse
- Einlass-Befestigung
- Schraube
- Druckparameter und Gesamtdruckdatei als 3MF

SHA-256 der ZIP-Datei: `e92aa2b70dffe201282637909f36c9ea6ffdbb5b692aeb3b4f7b03a987d0b2cb`

## Weitere Daten (auf Anfrage)

Auf Anfrage gibt es weitere, größere Dateien: den Gesamtscan der Front, die Punktwolken, Teil-Scans (Radar,
Emblem, Haubenverschluss) und die Fusion-Konstruktion der Düse (Stände v11 und v12, DWG/DXF). **Achtung:** Die
älteren Teil-Scans und der Gesamtscan liegen in **Metern** vor (der Gesamtscan der Front ist 1,5 Einheiten breit), die
Datei `Radar-3D-Scan.stl` in Millimetern. Ein Download-Link für die Rohdaten folgt.

## Hinweise zu Rechten

- Das VW-Emblem und die gescannten Teile sind Marken bzw. Konstruktionen von Volkswagen. Die Daten dienen nur der
  Konstruktion einer Nachrüstung; Marken gehören ihren Inhabern, das Projekt ist nicht mit dem Hersteller verbunden.
- Für den Anteil des Projekts gilt CC-BY-SA-4.0 (siehe [`../../LICENSE.md`](../../LICENSE.md)), **ohne Gewähr**
  ([`../../DISCLAIMER.md`](../../DISCLAIMER.md)).

## Wenn du etwas findest

Maße, die nicht stimmen, Lücken im Scan, die stören, oder eine reparierte oder reduzierte Fassung, die du teilen
möchtest: bitte als Issue oder Pull Request melden. Eine **wasserdichte, reparierte Variante** wäre eine sehr
willkommene Mitmach-Aufgabe.
