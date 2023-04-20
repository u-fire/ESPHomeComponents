#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome
{
    namespace mod_orp
    {
        static const uint8_t MEASURE_ORP_TASK = 80;     /*!< Command to start a pH measure */
        static const uint8_t CALIBRATE_SINGLE_TASK = 4; /*!< Command to calibrate the high point of the probe */
        static const uint8_t I2C_TASK = 2;              /*!< Command to change the i2c address */

        static const uint8_t HW_VERSION_REGISTER = 0;               /*!< hardware version register */
        static const uint8_t FW_VERSION_REGISTER = 1;               /*!< firmware version  register */
        static const uint8_t TASK_REGISTER = 2;                     /*!< task register */
        static const uint8_t STATUS_REGISTER = 3;                   /*!< status register */
        static const uint8_t MV_REGISTER = 4;                       /*!< mV register */
        static const uint8_t TEMP_C_REGISTER = 8;                   /*!< temperature in C register */
        static const uint8_t CALIBRATE_SINGLE_OFFSET_REGISTER = 12; /*!< single-point calibration register */

        typedef enum ORP_Error_e
        {
            STATUS_NO_ERROR,
            STATUS_SYSTEM_ERROR
        } ORP_Error_t;

        /// This class implements support for the i2c-based Microfire Mod-EC water conductivity sensor.
        class Mod_ORPSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice
        {
        public:
            void setup() override;
            void dump_config() override;
            void update() override;
            float get_setup_priority() const override;
            void calibrateSingle(float calibration_ph);
            void calibrateReset();
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

    } // namespace mod_orp
} // namespace esphome
