#pragma once

#include <Arduino.h>
#include <ESP_EEPROM.h>

namespace Persistence
{
  size_t eeprom_size();
  void increase_eeprom_size(size_t size);
  void mark_dirty();

  template<typename T>
  struct Tag {
    uint16_t offset;
    constexpr size_t size() const { return sizeof(T); }

    Tag() : offset(eeprom_size()) { increase_eeprom_size(size()); }
    Tag(const Tag&) = delete;
    Tag& operator=(const Tag&) = delete;

    T get() {
      T value;
      EEPROM.get(offset, value);
      return value;
    }

    const T& put(const T& value) {
      EEPROM.put(offset, value);
      mark_dirty();
      return value;
    }
  };

  void setup();
  void loop();
}
