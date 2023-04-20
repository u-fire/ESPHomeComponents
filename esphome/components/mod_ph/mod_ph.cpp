#include "mod_ph.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace mod_ph
    {

        static const char *const TAG = "mod_ph.sensor";

        void Mod_pHSensor::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up Mod-pH...");

            uint8_t version;
            if (!this->read_byte(HW_VERSION_REGISTER, &version) && version != 0xFF)
            {
                this->mark_failed();
                return;
            }
            ESP_LOGI(TAG, "Found Mod-pH version %d", version);
        }

        void Mod_pHSensor::dump_config()
        {
            ESP_LOGI(TAG, "Low reference/read %f/%f", _read_4_bytes(CALIBRATE_READLOW_REGISTER), _read_4_bytes(CALIBRATE_REFLOW_REGISTER));
            ESP_LOGI(TAG, "Mid reference/read %f/%f", _read_4_bytes(CALIBRATE_READMID_REGISTER), _read_4_bytes(CALIBRATE_REFMID_REGISTER));
            ESP_LOGI(TAG, "High reference/read %f/%f", _read_4_bytes(CALIBRATE_READHIGH_REGISTER), _read_4_bytes(CALIBRATE_REFHIGH_REGISTER));
            ESP_LOGI(TAG, "Single point %f", _read_4_bytes(CALIBRATE_SINGLE_OFFSET_REGISTER));
            ESP_LOGI(TAG, "Calibration temperature %f", _read_4_bytes(CALIBRATE_TEMPERATURE_REGISTER));
        }

        void Mod_pHSensor::update()
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

            _send_command(MEASURE_PH_TASK);
            this->set_timeout("data", wait, [this]()
                              { this->update_internal_(); });
        }

        void Mod_pHSensor::update_internal_()
        {
            uint8_t status;
            this->read_byte(STATUS_REGISTER, &status);
            if (status == STATUS_NO_ERROR)
            {
                float ms = _read_4_bytes(PH_REGISTER);
                ESP_LOGD(TAG, "'%s': Got pH=%.2f ", this->get_name().c_str(), ms);
                this->publish_state(ms);
            }
            else
            {
                switch (status)
                {
                case STATUS_SYSTEM_ERROR:
                    ESP_LOGE(TAG, "Error: Module not functioning properly");
                    break;
                case STATUS_OUTSIDE_LOWER:
                    ESP_LOGE(TAG, "Error: Measurement outside lower range");
                    break;
                case STATUS_OUTSIDE_UPPER:
                    ESP_LOGE(TAG, "Measurement outside upper range");
                    break;
                }
            }
        }

        void Mod_pHSensor::calibrateSingle(float calibration_ph)
        {
            ESP_LOGD(TAG, "Calibrate Single Pressed.", calibration_ph);
            _write_4_bytes(PH_REGISTER, calibration_ph);
            if (this->temperature_sensor != nullptr)
            {
                _write_4_bytes(CALIBRATE_TEMPERATURE_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                _write_4_bytes(CALIBRATE_TEMPERATURE_REGISTER, 25.0);
            }

            _send_command(CALIBRATE_SINGLE_TASK);
        }

        void Mod_pHSensor::calibrateLow(float calibration_ph)
        {
            ESP_LOGD(TAG, "Calibrate Low Pressed.", calibration_ph);
            _write_4_bytes(PH_REGISTER, calibration_ph);
            if (this->temperature_sensor != nullptr)
            {
                _write_4_bytes(CALIBRATE_TEMPERATURE_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                _write_4_bytes(CALIBRATE_TEMPERATURE_REGISTER, 25.0);
            }

            _send_command(CALIBRATE_LOW_TASK);
        }

        void Mod_pHSensor::calibrateMid(float calibration_ph)
        {
            ESP_LOGD(TAG, "Calibrate Mid Pressed.", calibration_ph);
            _write_4_bytes(PH_REGISTER, calibration_ph);
            if (this->temperature_sensor != nullptr)
            {
                _write_4_bytes(CALIBRATE_TEMPERATURE_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                _write_4_bytes(CALIBRATE_TEMPERATURE_REGISTER, 25.0);
            }

            _send_command(CALIBRATE_MID_TASK);
        }

        void Mod_pHSensor::calibrateHigh(float calibration_ph)
        {
            ESP_LOGD(TAG, "Calibrate High Pressed. %f", calibration_ph);
            _write_4_bytes(PH_REGISTER, calibration_ph);
            if (this->temperature_sensor != nullptr)
            {
                _write_4_bytes(CALIBRATE_TEMPERATURE_REGISTER, this->temperature_sensor->state);
            }
            else
            {
                _write_4_bytes(CALIBRATE_TEMPERATURE_REGISTER, 25.0);
            }

            _send_command(CALIBRATE_HIGH_TASK);
        }

        void Mod_pHSensor::calibrateReset()
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

        float Mod_pHSensor::get_setup_priority() const { return setup_priority::DATA; }

        void Mod_pHSensor::_write_4_bytes(uint8_t reg, float f)
        {
            uint8_t b[4];
            float f_val = f;

            b[0] = *((uint8_t *)&f_val);
            b[1] = *((uint8_t *)&f_val + 1);
            b[2] = *((uint8_t *)&f_val + 2);
            b[3] = *((uint8_t *)&f_val + 3);
            this->write_register(reg, b, 4);
        }

        void Mod_pHSensor::_send_command(uint8_t command)
        {
            this->write_byte(TASK_REGISTER, command);
        }

        float Mod_pHSensor::_read_4_bytes(uint8_t reg)
        {
            float f;
            uint8_t temp[4];

            this->write(&reg, 1);
            this->read(temp, 4);
            memcpy(&f, temp, sizeof(f));

            return f;
        }
    } // namespace mod_ph
} // namespace esphome
