#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) ACC-Radar-Heizung contributors
"""Erzeugt firmware/acc_radar_heizung/ntc_table.h aus den Daten des NTC.

Der NTC liegt nach Masse, der Pull-up (R_SERIES) an derselben Versorgung wie die
ADC-Referenz (VDD). Dann gilt für den 10-Bit-ADC-Rohwert

    raw = 1023 * R_ntc / (R_ntc + R_series)

unabhängig von der Versorgungsspannung. Die Tabelle enthält den Rohwert für Temperaturen im
festen Abstand (fallend, weil der NTC-Widerstand mit der Temperatur sinkt). Die Firmware
interpoliert linear zwischen den Stützstellen und rechnet nur mit Ganzzahlen.

Anderer NTC oder anderes Teilerverhältnis: Parameter unten bzw. per Kommandozeile ändern,
Skript neu ausführen, Datei ersetzen. Es ist keine Änderung am Code nötig.

Aufruf:  python tools/gen_ntc_table.py [--r0 10000] [--b 3435] [--rseries 10000]
"""
import argparse
import math
import pathlib

ADC_MAX = 1023


def r_ntc(t_c, r0, b, t0_c=25.0):
    return r0 * math.exp(b * (1.0 / (t_c + 273.15) - 1.0 / (t0_c + 273.15)))


def raw_at(t_c, r0, b, rs):
    r = r_ntc(t_c, r0, b)
    return round(ADC_MAX * r / (r + rs))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--r0', type=float, default=10000.0, help='NTC-Widerstand bei 25 Grad in Ohm')
    ap.add_argument('--b', type=float, default=3435.0, help='B-Konstante in K')
    ap.add_argument('--rseries', type=float, default=10000.0, help='Pull-up-Widerstand in Ohm')
    ap.add_argument('--tmin', type=int, default=-40)
    ap.add_argument('--tmax', type=int, default=120)
    ap.add_argument('--step', type=int, default=5, help='Abstand der Stützstellen in Grad')
    ap.add_argument('--out', default=str(pathlib.Path(__file__).resolve().parent.parent
                                         / 'firmware' / 'acc_radar_heizung' / 'ntc_table.h'))
    a = ap.parse_args()

    temps = list(range(a.tmin, a.tmax + 1, a.step))
    raws = [raw_at(t, a.r0, a.b, a.rseries) for t in temps]
    assert all(x > y for x, y in zip(raws, raws[1:])), 'Tabelle nicht streng fallend'

    # Interpolationsfehler gegen die exakte Formel (Rohwert -> Temperatur)
    worst = 0.0
    worst_at = None
    for raw in range(raws[-1] + 1, raws[0]):
        # exakte Temperatur zum Rohwert
        r = a.rseries * raw / (ADC_MAX - raw)
        t_exact = 1.0 / (math.log(r / a.r0) / a.b + 1.0 / 298.15) - 273.15
        # Interpolation wie in der Firmware
        i = next(k for k in range(1, len(raws)) if raw >= raws[k])
        span = raws[i - 1] - raws[i]
        off = raws[i - 1] - raw
        t_tab_dc = temps[i - 1] * 10 + (off * a.step * 10) // span   # wie in der Firmware: Zehntelgrad, Ganzzahl
        err = abs(t_tab_dc / 10.0 - t_exact)
        if -20 <= t_exact <= 100 and err > worst:
            worst, worst_at = err, t_exact

    lines = []
    lines.append('// SPDX-License-Identifier: GPL-3.0-or-later')
    lines.append('// Copyright (c) ACC-Radar-Heizung contributors')
    lines.append('//')
    lines.append('// GENERIERT mit tools/gen_ntc_table.py, nicht von Hand ändern.')
    lines.append(f'// NTC: R0 = {a.r0:g} Ohm bei 25 Grad, B = {a.b:g} K, Pull-up {a.rseries:g} Ohm, ADC 10 Bit (VDD-Referenz).')
    lines.append(f'// Größter Interpolationsfehler zwischen -20 und 100 Grad: {worst:.2f} K (bei {worst_at:.0f} Grad).')
    lines.append('#pragma once')
    lines.append('')
    lines.append(f'constexpr int16_t NTC_TABLE_T_START_DC = {a.tmin * 10};   // Temperatur der ersten Stützstelle in 0,1 Grad')
    lines.append(f'constexpr int16_t NTC_TABLE_STEP_DC = {a.step * 10};      // Abstand der Stützstellen in 0,1 Grad')
    lines.append(f'constexpr uint8_t NTC_TABLE_SIZE = {len(raws)};')
    lines.append('')
    lines.append('// ADC-Rohwert je Stützstelle, fallend mit steigender Temperatur')
    lines.append('const uint16_t NTC_TABLE_RAW[NTC_TABLE_SIZE] = {')
    for k in range(0, len(raws), 8):
        lines.append('  ' + ', '.join(str(x) for x in raws[k:k + 8]) + ',')
    lines.append('};')
    pathlib.Path(a.out).write_text('\n'.join(lines) + '\n', encoding='utf-8', newline='\n')
    print(f'{a.out} geschrieben: {len(raws)} Stützstellen, größter Fehler {worst:.2f} K bei {worst_at:.0f} Grad')
    print('Offen (Rohwert) <=', raws[-1], 'heißer als', temps[-1], 'Grad; >=', raws[0], 'kälter als', temps[0], 'Grad')


if __name__ == '__main__':
    main()
