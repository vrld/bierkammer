#pragma once

#include <PubSubClient.h>
#include <functional>
#include <vector>

namespace MQTT {

using Handler = std::function<void(const String&)>;

class Client
{
private:
  Client();
  std::vector<std::pair<String, Handler>> handlers;
  std::vector<std::pair<String, String>> to_publish;
  PubSubClient mqtt;

public:
  static Client instance;
  void loop();
  void subscribe(const String& topic, const Handler& handler);
  void handle(const char* topic, const char* payload);
  void publish(const String& topic, const String& payload);
  std::function<void()> on_connect = [](){};
};

inline void setup(std::function<void()> on_connect) { Client::instance.on_connect = on_connect; }
inline void loop() { Client::instance.loop(); }
inline void publish(const String& topic, const String& payload) { Client::instance.publish(topic, payload); }
inline void subscribe(const String& topic, const Handler& handler) { Client::instance.subscribe(topic, handler); }

}
