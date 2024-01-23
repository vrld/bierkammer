#pragma once
#include <RCSwitch.h>

template<uint8_t CODE_BITS>
struct Socket {
  enum socket_state { UNDEFINED = 0, SOCKET_ON, SOCKET_OFF, };
  socket_state state;

  uint32_t code_on;
  uint32_t code_off;

  Socket(uint32_t code_on, uint32_t code_off)
  : code_on(code_on)
  , code_off(code_off)
  {}

  void turnOn(RCSwitch& rcswitch)
  {
    rcswitch.send(code_on, CODE_BITS);
    state = SOCKET_ON;
  }

  void turnOff(RCSwitch& rcswitch)
  {
    rcswitch.send(code_off, CODE_BITS);
    state = SOCKET_OFF;
  }

  constexpr bool isOn() const {
    return state == SOCKET_ON;
  }

  constexpr bool isOff() const {
    return state == SOCKET_OFF;
  }
};

