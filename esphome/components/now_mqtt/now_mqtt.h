#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
    namespace now_mqtt
    {
        class Now_MQTTComponent : public Component
        {
        public:
            void setup() override;
        private:
            void on_sensor_update(sensor::Sensor *obj, float state);
        };
    } // namespace mod_ntc
} // namespace esphome
