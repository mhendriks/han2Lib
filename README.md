# Han2Lib

Arduino reader/parser library for Scandinavian HAN/AMS smart meter messages.

## Goals

- Keep the public API close to `dsmr2Lib`
- Allow `HanReader` and `P1Reader` to coexist in the same sketch
- Parse byte-oriented HAN frames such as list 1 and list 2
- Retain hourly totals from list 2 while list 1 keeps updating live values
- Offer a DSMR-compatible mapping path for projects that already use `ParsedData`

## Supported HAN profiles

The library is structured so multiple Scandinavian HAN profiles can be supported
behind one API. At the moment the implemented and tested focus is:

- `HAN-NVE` / Kamstrup list 1 and list 2 style push frames

Profiles currently recognized as relevant design targets:

- `HAN-NVE`
- `HAN DLMS push 1`
- `HAN-P1`

Current limitations:

- `HAN-NVE` is the profile that is actively mapped today
- reactive values are parsed where available, but are not yet the main focus of
  the compatibility layer
- per-phase power is not synthesized when the source profile does not provide it
- CRC/HDLC validation is not yet implemented in this first cut

## Direct `HanData` usage

Use this mode when your project wants direct access to HAN-native values without
going through DSMR compatibility fields.

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

`HanData` currently exposes:

- `timestamp`
- `identification`
- `equipment_id`
- `list_type`
- active import/export power
- reactive import/export power
- voltage `L1/L2/L3`
- current `L1/L2/L3`
- total import/export energy
- total reactive import/export energy

`HanReader::state()` exposes a retained `HanState` with:

- `last_list1`
- `last_list2`
- `effective`

That retained state is important for `HAN-NVE`, where list 1 contains live data
and list 2 refreshes the cumulative totals.

## DSMR-compat usage

Use this mode when your project already depends on `dsmr2Lib` and expects data
in a `ParsedData<...>` structure. In that case `HanReader::parse(...)` can map
directly to the same destination model.

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

This mode is useful when the rest of your firmware should remain identical after
`read -> parse -> map`. In that setup only the transport, parser and mapping are
HAN-specific; everything after `parse(...)` can stay generic.

## Test and replay

The library includes replay helpers so projects can test parser and mapping
logic without a live meter.

Available helper types:

- `HanReplayDriver`
- `han::test::KamstrupNveReplayProfile`

`HanReplayDriver` can replay two frame types with independent timings:

- list 1 interval, default `5000 ms`
- list 2 interval, default `20000 ms`

Example:

```cpp
#include <han2.h>

HanReader reader(&Serial1);
han::test::KamstrupNveReplayProfile replay;

void loop() {
  replay.step(reader, millis(), false);

  if (reader.available()) {
    HanData data;
    String err;
    if (reader.parse(&data, &err)) {
      // handle replayed data
    }
  }
}
```

Dynamic replay mode is also available through the Kamstrup profile:

```cpp
replay.step(reader, millis(), true);
```

In dynamic mode the profile mutates the frame data before replay so you can test
with changing:

- voltages
- currents
- total power
- cumulative totals in list 2

The replay path feeds real frame bytes into `HanReader`, so it tests more than
just the field mapping. It also exercises the frame ingestion and parser flow.

## Notes

- OBIS coverage is intentionally minimal and based on the currently used Kamstrup list 1 / list 2 fields.
- The `doc/` directory can contain vendor specifications and notes used to validate mappings.
- The API intentionally stays close to `dsmr2Lib`, but HAN remains a binary
  protocol internally. This library parses byte frames directly instead of first
  expanding them to text.
