#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/automation.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome
{
    namespace now_mqtt
    {
        class Now_MQTTComponent : public Component
        {
        public:
            void setup() override;
            void add_on_state_callback(std::function<void(float)> &&callback) { this->callback_.add(std::move(callback)); }

        private:
            CallbackManager<void(float)> callback_;
            void on_sensor_update(sensor::Sensor *obj, float state);
            void on_binary_sensor_update(binary_sensor::BinarySensor *obj, float state);
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
