#include "net.hpp"
#include "netconfig.hpp"
#include "persistence.hpp"

#include <ArduinoOTA.h>

namespace Net {

namespace {
bool led_status = HIGH;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(Config::wifi_default_ssid, Config::wifi_default_password);
  WiFi.config(Config::default_ip_address, Config::default_gateway, Config::default_netmask);

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(Config::ota_hostname);
  ArduinoOTA.setPassword(Config::ota_password);
  ArduinoOTA.begin();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, led_status = led_status == HIGH ? LOW : HIGH);
    return;
  }

  digitalWrite(LED_BUILTIN, HIGH); // LED aus = fertig
  ArduinoOTA.handle();
}

}
