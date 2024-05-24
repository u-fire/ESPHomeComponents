#pragma once

#include "esphome/core/component.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "esp_wifi.h"

namespace esphome
{
    namespace now_mqtt_bridge
    {
        class Now_MQTT_BridgeComponent : public Component
        {
            typedef struct
            {
                unsigned frame_ctrl : 16;    // 2 bytes / 16 bit fields
                unsigned duration_id : 16;   // 2 bytes / 16 bit fields
                uint8_t addr1[6];            // receiver address
                uint8_t addr2[6];            // sender address
                uint8_t addr3[6];            // filtering address
                unsigned sequence_ctrl : 16; // 2 bytes / 16 bit fields
            } wifi_ieee80211_mac_hdr_t;      // 24 bytes

            typedef struct
            {
                wifi_ieee80211_mac_hdr_t hdr;
                unsigned category_code : 8; // 1 byte / 8 bit fields
                uint8_t oui[3];             // 3 bytes / 24 bit fields
                uint8_t payload[0];
            } wifi_ieee80211_packet_t;

        public:
            void setup() override;
            float get_setup_priority() const override;
            void set_wifi_channel(uint8_t channel) { this->wifi_channel_ = channel; }

        protected:
            uint8_t wifi_channel_;

        private:
            static int32_t last_rssi;
            void receivecallback(const uint8_t *bssid, const uint8_t *data, int len);
            static void call_on_data_recv_callback(const uint8_t *mac_addr, const uint8_t *incomingData, int len);
            void promcallback(void *buf, wifi_promiscuous_pkt_type_t type);
            static void call_prom_callback(void *buf, wifi_promiscuous_pkt_type_t type);
            void split(char **argv, int *argc, char *string, const char delimiter, int allowempty);
            mqtt::MQTTDiscoveryInfo discovery_info;
        };
    } // namespace now_mqtt_bridge
} // namespace esphome
