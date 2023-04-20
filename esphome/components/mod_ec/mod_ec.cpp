#include "mod_ec.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace mod_ec
    {

        static const char *const TAG = "mod_ec.sensor";

        void Mod_ECSensor::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up Mod-EC...");

            uint8_t version;
            if (!this->read_byte(HW_VERSION_REGISTER, &version) && version != 0xFF)
            {
                this->mark_failed();
                return;
            }
            ESP_LOGI(TAG, "Found Mod-EC version %d", version);
        }

        void Mod_ECSensor::dump_config()
        {
            ESP_LOGI(TAG, "Low reference/read %f/%f", _read_4_bytes(CALIBRATE_READLOW_REGISTER), _read_4_bytes(CALIBRATE_REFLOW_REGISTER));
            ESP_LOGI(TAG, "Mid reference/read %f/%f", _read_4_bytes(CALIBRATE_READMID_REGISTER), _read_4_bytes(CALIBRATE_REFMID_REGISTER));
            ESP_LOGI(TAG, "High reference/read %f/%f", _read_4_bytes(CALIBRATE_READHIGH_REGISTER), _read_4_bytes(CALIBRATE_REFHIGH_REGISTER));
            ESP_LOGI(TAG, "Single point %f", _read_4_bytes(CALIBRATE_SINGLE_OFFSET_REGISTER));
            ESP_LOGI(TAG, "Temperature Constant %f", temperature_constant_);
            ESP_LOGI(TAG, "Temperature Coefficient %f", temperature_coefficient_);
        }

        void Mod_ECSensor::update()
        {
            int wait = 750;
            
            if (this->temperature_sensor != nullptr)
            {
                ESP_LOGD(TAG, "temperature_sensor %f", this->temperature_sensor->state);
                _write_4_bytes(TEMP_C_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                ESP_LOGD(TAG, "temperature_sensor %f", 25.0);
                _write_4_bytes(TEMP_C_REGISTER, 25.0);
            }

            _write_4_bytes(CONSTANT_REGISTER, temperature_constant_);
            _write_4_bytes(COEFFICIENT_REGISTER, temperature_coefficient_);
            _write_4_bytes(K_REGISTER, 1.0);
            _write_4_bytes(KPA_REGISTER, 0);

            _send_command(MEASURE_EC_TASK);
            this->set_timeout("data", wait, [this]()
                              { this->update_internal_(); });
        }

        void Mod_ECSensor::update_internal_()
        {
            uint8_t status;
            this->read_byte(STATUS_REGISTER, &status);
            if (status == STATUS_NO_ERROR)
            {
                float ms = _read_4_bytes(MS_REGISTER);
                ESP_LOGD(TAG, "'%s': Got EC=%.2f mS/cm", this->get_name().c_str(), ms);
                this->publish_state(ms);
            }
            else
            {
                switch (status)
                {
                case STATUS_NO_PROBE_RANGE:
                    ESP_LOGE(TAG, "Error: probe not connected or solution out of range");
                    break;
                case STATUS_SYSTEM_ERROR:
                    ESP_LOGE(TAG, "Error: module not functioning properly");
                    break;
                case STATUS_CONFIG_ERROR:
                    ESP_LOGE(TAG, "parameters or calibration data incorrect");
                    break;
                }
                ESP_LOGD(TAG, "temperature_constant_ %f", temperature_constant_);
                ESP_LOGD(TAG, "temperature_coefficient_ %f", temperature_coefficient_);
            }
        }

        void Mod_ECSensor::calibrateSingle(float calibration_ms)
        {
            ESP_LOGD(TAG, "Calibrate Single Pressed.", calibration_ms);
            _write_4_bytes(MS_REGISTER, calibration_ms);
            if (this->temperature_sensor != nullptr)
            {
                _write_4_bytes(TEMP_C_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                _write_4_bytes(TEMP_C_REGISTER, 25.0);
            }
            _write_4_bytes(CONSTANT_REGISTER, temperature_constant_);
            _write_4_bytes(COEFFICIENT_REGISTER, temperature_coefficient_);
            _write_4_bytes(K_REGISTER, 1);

            _send_command(CALIBRATE_SINGLE_TASK);
        }

        void Mod_ECSensor::calibrateLow(float calibration_ms)
        {
            ESP_LOGD(TAG, "Calibrate Low Pressed.", calibration_ms);
            _write_4_bytes(MS_REGISTER, calibration_ms);
            if (this->temperature_sensor != nullptr)
            {
                _write_4_bytes(TEMP_C_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                _write_4_bytes(TEMP_C_REGISTER, 25.0);
            }
            _write_4_bytes(CONSTANT_REGISTER, temperature_constant_);
            _write_4_bytes(COEFFICIENT_REGISTER, temperature_coefficient_);
            _write_4_bytes(K_REGISTER, 1);

            _send_command(CALIBRATE_LOW_TASK);
        }

        void Mod_ECSensor::calibrateMid(float calibration_ms)
        {
            ESP_LOGD(TAG, "Calibrate Mid Pressed.", calibration_ms);
            _write_4_bytes(MS_REGISTER, calibration_ms);
            if (this->temperature_sensor != nullptr)
            {
                _write_4_bytes(TEMP_C_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                _write_4_bytes(TEMP_C_REGISTER, 25.0);
            }
            _write_4_bytes(CONSTANT_REGISTER, temperature_constant_);
            _write_4_bytes(COEFFICIENT_REGISTER, temperature_coefficient_);
            _write_4_bytes(K_REGISTER, 1.0);

            _send_command(CALIBRATE_MID_TASK);
        }

        void Mod_ECSensor::calibrateHigh(float calibration_ms)
        {
            ESP_LOGD(TAG, "Calibrate High Pressed. %f", calibration_ms);
            _write_4_bytes(MS_REGISTER, calibration_ms);
            if (this->temperature_sensor != nullptr)
            {
                _write_4_bytes(TEMP_C_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                _write_4_bytes(TEMP_C_REGISTER, 25.0);
            }
            _write_4_bytes(CONSTANT_REGISTER, temperature_constant_);
            _write_4_bytes(COEFFICIENT_REGISTER, temperature_coefficient_);
            _write_4_bytes(K_REGISTER, 1.0);

            _send_command(CALIBRATE_HIGH_TASK);
        }

        void Mod_ECSensor::calibrateReset()
        {
            ESP_LOGD(TAG, "Calibrate Reset Pressed.");
            _write_4_bytes(CALIBRATE_REFHIGH_REGISTER, NAN);
            _write_4_bytes(CALIBRATE_REFLOW_REGISTER, NAN);
            _write_4_bytes(CALIBRATE_READHIGH_REGISTER, NAN);
            _write_4_bytes(CALIBRATE_READLOW_REGISTER, NAN);
            _write_4_bytes(CALIBRATE_REFMID_REGISTER, NAN);
            _write_4_bytes(CALIBRATE_READMID_REGISTER, NAN);
            _write_4_bytes(CALIBRATE_SINGLE_OFFSET_REGISTER, NAN);
        }

        float Mod_ECSensor::get_setup_priority() const { return setup_priority::DATA; }

        void Mod_ECSensor::_write_4_bytes(uint8_t reg, float f)
        {
            uint8_t b[4];
            float f_val = f;

            b[0] = *((uint8_t *)&f_val);
            b[1] = *((uint8_t *)&f_val + 1);
            b[2] = *((uint8_t *)&f_val + 2);
            b[3] = *((uint8_t *)&f_val + 3);
            this->write_register(reg, b, 4);
        }

        void Mod_ECSensor::_send_command(uint8_t command)
        {
            this->write_byte(TASK_REGISTER, command);
        }

        float Mod_ECSensor::_read_4_bytes(uint8_t reg)
        {
            float f;
            uint8_t temp[4];

            this->write(&reg, 1);
            this->read(temp, 4);
            memcpy(&f, temp, sizeof(f));

            return f;
        }
    } // namespace mod_ec
} // namespace esphome
