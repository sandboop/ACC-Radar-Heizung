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

## Dateien, die nur lokal beim Owner liegen

Aus Größen- und Platzgründen liegen die folgenden Daten **nicht** auf
dem Git-Repo (siehe `.gitignore`):

- 3D-Scan der Front mit Radar und Emblem (Einstar: Punktwolken, STL,
  OBJ, 3MF; ca. 3,7 GB)
- Konstruktion der Düse (Fusion-Projekte, DWG/DXF)
- 3D-Druckdateien des Prototyps V1 (Gehäuse Heizelement, Platinengehäuse
  und -deckel, DHT11-Gehäuse, Einlassbefestigung, Verschluss, Haube)
  samt Druckparametern (Bambu Lab, 06.04.2025)

Geplant: optimierte Varianten später auf GitHub, Rohdaten als
Download-Link.

## Offene Punkte

- Maße und Toleranzen aus dem Scan dokumentieren.
- Wirkung des Luftstroms (Strömungssimulation) und Material des Logos
  bzw. der Düse hinsichtlich Radardurchlässigkeit: ungeprüft.
