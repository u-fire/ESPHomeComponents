#pragma once

#include "esphome/core/component.h"
#include "esphome/components/mqtt/mqtt_client.h"

namespace esphome
{
    namespace now_mqtt_bridge
    {
        class Now_MQTT_BridgeComponent : public Component
        {
        public:
            void setup() override;
            float get_setup_priority() const override;

        private:
            void receivecallback(const uint8_t *bssid, const uint8_t *data, int len);
            static void call_on_data_recv_callback(const uint8_t *mac_addr, const uint8_t *incomingData, int len);
            void split(char **argv, int *argc, char *string, const char delimiter, int allowempty);

            mqtt::MQTTDiscoveryInfo discovery_info;
        };
    } // namespace now_mqtt_bridge
} // namespace esphome
