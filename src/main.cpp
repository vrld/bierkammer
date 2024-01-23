#include "persistence.hpp"
#include "temperature.hpp"
#include "socketconfig.hpp"
#include "net.hpp"
#include "mqtt.hpp"

#include <ArduinoJson.h>

void mqtt_send_status(); // forward def needed in below

// changeable settings
namespace Tags {
  Persistence::Tag<float> target;
  Persistence::Tag<float> margin;
  Persistence::Tag<bool> control_on;
}
float target = 4.0f;
float margin = 1.0f;
bool control_on = true;

// controller
Temperature::Sensor temperature_sensor(D1);

namespace Switches {
  RCSwitch legacy;
  RCSwitch modern;
}

bool is_on(const String& s)
{
  return s == "on" || s == "true";
}

bool is_off(const String& s)
{
  return s == "off" || s == "false";
}

template<typename SOCKET>
void control_socket(SOCKET& socket, RCSwitch& schalter, const String& payload)
{
  if (is_on(payload)) {
    socket.turnOn(schalter);
  } else if (is_off(payload)) {
    socket.turnOff(schalter);
  }
  mqtt_send_status();
}

// MQTT
void mqtt_on_connect()
{
  MQTT::subscribe("bierkammer/set_target", [](const String& payload) {
    auto value = payload.toFloat();
    if (value < 0 || value > 25) {
      return;
    }

    target = Tags::target.put(value);
    mqtt_send_status();
  });

  MQTT::subscribe("bierkammer/set_margin", [](const String& payload) {
    auto value = payload.toFloat();
    if (value < 1) {
      return;
    }

    margin = Tags::margin.put(value);
    mqtt_send_status();
  });

  MQTT::subscribe("bierkammer/set_control", [](const String& payload) {
    control_on = Tags::control_on.put(is_on(payload));
    mqtt_send_status();
  });

  MQTT::subscribe("bierkammer/set_power", [](const String& payload) {
    control_socket(Sockets::freezer, Switches::legacy, payload);
  });

  MQTT::subscribe("bierkammer/ping", [](const String& payload) {
    mqtt_send_status();
  });

  MQTT::subscribe("bierkammer/socket", [](const String& payload) {
    auto pos_sep = payload.indexOf(' ');
    if (pos_sep == -1) {
      return;
    }

    auto named_socket = Sockets::named.find(payload.substring(0, pos_sep));
    if (named_socket == Sockets::named.end()) {
      return;
    }

    control_socket(named_socket->second, Switches::modern, payload.substring(pos_sep + 1));
  });

  mqtt_send_status();
}

void mqtt_send_status()
{
  JsonDocument doc;
  doc["temp"][0] = temperature_sensor.sample[0];
  doc["temp"][1] = temperature_sensor.sample[1];
  doc["mean"] = temperature_sensor.mean;
  doc["target"] = target;
  doc["margin"] = margin;
  doc["control"] = control_on;
  doc["power"] = Sockets::freezer.isOn();

  for (const auto& named_socket : Sockets::named) {
    doc["sockets"][named_socket.first] = named_socket.second.isOn();
  }

  String json;
  serializeJson(doc, json);
  MQTT::publish("bierkammer/tick", json);
}


// entry point
void setup()
{
  Persistence::setup();
  Net::setup();

  MQTT::setup(mqtt_on_connect);

  temperature_sensor.setup();
  target = Tags::target.get();
  margin = Tags::margin.get();
  control_on = Tags::control_on.get();

  Switches::legacy.enableTransmit(D7);
  Switches::legacy.setProtocol(1);
  Switches::legacy.setPulseLength(280);

  Switches::modern.enableTransmit(D7);
  Switches::modern.setProtocol(2);
  Switches::modern.setPulseLength(650);
  Switches::modern.setRepeatTransmit(30);
}

void loop()
{
  // roughly every minute
  temperature_sensor.loop();
  if (!control_on) {
  } else if (temperature_sensor.mean < target - margin) {
    Sockets::freezer.turnOff(Switches::legacy);
  } else if (temperature_sensor.mean > target + margin) {
    Sockets::freezer.turnOn(Switches::legacy);
  }
  mqtt_send_status();

  for (int i = 0; i < 600; ++i) {
    Net::loop();
    Persistence::loop();
    MQTT::loop();
    delay(100);
  }
}
