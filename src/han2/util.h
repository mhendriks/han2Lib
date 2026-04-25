#ifndef HAN2_INCLUDE_UTIL_H
#define HAN2_INCLUDE_UTIL_H

#include <Arduino.h>
#include "fields2.h"

namespace han {

template<typename T, size_t N>
constexpr size_t array_size(const T (&)[N]) {
  return N;
}

namespace debug {

inline const char* listTypeName(HanListType type) {
  switch (type) {
    case HanListType::List1:
      return "List1";
    case HanListType::List2:
      return "List2";
    default:
      return "Unknown";
  }
}

inline void printField(Stream& out, const __FlashStringHelper* name, const fields::StringValue& value) {
  if (!value.present()) return;
  out.print(name);
  out.print(F(": "));
  out.println(value.val());
}

inline void printField(Stream& out, const __FlashStringHelper* name, const fields::FixedValue& value, int decimals = 3) {
  if (!value.present()) return;
  out.print(name);
  out.print(F(": "));
  out.println(value.val(), decimals);
}

inline void printHanData(Stream& out, const HanData& data, const __FlashStringHelper* title = nullptr) {
  if (title) out.println(title);

  out.print(F("list_type: "));
  out.println(listTypeName(data.list_type));
  printField(out, F("timestamp"), data.timestamp);
  printField(out, F("identification"), data.identification);
  printField(out, F("equipment_id"), data.equipment_id);
  printField(out, F("power_delivered_kw"), data.power_delivered_kw);
  printField(out, F("power_returned_kw"), data.power_returned_kw);
  printField(out, F("reactive_power_delivered_kvar"), data.reactive_power_delivered_kvar);
  printField(out, F("reactive_power_returned_kvar"), data.reactive_power_returned_kvar);
  printField(out, F("voltage_l1_v"), data.voltage_l1_v);
  printField(out, F("voltage_l2_v"), data.voltage_l2_v);
  printField(out, F("voltage_l3_v"), data.voltage_l3_v);
  printField(out, F("current_l1_a"), data.current_l1_a);
  printField(out, F("current_l2_a"), data.current_l2_a);
  printField(out, F("current_l3_a"), data.current_l3_a);
  printField(out, F("energy_delivered_total_kwh"), data.energy_delivered_total_kwh);
  printField(out, F("energy_returned_total_kwh"), data.energy_returned_total_kwh);
  printField(out, F("reactive_energy_delivered_total_kvarh"), data.reactive_energy_delivered_total_kvarh);
  printField(out, F("reactive_energy_returned_total_kvarh"), data.reactive_energy_returned_total_kvarh);
}

inline void printHanState(Stream& out, const HanState& state) {
  out.println(F("HanState"));
  out.print(F("has_list1: "));
  out.println(state.has_list1 ? F("true") : F("false"));
  out.print(F("has_list2: "));
  out.println(state.has_list2 ? F("true") : F("false"));

  if (state.has_list1) printHanData(out, state.last_list1, F("last_list1"));
  if (state.has_list2) printHanData(out, state.last_list2, F("last_list2"));
  printHanData(out, state.effective, F("effective"));
}

}  // namespace debug

}  // namespace han

#endif
