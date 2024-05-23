#include "now_mqtt_bridge.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include "esphome/components/mqtt/mqtt_client.h"

namespace esphome
{
    namespace now_mqtt_bridge
    {
        static const char *const TAG = "now_mqtt_bridge.sensor";
        int32_t Now_MQTT_BridgeComponent::last_rssi = 0;

        void Now_MQTT_BridgeComponent::receivecallback(const uint8_t *bssid, const uint8_t *data, int len)
        {
            char received_string[251];
            char config_topic[] = "%s/sensor/%s/%s/config";
            char sensor_topic[] = "%s/sensor/%s/state";
            char binary_config_topic[] = "%s/binary_sensor/%s/%s/config";
            char binary_sensor_topic[] = "%s/binary_sensor/%s/state";

            char topic[250];
            char macStr[18];
            StaticJsonDocument<500> doc;
            JsonObject dev;
            std::string json;
            std::string message_type;

            // sender mac address
            snprintf(macStr, sizeof(macStr), "%02x%02x%02x%02x%02x%02x", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

            // received data
            memset(&received_string, 0, sizeof(received_string));
            memcpy(&received_string, data, len);

            // tokenize the received string
            char *tokens[13];
            int argc;
            split(tokens, &argc, received_string, ':', 1);

            // if we didn't get 11 elements, this wasn't a message from our sensors
            if (argc != 11)
            {
                return;
            }

            ESP_LOGI(TAG, "line rcv: %s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s", tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], tokens[7], tokens[8], tokens[9], tokens[10]);

            // check for binary_sensor or sensor
            message_type = tokens[2];
            if (message_type.compare("binary_sensor") == 0)
            {
                if (strlen(tokens[3]) != 0)
                {
                    doc["name"] = tokens[3];
                }
                if (strlen(tokens[0]) != 0)
                {
                    std::string stat_t = tokens[0];
                    stat_t += "/binary_sensor/";
                    stat_t += tokens[3];
                    stat_t += "/state";
                    doc["stat_t"] = stat_t;
                }
                if (strlen(tokens[0]) != 0)
                {
                    std::string uniq_id = macStr;
                    uniq_id += "_";
                    uniq_id += tokens[3];
                    doc["uniq_id"] = uniq_id;
                }
                dev = doc.createNestedObject("dev");
                dev["ids"] = macStr;
                if (strlen(tokens[0]) != 0)
                {
                    dev["name"] = tokens[0];
                }
                dev["sw"] = tokens[8];
                dev["mdl"] = tokens[9];
                dev["mf"] = "espressif";
                serializeJson(doc, json);

                // make and send the config topic
                discovery_info = mqtt::global_mqtt_client->get_discovery_info();
                memset(&topic, 0, sizeof(topic));
                snprintf(topic, sizeof(topic), binary_config_topic, discovery_info.prefix.c_str(), tokens[0], tokens[3]);
                mqtt::global_mqtt_client->publish(topic, json.c_str(), json.length(), 2, true);

                // make and send the state topic
                memset(&topic, 0, sizeof(topic));
                snprintf(topic, sizeof(topic), binary_sensor_topic, tokens[0], tokens[3]);
                mqtt::global_mqtt_client->publish(topic, tokens[5], strlen(tokens[5]), 2, true);
            }
            else
            {
                if (strlen(tokens[1]) != 0)
                {
                    doc["dev_cla"] = tokens[1];
                }
                if (strlen(tokens[4]) != 0)
                {
                    doc["unit_of_meas"] = tokens[4];
                }
                if (strlen(tokens[2]) != 0)
                {
                    doc["stat_cla"] = tokens[2];
                }
                if (strlen(tokens[3]) != 0)
                {
                    doc["name"] = tokens[3];
                }
                if (strlen(tokens[6]) != 0)
                {
                    std::string icon = tokens[6];
                    icon += ":";
                    icon += tokens[7];
                    doc["icon"] = icon;
                }
                if (strlen(tokens[0]) != 0)
                {
                    std::string stat_t = tokens[0];
                    stat_t += "/sensor/";
                    stat_t += tokens[3];
                    stat_t += "/state";
                    doc["stat_t"] = stat_t;
                }
                if (strlen(tokens[0]) != 0)
                {
                    std::string uniq_id = macStr;
                    uniq_id += "_";
                    uniq_id += tokens[3];
                    doc["uniq_id"] = uniq_id;
                }
                dev = doc.createNestedObject("dev");
                dev["ids"] = macStr;
                if (strlen(tokens[0]) != 0)
                {
                    dev["name"] = tokens[0];
                }
                dev["sw"] = tokens[8];
                dev["mdl"] = tokens[9];
                dev["mf"] = "espressif";
                serializeJson(doc, json);

                // make and send the config topic
                discovery_info = mqtt::global_mqtt_client->get_discovery_info();
                memset(&topic, 0, sizeof(topic));
                snprintf(topic, sizeof(topic), config_topic, discovery_info.prefix.c_str(), tokens[0], tokens[3]);
                mqtt::global_mqtt_client->publish(topic, json.c_str(), json.length(), 2, true);

                // make and send the state topic
                memset(&topic, 0, sizeof(topic));
                snprintf(topic, sizeof(topic), sensor_topic, tokens[0], tokens[3]);
                mqtt::global_mqtt_client->publish(topic, tokens[5], strlen(tokens[5]), 2, true);
            }

            // create RSSI message
            json = "";
            doc["name"] = "rssi";
            doc["dev_cla"] = "SIGNAL_STRENGTH";
            doc["unit_of_meas"] = "dBm";
            doc["stat_cla"] = "measurement";
            doc["icon"] = "mdi:wifi";

            std::string stat_t = tokens[0];
            stat_t += "/sensor/";
            stat_t += "rssi";
            stat_t += "/state";
            doc["stat_t"] = stat_t;

            std::string uniq_id = macStr;
            uniq_id += "_";
            uniq_id += "rssi";
            doc["uniq_id"] = uniq_id;

            serializeJson(doc, json);

            // make and send the rssi config topic
            discovery_info = mqtt::global_mqtt_client->get_discovery_info();
            memset(&topic, 0, sizeof(topic));
            snprintf(topic, sizeof(topic), config_topic, discovery_info.prefix.c_str(), tokens[0], "rssi");
            mqtt::global_mqtt_client->publish(topic, json.c_str(), json.length(), 2, true);

            // make and send the rssi state topic
            memset(&topic, 0, sizeof(topic));
            snprintf(topic, sizeof(topic), sensor_topic, tokens[0], "rssi");
            std::string last_rssi_str = std::to_string(last_rssi);
            mqtt::global_mqtt_client->publish(topic, last_rssi_str.c_str(), last_rssi_str.length(), 2, true);
        }

        float Now_MQTT_BridgeComponent::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }

        void Now_MQTT_BridgeComponent::setup()
        {
            // if there's no wifi section, init wifi here
            #ifndef USE_WIFI
            ESP_LOGD(TAG, "Setting up ESP-Now WiFi interface...");
            ESP_ERROR_CHECK(esp_netif_init());
            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&cfg));
            ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_start());
            ESP_ERROR_CHECK(esp_wifi_set_channel(this->wifi_channel_, WIFI_SECOND_CHAN_NONE));
            #endif
            uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            esp_now_peer_info_t peerInfo = {};

            ESP_LOGD(TAG, "Setting up ESP-Now MQTT Bridge...");
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

            if (esp_now_init() != ESP_OK)
            {
                this->mark_failed();
                ESP_LOGE(TAG, "Error initializing ESP-Now MQTT Bridge");
                return;
            }
            esp_now_register_recv_cb(Now_MQTT_BridgeComponent::call_on_data_recv_callback);
            esp_wifi_set_promiscuous(true);
            esp_wifi_set_promiscuous_rx_cb(Now_MQTT_BridgeComponent::call_prom_callback);
        }

        void Now_MQTT_BridgeComponent::call_on_data_recv_callback(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
        {
            Now_MQTT_BridgeComponent().receivecallback(mac_addr, incomingData, len);
        }

        void Now_MQTT_BridgeComponent::call_prom_callback(void *buf, wifi_promiscuous_pkt_type_t type)
        {
            Now_MQTT_BridgeComponent().promcallback(buf, type);
        }

        void Now_MQTT_BridgeComponent::promcallback(void *buf, wifi_promiscuous_pkt_type_t type)
        {
            // only filter mgmt frames
            if (type != WIFI_PKT_MGMT)
                return;

            const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
            const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
            const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

            static const uint8_t esp_oui[3] = {0x18, 0xfe, 0x34}; // esp32 oui

            // Filter vendor specific frame with the esp oui.
            if (((ipkt->category_code) == 127) && (memcmp(ipkt->oui, esp_oui, 3) == 0))
            {
                last_rssi = ppkt->rx_ctrl.rssi;
            }
        }
        void Now_MQTT_BridgeComponent::split(char **argv, int *argc, char *string, const char delimiter, int allowempty)
        {
            *argc = 0;
            do
            {
                if (*string && (*string != delimiter || allowempty))
                {
                    argv[(*argc)++] = string;
                }
                while (*string && *string != delimiter)
                    string++;
                if (*string)
                    *string++ = 0;
                if (!allowempty)
                    while (*string && *string == delimiter)
                        string++;
            } while (*string);
        }
    } // namespace now_mqtt_bridge
} // namespace esphome
