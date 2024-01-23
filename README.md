# Bierkammer

Code for a DIY beer fermentation chamber built from a fridge, an ESP8266, some
temperature sensors and a remote controlled socket.

## Why?

Some beer-fermenting yeasts, especially bottom-fermenting ones, require
temperatures below room temperature to produce tasty beer. If the wort gets too
cold, the yeast won't do anything. If it gets too hot, it produces fusel
alcohols and unwanted esters. Professional fermentation chambers are expensive,
but you can combine a cheap, used fridge and some electronic parts to build
your own.

Mine also doubles as poor-mans home automation center / smart switch controller.

## Hardware

Required materials:

- 1 fridge off of ebay
- 1 Wemos D1 Mini (or similar)
- 2 or more DS18B20 temperature sensors in protective housing
- 1 433 MHz sender module, to control...
- 1 remote controlled socket

Installation:

- Install the temperature sensors inside the fridge
- Connect the sensors to the `D1` pin on the D1 Mini
- Connect the 433 MHz sender module to the `D7` pin
- Plug the fridge into the remote controlled socket

## Software

At this point, the chambers state is communicated and parameters are set via
MQTT. This means you need access to a broker, e.g., mosquitto running on a
Raspberry Pi.

Why MQTT? Because it enables monitoring and fine-grained control of temperature
curves using external scripts. It also allows the chamber to double as remote
switch controller for your smart home.

### Configuration

Secrets like Wifi SSID and password are excluded from this repository. Create
the following files:

- `include/netconfig.hpp`:

  ```cpp
  #pragma once
  #include <ESP8266WiFi.h>
  
  namespace Net {
  namespace Config {
  
  const char* ota_hostname = "bierkammer";
  const char* ota_password = "...";
  
  const char* wifi_default_ssid = "...";
  const char* wifi_default_password = "...";
  
  const IPAddress default_ip_address{..., ..., ..., ...};
  const IPAddress default_gateway{..., ..., ..., ...};
  const IPAddress default_netmask = IPAddress(255, 255, 255, 0);
  
  }
  }
  ```

- `include/mqttconfig.hpp`:
  
  ```cpp
  #pragma once
  #include <ESP8266WiFi.h>
  
  namespace MQTT {
  namespace Config {
  const IPAddress broker_ip{..., ..., ..., ...};
  const short broker_port = 1883;
  const char* client_id = "bierkammer";
  const char* username = "...";
  const char* password = "...";
  }
  }
  ```

  Note: the MQTT user must have access to these topics:
  read: `bierkammer/set_target`, `bierkammer/set_margin`,
  `bierkammer/set_control`, `bierkammer/set_power`, `bierkammer/ping`,
  `bierkammer/socket`; write: `bierkammer/tick`.



- `include/socketconfig.hpp`:

  ```cpp
  #pragma once
  #include "socket.hpp"
  #include <map>
  
  namespace Sockets {
    Socket<{CODE BIT LENGTH}> freezer({CODE ON}, {CODE OFF});
  
    std::map<String, Socket<{BIT LENGTH}>> named = {
      {"...", {..., ...}},
      ...
    };
  }
  ```
