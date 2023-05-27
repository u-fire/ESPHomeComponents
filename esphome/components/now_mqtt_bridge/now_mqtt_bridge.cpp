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

        void Now_MQTT_BridgeComponent::receivecallback(const uint8_t *bssid, const uint8_t *data, int len)
        {
            char received_string[251];
            char config_topic[] = "%s/sensor/%s/%s/config";
            char sensor_topic[] = "%s/sensor/%s/state";
            char topic[250];
            char macStr[18];
            StaticJsonDocument<500> doc;
            std::string json;

            // sender mac address
            snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                     bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

            // received data
            memset(&received_string, 0, sizeof(received_string));
            memcpy(&received_string, data, len);

            // tokenize the received string
            char *tokens[6];
            int argc;
            split(tokens, &argc, received_string, ':', 1);
            ESP_LOGI(TAG, "ESP-Now: %s:%s:%s:%s:%s:%s", tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
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
            JsonObject dev = doc.createNestedObject("dev");
            dev["ids"] = macStr;
            if (strlen(tokens[0]) != 0)
            {
                dev["name"] = tokens[0];
            }
            dev["sw"] = "1.0.0";
            dev["mdl"] = "esp32";
            dev["mf"] = "espressif";
            serializeJson(doc, json);

            // make and send the config topic
            discovery_info = mqtt::global_mqtt_client->get_discovery_info();
            memset(&topic, 0, sizeof(topic));
            snprintf(topic, sizeof(topic), config_topic, discovery_info.prefix.c_str(), tokens[0], tokens[3]);
            ESP_LOGI(TAG, "ESP-Now: %s", topic);
            mqtt::global_mqtt_client->publish(topic, json.c_str(), json.length(), 2, true);

            // make and send the state topic
            memset(&topic, 0, sizeof(topic));
            snprintf(topic, sizeof(topic), sensor_topic, tokens[0], tokens[3]);
            ESP_LOGI(TAG, "ESP-Now: %s", topic);
            mqtt::global_mqtt_client->publish(topic, tokens[5], strlen(tokens[5]), 2, true);
        }

        float Now_MQTT_BridgeComponent::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }

        void Now_MQTT_BridgeComponent::setup()
        {
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
        }

        void Now_MQTT_BridgeComponent::call_on_data_recv_callback(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
        {
            Now_MQTT_BridgeComponent().receivecallback(mac_addr, incomingData, len);
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
