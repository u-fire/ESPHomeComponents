#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome
{
    namespace mod_ec
    {

        static const uint8_t UFIRE_MOD_EC = 0x0a;
        static const uint8_t MEASURE_EC_TASK = 80;      /*!< Command to start an EC measure */
        static const uint8_t CALIBRATE_LOW_TASK = 20;   /*!< Command to calibrate the probe */
        static const uint8_t CALIBRATE_MID_TASK = 10;   /*!< Command to calibrate the probe */
        static const uint8_t CALIBRATE_HIGH_TASK = 8;   /*!< Command to calibrate the high point of the probe */
        static const uint8_t CALIBRATE_SINGLE_TASK = 4; /*!< Command to calibrate the high point of the probe */
        static const uint8_t I2C_TASK = 2;              /*!< Command to change the i2c address */

        static const uint8_t HW_VERSION_REGISTER = 0;               /*!< hardware version register */
        static const uint8_t FW_VERSION_REGISTER = 1;               /*!< firmware version  register */
        static const uint8_t TASK_REGISTER = 2;                     /*!< task register */
        static const uint8_t STATUS_REGISTER = 3;                   /*!< status register */
        static const uint8_t MS_REGISTER = 4;                       /*!< mS register */
        static const uint8_t PSU_REGISTER = 8;                      /*!< PSU register */
        static const uint8_t TEMP_C_REGISTER = 12;                  /*!< temperature in C register */
        static const uint8_t CALIBRATE_REFLOW_REGISTER = 16;        /*!< reference low register */
        static const uint8_t CALIBRATE_READLOW_REGISTER = 20;       /*!< read high register */
        static const uint8_t CALIBRATE_REFMID_REGISTER = 24;        /*!< reference mid register */
        static const uint8_t CALIBRATE_READMID_REGISTER = 28;       /*!< reading mid register */
        static const uint8_t CALIBRATE_REFHIGH_REGISTER = 32;       /*!< reference high register */
        static const uint8_t CALIBRATE_READHIGH_REGISTER = 36;      /*!< reading high register */
        static const uint8_t CALIBRATE_SINGLE_OFFSET_REGISTER = 40; /*!< calibration single register */
        static const uint8_t COEFFICIENT_REGISTER = 44;             /*!< temperature coefficient register */
        static const uint8_t CONSTANT_REGISTER = 48;                /*!< temperature constant register */
        static const uint8_t K_REGISTER = 52;                       /*!< Probe cell constant register */
        static const uint8_t KPA_REGISTER = 56;                     /*!< kPa register */
        static const uint8_t DENSITY_REGISTER = 60;                 /*!< density register */

        typedef enum EC_Error_e
        {
            STATUS_NO_ERROR,
            STATUS_NO_PROBE_RANGE,
            STATUS_SYSTEM_ERROR,
            STATUS_CONFIG_ERROR
        } EC_Error_t;

        /// This class implements support for the i2c-based Microfire Mod-EC water conductivity sensor.
        class Mod_ECSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice
        {
        public:
            void setup() override;
            void dump_config() override;
            void update() override;
            float get_setup_priority() const override;
            void calibrateLow(float calibration_ms);
            void calibrateMid(float calibration_ms);
            void calibrateHigh(float calibration_ms);
            void calibrateSingle(float calibration_ms);
            void calibrateReset();
            void update_internal_();
            void set_temperature_sensor_external(sensor::Sensor *temperature_sensor)
            {
                this->temperature_sensor = temperature_sensor;
            }
            void set_temperature_constant(float constant) { this->temperature_constant_ = constant; }
            void set_temperature_coefficient(float coefficient) { this->temperature_coefficient_ = coefficient; }

        protected:
            sensor::Sensor *temperature_sensor{nullptr};
            float temperature_constant_{0.0};
            float temperature_coefficient_{0.0};

            void _write_4_bytes(uint8_t reg, float f);
            void _send_command(uint8_t command);
            float _read_4_bytes(uint8_t reg);
        };

    } // namespace mod_ec
} // namespace esphome
