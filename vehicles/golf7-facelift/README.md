# Golf 7 Facelift (Referenzfahrzeug)

Beim Facelift sitzt der Radarsensor (ACC / Front Assist) **hinter dem
VW-Logo** in der Front. Das Logo ist nicht beheizt, der Sensor dahinter
nur eingeschränkt. Bei Schnee und Eis fällt die ACC-Funktion aus.
Beim Vor-Facelift sitzt der Sensor im Kühlergrill unter dem
Kennzeichenhalter; dieser Fall ist hier nicht abgedeckt.

Ziel der Heizung: das Logo warm bekommen, **ohne metallische Teile vor
die Radarantenne zu setzen**. Deshalb wird mit einem Luftstrom über ein
Heizelement gearbeitet.

Hintergrund und Erfahrungen aus dem Forum: [`../../docs/hintergrund-forum.md`](../../docs/hintergrund-forum.md).

## Scan- und CAD-Daten (für alle, die konstruieren wollen)

Der Projektinhaber hat einen 3D-Scan der Front und erste Konstruktionsstände. Die Dateien sind **sehr groß** und
liegen deshalb nicht im Git (siehe `.gitignore`):

- **3D-Scan der Front** mit Emblem und Radarsensor, vorn und hinten (Einstar): STL, OBJ, 3MF und Punktwolken. Zusammen
  ca. 3,7 GB, einzelne Dateien 40 bis 550 MB. Teilscans: Radar, Emblem (Vorder- und Rückseite), Haubenverschluss.
- **Düsenkonstruktion** (Fusion-Projekt, Stände v11 und v12, DWG/DXF) und eine einfache Strömungsabschätzung.
- **3D-Druckdateien des Prototyps V1**: Gehäuse Heizelement, Platinengehäuse und -deckel, DHT11-Gehäuse,
  Einlass-Befestigung, mit Druckparametern (Bambu Lab).

**Bereitstellung:** Reduzierte Varianten (nur Emblem und Radarbereich) kommen als Release-Anhang hier auf GitHub, die
Rohdaten per Download-Link. Wer daran arbeiten möchte, meldet sich bitte in [Issue #31](https://github.com/sandboop/ACC-Radar-Heizung/issues/31) oder im Forum:
Dann werden die Dateien als Erstes bereitgestellt. Die zugehörige Aufgabe zum Konstruieren steht in
[Issue #32](https://github.com/sandboop/ACC-Radar-Heizung/issues/32).

## Offene Punkte

- Maße und Toleranzen aus dem Scan dokumentieren.
- Wirkung des Luftstroms (Strömungssimulation) und Material des Logos
  bzw. der Düse hinsichtlich Radardurchlässigkeit: ungeprüft.
