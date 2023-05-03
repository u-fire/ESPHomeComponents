#include "mlx90614.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace mlx90614
    {

        static const char *const TAG = "mlx90614.sensor";

        void MLX90614Sensor::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up MLX90614...");
            if (this->write(&MLX90614_TOBJ1, 1) != i2c::ERROR_OK)
            {
                ESP_LOGE(TAG, "MLX90614 not found");
                this->mark_failed();
                return;
            }

            ESP_LOGI(TAG, "Found MLX90614");
        }

        void MLX90614Sensor::dump_config()
        {
            LOG_SENSOR("", "MLX90614", this);
            LOG_I2C_DEVICE(this);
            if (this->is_failed())
            {
                ESP_LOGE(TAG, "Communication with MLX90614 failed!");
            }

            LOG_UPDATE_INTERVAL(this);
        }

        void MLX90614Sensor::update()
        {
            uint8_t buffer[2];
            uint16_t temp16;
            float temp;

            if (this->ambient_temperature_sensor_ != nullptr)
            {
                this->write(&MLX90614_TA, 1, false);
                this->read(buffer, 2);
                temp16 = uint16_t(buffer[0]) | (uint16_t(buffer[1]) << 8);
                temp = .02 * temp16;
                temp = temp - 273.15;

                ESP_LOGD(TAG, "Got ambient temperature=%.2f °C", temp);
                this->ambient_temperature_sensor_->publish_state(temp);
            }
            
            if (this->object_temperature_sensor_ != nullptr)
            {
                this->write(&MLX90614_TOBJ1, 1, false);
                this->read(buffer, 2);
                temp16 = uint16_t(buffer[0]) | (uint16_t(buffer[1]) << 8);
                temp = .02 * temp16;
                temp = temp - 273.15;

                ESP_LOGD(TAG, "Got object temperature=%.2f °C", temp);
                this->object_temperature_sensor_->publish_state(temp);
            }
        }

        float MLX90614Sensor::get_setup_priority() const { return setup_priority::DATA; }

    } // namespace mlx90614
} // namespace esphome
