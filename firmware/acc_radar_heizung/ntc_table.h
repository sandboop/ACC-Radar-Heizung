// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors
//
// GENERIERT mit tools/gen_ntc_table.py, nicht von Hand ändern.
// NTC: R0 = 10000 Ohm bei 25 Grad, B = 3435 K, Pull-up 10000 Ohm, ADC 10 Bit (VDD-Referenz).
// Größter Interpolationsfehler zwischen -20 und 100 Grad: 0.19 K (bei -13 Grad).
#pragma once

constexpr int16_t NTC_TABLE_T_START_DC = -400;   // Temperatur der ersten Stützstelle in 0,1 Grad
constexpr int16_t NTC_TABLE_STEP_DC = 50;      // Abstand der Stützstellen in 0,1 Grad
constexpr uint8_t NTC_TABLE_SIZE = 33;

// ADC-Rohwert je Stützstelle, fallend mit steigender Temperatur
const uint16_t NTC_TABLE_RAW[NTC_TABLE_SIZE] = {
  983, 970, 953, 932, 906, 876, 841, 802,
  759, 712, 663, 612, 562, 512, 463, 417,
  374, 334, 298, 265, 235, 208, 185, 164,
  146, 130, 115, 103, 92, 82, 74, 66,
  60,
};
