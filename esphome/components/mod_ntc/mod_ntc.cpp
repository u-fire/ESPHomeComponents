#include "mod_ntc.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace mod_ntc
    {
        static const char *const TAG = "mod_ntc.sensor";

        void Mod_NTCSensor::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up Mod-NTC...");

            uint8_t version;
            if (!this->read_byte(HW_VERSION_REGISTER, &version) && version != 0xFF)
            {
                this->mark_failed();
                return;
            }
            ESP_LOGI(TAG, "Found Mod-NTC version %d", version);
        }

        void Mod_NTCSensor::dump_config()
        {
            ESP_LOGI(TAG, "Beta %f", _read_4_bytes(BETA_REGISTER));
        }

        void Mod_NTCSensor::update()
        {
            int wait = 150;
            
            _send_command(MEASURE_TEMP_TASK);
            this->set_timeout("data", wait, [this]()
                              { this->update_internal_(); });
        }

        void Mod_NTCSensor::update_internal_()
        {
            uint8_t status;
            this->read_byte(STATUS_REGISTER, &status);
            if (status == STATUS_NO_ERROR)
            {
                float ms = _read_4_bytes(TEMP_C_REGISTER);
                ESP_LOGD(TAG, "'%s': Got temperature=%.2f °C", this->get_name().c_str(), ms);
                this->publish_state(ms);
            }
            else
            {
                switch (status)
                {
                case STATUS_SYSTEM_ERROR:
                    ESP_LOGE(TAG, "Error: module not functioning properly");
                    break;
                case STATUS_NO_PROBE:
                    ESP_LOGE(TAG, "Error: not probe connected");
                    break;
                }
            }
        }

        float Mod_NTCSensor::setBeta(float beta)
        {
            ESP_LOGD(TAG, "New beta value %f", beta);
            _write_4_bytes(BETA_REGISTER, beta);
            _send_command(BETA_TASK);

            return _read_4_bytes(BETA_REGISTER);
        }

        float Mod_NTCSensor::getBeta()
        {
            // ESP_LOGD(TAG, "Beta value %f", beta);
            return _read_4_bytes(BETA_REGISTER);
        }

        void Mod_NTCSensor::reset()
        {
            ESP_LOGD(TAG, "Beta Reset Pressed.");
            _write_4_bytes(BETA_REGISTER, 3799.0);
            _send_command(BETA_TASK);
        }

        float Mod_NTCSensor::get_setup_priority() const { return setup_priority::DATA; }

        void Mod_NTCSensor::_write_4_bytes(uint8_t reg, float f)
        {
            uint8_t b[4];
            float f_val = f;

            b[0] = *((uint8_t *)&f_val);
            b[1] = *((uint8_t *)&f_val + 1);
            b[2] = *((uint8_t *)&f_val + 2);
            b[3] = *((uint8_t *)&f_val + 3);
            this->write_register(reg, b, 4);
        }

        void Mod_NTCSensor::_send_command(uint8_t command)
        {
            this->write_byte(TASK_REGISTER, command);
        }

        float Mod_NTCSensor::_read_4_bytes(uint8_t reg)
        {
            float f;
            uint8_t temp[4];

            this->write(&reg, 1);
            this->read(temp, 4);
            memcpy(&f, temp, sizeof(f));

            return f;
        }
    } // namespace mod_ntc
} // namespace esphome
