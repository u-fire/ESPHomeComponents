#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/automation.h"

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

namespace esphome
{
    namespace now_mqtt
    {
        class Now_MQTTComponent : public Component
        {
        public:
            void setup() override;
            void set_wifi_channel(uint8_t channel) { this->wifi_channel_ = channel; }
            void add_on_state_callback(std::function<void(float)> &&callback) { this->callback_.add(std::move(callback)); }

        protected:
            uint8_t wifi_channel_;

        private:
            CallbackManager<void(float)> callback_;
            CallbackManager<void(std::string)> callback_text_;
            void on_sensor_update(sensor::Sensor *obj, float state);
            #ifdef USE_BINARY_SENSOR
            void on_binary_sensor_update(binary_sensor::BinarySensor *obj, float state);
            #endif
            #ifdef USE_TEXT_SENSOR
            void on_text_sensor_update(text_sensor::TextSensor *obj, std::string state);
            #endif
        };

        class ESPNowSendTrigger : public Trigger<float>
        {
        public:
            explicit ESPNowSendTrigger(Now_MQTTComponent *parent)
            {
                parent->add_on_state_callback([this](float value) { this->trigger(value); });
            }
        };
        
    } // namespace now_mqtt
} // namespace esphome
