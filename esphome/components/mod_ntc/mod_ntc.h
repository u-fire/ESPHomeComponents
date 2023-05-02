#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome
{
    namespace mod_ntc
    {
        static const uint8_t MEASURE_TEMP_TASK = 40; /*!< Command to measure temperature */
        static const uint8_t BETA_TASK = 20;         /*!< Command to calibrate the probe */
        static const uint8_t I2C_TASK = 2;           /*!< Command to change the i2c address */

        static const uint8_t HW_VERSION_REGISTER = 0;  /*!< hardware version register */
        static const uint8_t FW_VERSION_REGISTER = 1;  /*!< firmware version  register */
        static const uint8_t TASK_REGISTER = 2;        /*!< task register */
        static const uint8_t STATUS_REGISTER = 3;      /*!< status register */
        static const uint8_t BETA_REGISTER = 4;        /*!< Beta register */
        static const uint8_t TEMP_C_REGISTER = 8;      /*!< Temperature in C register */
        static const uint8_t TEMP_K_REGISTER = 12;     /*!< Temperature in K register */
        static const uint8_t TEMP_F_REGISTER = 16;     /*!< Temperature in F register */
        static const uint8_t RESISTANCE_REGISTER = 20; /*!< resistance in Ohms register */
        static const uint8_t BUFFER_REGISTER = 24;     /*!< buffer register */

        typedef enum Temp_Error_e
        {
            STATUS_NO_ERROR,
            STATUS_NO_PROBE,
            STATUS_SYSTEM_ERROR
        } Temp_Error_t;

        class Mod_NTCSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice
        {
        public:
            void setup() override;
            void dump_config() override;
            void update() override;
            float get_setup_priority() const override;
            float setBeta(float beta);
            float getBeta();
            void reset();
            void update_internal_();
            void set_temperature_sensor_external(sensor::Sensor *temperature_sensor)
            {
                this->temperature_sensor = temperature_sensor;
            }

        protected:
            sensor::Sensor *temperature_sensor{nullptr};
            void _write_4_bytes(uint8_t reg, float f);
            void _send_command(uint8_t command);
            float _read_4_bytes(uint8_t reg);
        };

    } // namespace mod_ntc
} // namespace esphome
