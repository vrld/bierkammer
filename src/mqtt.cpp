#include "mqtt.hpp"
#include "mqttconfig.hpp"

#include <ESP8266WiFi.h>

namespace MQTT {

Client Client::instance;

namespace {
WiFiClient wifi_client;

void mqtt_callback(char* topic_cstr, byte* payload_bytes, unsigned int payload_length)
{
  if (payload_length == 0)
    return;

  payload_bytes[payload_length] = '\0';
  Client::instance.handle(topic_cstr, (char*)payload_bytes);
}
}

Client::Client()
: mqtt(Config::broker_ip, Config::broker_port, mqtt_callback, wifi_client)
{}

void Client::loop()
{
  if (!mqtt.connected() && mqtt.connect(Config::client_id, Config::username, Config::password)) {
    on_connect();
  }

  if (!mqtt.connected()) {
    return;
  }

  mqtt.loop();

  for (const auto& it : to_publish) {
    mqtt.publish(it.first.c_str(), it.second.c_str());
  }
  to_publish.clear();
}

void Client::subscribe(const String& topic, const Handler& handler)
{
  mqtt.subscribe(topic.c_str(), 1);
  handlers.push_back(std::make_pair(topic, handler));
}

void Client::handle(const char* topic, const char* payload)
{
  for (auto& candidate : handlers) {
    if (strcmp(candidate.first.c_str(), topic) == 0) {
      candidate.second(payload);
    }
  }
}

void Client::publish(const String& topic, const String& payload)
{
  to_publish.push_back(std::make_pair(topic, payload));
}

}
