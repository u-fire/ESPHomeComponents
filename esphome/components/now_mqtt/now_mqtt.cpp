#include "now_mqtt.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <esphome/core/helpers.h>
#include "esphome/core/version.h"

namespace esphome
{
    namespace now_mqtt
    {
        static const char *const TAG = "now_mqtt.sensor";
        void Now_MQTTComponent::setup()
        {
            uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            esp_now_peer_info_t peerInfo = {};

            ESP_LOGD(TAG, "Setting up ESP-Now-MQTT...");
            ESP_ERROR_CHECK(esp_netif_init());
            ESP_ERROR_CHECK(esp_event_loop_create_default());
            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&cfg));
            ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_start());

            if (esp_now_init() != ESP_OK)
            {
                this->mark_failed();
                ESP_LOGE(TAG, "Error initializing ESP-NOW");
                return;
            }
            esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
            memcpy(peerInfo.peer_addr, broadcastAddress, 6);
            peerInfo.channel = 1;
            peerInfo.encrypt = false;

            if (esp_now_add_peer(&peerInfo) != ESP_OK)
            {
                this->mark_failed();
                ESP_LOGE(TAG, "Failed to add peer");
                return;
            }

            for (auto *obj : App.get_sensors())
            {
                obj->add_on_state_callback([this, obj](float state)
                                           { this->on_sensor_update(obj, state); });
            }
        }

        void Now_MQTTComponent::on_sensor_update(sensor::Sensor *obj, float state)
        {
            if (!obj->has_state())
                return;
            uint8_t serverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            std::string line;
            int8_t accuracy = obj->get_accuracy_decimals();

            line = str_snake_case(App.get_name());
            line += ":";
            line += obj->get_device_class().c_str();
            line += ":";
            line += state_class_to_string(obj->get_state_class()).c_str();
            line += ":";
            line += str_snake_case(obj->get_name().c_str());
            line += ":";
            line += obj->get_unit_of_measurement().c_str();
            line += ":";
            line += value_accuracy_to_string(state, accuracy);
            line += ":";
            line += obj->get_icon();
            line += ":";
            line += ESPHOME_VERSION;
            line += ":";
            line += ESPHOME_BOARD;
            line += "::"; // spaceholder for manufacturer

            ESP_LOGI(TAG, "ESP-Now-MQTT Publish:  %s", line.c_str());
            ESP_ERROR_CHECK(esp_now_send(serverAddress, reinterpret_cast<const uint8_t *>(&line[0]), line.size()));
        }
    } // namespace now_mqtt
} // namespace esphome