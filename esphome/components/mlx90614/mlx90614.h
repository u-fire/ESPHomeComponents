#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome
{
    namespace mlx90614
    {
        static const uint8_t MLX90614_TA = 0x06;    ///< the register address stores the measured ambient temperature
        static const uint8_t MLX90614_TOBJ1 = 0x07; ///< the register address stores the measured temperature of object 1

        class MLX90614Sensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice
        {
        public:
            void set_ambient_temperature_sensor(sensor::Sensor *temperature_sensor) { ambient_temperature_sensor_ = temperature_sensor; }
            void set_object_temperature_sensor(sensor::Sensor *target_temperature_sensor) { object_temperature_sensor_ = target_temperature_sensor; }
            void setup() override;
            void dump_config() override;
            void update() override;
            float get_setup_priority() const override;
            void update_internal_();

        protected:
            sensor::Sensor *ambient_temperature_sensor_{nullptr};
            sensor::Sensor *object_temperature_sensor_{nullptr};
        };

    } // namespace mlx90614
} // namespace esphome
