# Han2Lib

Arduino reader/parser library for Scandinavian HAN/AMS smart meter messages.

## Goals

- Keep the public API close to `dsmr2Lib`
- Allow `HanReader` and `P1Reader` to coexist in the same sketch
- Parse byte-oriented HAN frames such as list 1 and list 2
- Retain hourly totals from list 2 while list 1 keeps updating live values
- Offer a DSMR-compatible mapping path for projects that already use `ParsedData`

## Current public API

```cpp
#include <han2.h>

HanReader reader(&Serial1);
HanData data;
String err;

void loop() {
  reader.loop();
  if (reader.available()) {
    if (reader.parse(&data, &err)) {
      // use data
    }
  }
}
```

For DSMR-compatible projects:

```cpp
#include <han2.h>
#include <dsmr2.h>

using MyData = ParsedData<
  identification,
  timestamp,
  equipment_id,
  energy_delivered_tariff1,
  energy_delivered_tariff2,
  energy_returned_tariff1,
  energy_returned_tariff2,
  power_delivered,
  power_returned,
  voltage_l1,
  voltage_l2,
  voltage_l3,
  current_l1,
  current_l2,
  current_l3
>;

HanReader reader(&Serial1);
MyData data;
String err;

void loop() {
  reader.loop();
  if (reader.available()) {
    if (reader.parse(&data, &err)) {
      // use DSMR-compatible data
    }
  }
}
```

## Current data model

`HanData` currently exposes:

- timestamp
- identification
- equipment_id
- list_type
- active import/export power
- reactive import/export power
- voltage L1/L2/L3
- current L1/L2/L3
- total import/export energy
- total reactive import/export energy

`HanReader::state()` exposes a retained `HanState` with:

- `last_list1`
- `last_list2`
- `effective`

## Notes

- CRC/HDLC validation is not yet implemented in this first cut.
- OBIS coverage is intentionally minimal and based on the currently used Kamstrup list 1 / list 2 fields.
- The `doc/` directory can contain vendor specifications and notes used to validate mappings.
