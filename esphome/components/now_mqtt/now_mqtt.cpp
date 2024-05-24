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
            ESP_ERROR_CHECK(esp_wifi_set_channel(this->wifi_channel_, WIFI_SECOND_CHAN_NONE));

            if (esp_now_init() != ESP_OK)
            {
                this->mark_failed();
                ESP_LOGE(TAG, "Error initializing ESP-NOW");
                return;
            }
            esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
            memcpy(peerInfo.peer_addr, broadcastAddress, 6);
            peerInfo.channel = this->wifi_channel_;
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

            #ifdef USE_BINARY_SENSOR
            for (auto *obj : App.get_binary_sensors())
            {
                obj->add_on_state_callback([this, obj](float state)
                                           { this->on_binary_sensor_update(obj, state); });
            }
            #endif

            #ifdef USE_TEXT_SENSOR
            for (auto *obj : App.get_text_sensors())
            {
                obj->add_on_state_callback([this, obj](std::string state)
                                           { this->on_text_sensor_update(obj, state); });
            }
            #endif
        }

        #ifdef USE_BINARY_SENSOR
        void Now_MQTTComponent::on_binary_sensor_update(binary_sensor::BinarySensor *obj, float state)
        {
            if (!obj->has_state())
                return;
            uint8_t serverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            std::string line;
            const char *state_s = state ? "ON" : "OFF";

            line = str_snake_case(App.get_name());
            line += ":";
            line += obj->get_device_class().c_str();
            line += ":";
            line += "binary_sensor";
            line += ":";
            line += str_snake_case(obj->get_name().c_str());
            line += ":";
            line += ":";
            line += state_s;
            line += ":";
            if (obj->get_icon().length() != 0)
            {
                line += obj->get_icon();
            }
            else
            {
                line += ":";
            }
            line += ":";
            line += ESPHOME_VERSION;
            line += ":";
            line += ESPHOME_BOARD;
            line += "::";

            ESP_LOGI(TAG, "ESP-Now-MQTT Publish:  %s", line.c_str());
            ESP_ERROR_CHECK(esp_now_send(serverAddress, reinterpret_cast<const uint8_t *>(&line[0]), line.size()));
            this->callback_.call(state);
        }
        #endif
        
        #ifdef USE_TEXT_SENSOR
        void Now_MQTTComponent::on_text_sensor_update(text_sensor::TextSensor *obj, std::string state)
        {
            if (!obj->has_state())
                return;
            uint8_t serverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            std::string line;

            line = str_snake_case(App.get_name());
            line += ":";
            //line += obj->get_device_class().c_str();
            line += ":";
            //line += "sensor";
            line += ":";
            line += str_snake_case(obj->get_name().c_str());
            line += ":";
            line += ":";
            line += state;
            line += ":";
            if (obj->get_icon().length() != 0)
            {
                line += obj->get_icon();
            }
            else
            {
                line += ":";
            }
            line += ":";
            line += ESPHOME_VERSION;
            line += ":";
            line += ESPHOME_BOARD;
            line += "::";

            ESP_LOGI(TAG, "ESP-Now-MQTT Publish:  %s", line.c_str());
            ESP_ERROR_CHECK(esp_now_send(serverAddress, reinterpret_cast<const uint8_t *>(&line[0]), line.size()));
            this->callback_text_.call(state);
        }
        #endif

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
            if (obj->get_icon().length() != 0)
            {
                line += obj->get_icon();
            }
            else
            {
                line += ":";
            }
            line += ":";
            line += ESPHOME_VERSION;
            line += ":";
            line += ESPHOME_BOARD;
            line += ":sensor:";

            ESP_LOGI(TAG, "ESP-Now-MQTT Publish:  %s", line.c_str());
            ESP_ERROR_CHECK(esp_now_send(serverAddress, reinterpret_cast<const uint8_t *>(&line[0]), line.size()));
            this->callback_.call(state);
        }
    } // namespace now_mqtt
} // namespace esphome
