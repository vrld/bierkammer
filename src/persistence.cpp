#include "persistence.hpp"

namespace {
  bool is_dirty = false;
  size_t eeprom_size = 0;
}

size_t Persistence::eeprom_size()
{
  return ::eeprom_size;
}

void Persistence::increase_eeprom_size(size_t size)
{
  ::eeprom_size += size;
}

void Persistence::mark_dirty()
{
  ::is_dirty = true;
}

void Persistence::setup()
{
    EEPROM.begin(::eeprom_size);
    ::is_dirty = false;
}

void Persistence::loop()
{
  if (::is_dirty) {
    EEPROM.commit();
    ::is_dirty = false;
  }
}
