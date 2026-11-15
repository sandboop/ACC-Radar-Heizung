// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) ACC-Radar-Heizung contributors

/*
  Luftsensor-Schicht: liefert in.t_air_dc, in.rh_air_d10, in.air_valid und in.air_errors.
  Der Rest der Firmware kennt nur diese Werte, nicht den Sensortyp. Ein späterer Sensor
  (z. B. I2C, Platine V4) ersetzt nur diese Datei.
*/

#if AIR_DRIVER == AIR_DRIVER_LIBRARY

DHT dht(PIN_AIR_SENSOR, DHT11);

void air_init()
{
  dht.begin();                         // One-Wire-Bus initialisieren
}

// Liest den Sensor; gibt true zurück, wenn gültige Werte vorliegen.
static bool air_read(int16_t &t_dc, int16_t &rh_d10)
{
  const float h = dht.readHumidity();
  const float t = dht.readTemperature();
  if(isnan(h) || isnan(t))
  {return false;}

  t_dc = (int16_t)lroundf(t * 10.0f);
  rh_d10 = (int16_t)lroundf(h * 10.0f);
  return true;
}

#else   // AIR_DRIVER_OWN: eigener DHT11-Treiber, direkter Portzugriff

/*
  Protokoll des DHT11 (Datenblatt): Der Controller zieht die Leitung mindestens 18 ms nach Masse und
  lässt sie los (externer Pull-up R7). Der Sensor antwortet mit 80 us Low und 80 us High und sendet
  dann 40 Bits: je ein Low-Impuls von ca. 50 us, danach ein High-Impuls von ca. 27 us (Bit 0) oder
  ca. 70 us (Bit 1). Aufbau der 5 Bytes: Feuchte (ganz, Nachkomma), Temperatur (ganz, Nachkomma),
  Prüfsumme als Summe der ersten vier Bytes.

  Die Impulslängen werden als Schleifendurchläufe gemessen. Ein Bit ist 1, wenn der High-Impuls länger
  als der Low-Impuls davor ist; dadurch ist das Verfahren unabhängig vom Takt. Während der
  Bitmessung (ca. 4 ms) sind die Interrupts gesperrt, millis() geht dabei um wenige ms nach.
*/
constexpr uint16_t AIR_TIMEOUT_LOOPS = (uint16_t)(F_CPU / 6000UL);   // ca. 1 ms

// Zählt die Schleifendurchläufe, solange die Leitung auf "level" steht. 0 = Zeitüberschreitung oder
// Leitung war nicht auf "level".
static uint16_t air_expect_pulse(bool level)
{
  uint16_t count = 0;
  while(((AIR_VPORT.IN & AIR_BM) != 0) == level)
  {
    if(++count >= AIR_TIMEOUT_LOOPS)
    {return 0;}
  }
  return count;
}

void air_init()
{
  AIR_VPORT.DIR &= ~AIR_BM;            // Eingang, Pull-up extern (R7)
}

static bool air_read(int16_t &t_dc, int16_t &rh_d10)
{
  uint8_t data[5] = {0, 0, 0, 0, 0};

  AIR_VPORT.OUT &= ~AIR_BM;            // Startsignal: Leitung nach Masse ...
  AIR_VPORT.DIR |= AIR_BM;
  delay(20);                           // ... mindestens 18 ms
  AIR_VPORT.DIR &= ~AIR_BM;            // loslassen

  bool ok = true;
  const uint8_t sreg = SREG;
  cli();
  delayMicroseconds(55);
  if((air_expect_pulse(false) == 0) || (air_expect_pulse(true) == 0))   // Antwort des Sensors
  {ok = false;}

  for(uint8_t i = 0; (ok == true) && (i < 40); i++)
  {
    const uint16_t low = air_expect_pulse(false);
    const uint16_t high = air_expect_pulse(true);
    if((low == 0) || (high == 0))
    {
      ok = false;
      break;
    }
    data[i / 8] <<= 1;
    if(high > low)
    {data[i / 8] |= 1;}
  }
  SREG = sreg;

  if(ok == false)
  {return false;}
  if(data[4] != (uint8_t)(data[0] + data[1] + data[2] + data[3]))
  {return false;}                      // Prüfsumme stimmt nicht

  const int16_t humidity = (int16_t)data[0] * 10 + (data[1] & 0x0F);
  int16_t temperature = (int16_t)data[2] * 10 + (data[3] & 0x0F);
  if((data[3] & 0x80) != 0)
  {temperature = -temperature;}        // negative Temperatur (Bit 7 des Nachkomma-Bytes)

  if((humidity > 1000) || (temperature < -400) || (temperature > 800))
  {return false;}                      // außerhalb des Messbereichs des DHT11

  t_dc = temperature;
  rh_d10 = humidity;
  return true;
}

#endif

void air_update(unsigned long now)
{
  if((now - air_timer_ms) < AIR_INTERVAL_MS)
  {return;}
  air_timer_ms = now;

  int16_t t_dc = 0;
  int16_t rh_d10 = 0;

  if(air_read(t_dc, rh_d10) == false)
  {
    in.air_valid = false;              // ungültig: Wetterbedingung gilt als nicht erfüllt
    if(in.air_errors < 255)
    {in.air_errors++;}
  }
  else
  {
    in.air_valid = true;
    in.t_air_dc = t_dc;
    in.rh_air_d10 = rh_d10;
    if(in.air_errors > 0)
    {in.air_errors--;}                 // einzelne Aussetzer klingen wieder ab
  }
}
