#include "lora_mqtt.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <esp_wifi.h>
#include <esphome/core/helpers.h>
#include "esphome/core/version.h"
#include <SPI.h>
#include "LoRa.h"

namespace esphome
{
    namespace lora_mqtt
    {
        static const char *const TAG = "lora_mqtt.sensor";
        void Lora_MQTTComponent::setup()
        {
            ESP_LOGD(TAG, "Setting up LoRa-MQTT...");
            auto *cs_internal = (InternalGPIOPin *)_cs;
            int cs_pin = cs_internal->get_pin();
            auto *reset_internal = (InternalGPIOPin *)_reset;
            int reset_pin = reset_internal->get_pin();
            auto *dio0_internal = (InternalGPIOPin *)_dio0;
            int dio0_pin = dio0_internal->get_pin();

            LoRa.setPins(cs_pin, reset_pin, dio0_pin);
            if (!LoRa.begin(_frequency))
            {
                this->mark_failed();
                ESP_LOGE(TAG, "Error initializing LoRa");
                return;
            }
            LoRa.setSyncWord(_sync);
            LoRa.setCodingRate4(_coding);
            LoRa.setSpreadingFactor(_spread);
            LoRa.setSignalBandwidth(_bandwidth);
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
        }

#ifdef USE_BINARY_SENSOR
        void Lora_MQTTComponent::on_binary_sensor_update(binary_sensor::BinarySensor *obj, float state)
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

            ESP_LOGI(TAG, "LoRa-MQTT Publish:  %s", line.c_str());
            LoRa.beginPacket();
            LoRa.print(line.c_str());
            LoRa.endPacket();
            this->callback_.call(state);
        }
#endif

#ifdef USE_TEXT_SENSOR
        void Lora_MQTTComponent::on_text_sensor_update(text_sensor::TextSensor *obj, std::string state)
        {
            if (!obj->has_state())
                return;
            uint8_t serverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            std::string line;

            line = str_snake_case(App.get_name());
            line += ":";
            // line += obj->get_device_class().c_str();
            line += ":";
            // line += "sensor";
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

            ESP_LOGI(TAG, "LoRa-MQTT Publish:  %s", line.c_str());
            LoRa.beginPacket();
            LoRa.print(line.c_str());
            LoRa.endPacket();
            this->callback_text_.call(state);
        }
#endif

        void Lora_MQTTComponent::on_sensor_update(sensor::Sensor *obj, float state)
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

            ESP_LOGI(TAG, "LoRa-MQTT Publish:  %s", line.c_str());
            LoRa.beginPacket();
            LoRa.print(line.c_str());
            LoRa.endPacket();
            this->callback_.call(state);
        }
    } // namespace lora_mqtt
} // namespace esphome
