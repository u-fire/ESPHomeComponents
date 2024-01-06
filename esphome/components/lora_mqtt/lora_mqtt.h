#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/automation.h"
#include "esphome/core/hal.h"

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

namespace esphome
{
    namespace lora_mqtt
    {
        class Lora_MQTTComponent : public Component
        {
        public:
            void setup() override;
            void add_on_state_callback(std::function<void(float)> &&callback) { this->callback_.add(std::move(callback)); }
            void set_cs_constant(GPIOPin *constant) { this->_cs = constant; }
            void set_reset_constant(GPIOPin *constant) { this->_reset = constant; }
            void set_dio0_constant(GPIOPin *constant) { this->_dio0 = constant; }
            void set_frequency_constant(long constant) { this->_frequency = constant; }
            void set_bandwidth_constant(long constant) { this->_bandwidth = constant; }
            void set_spread_constant(long constant) { this->_spread = constant; }
            void set_coding_constant(long constant) { this->_coding = constant; }
            void set_sync_constant(long constant) { this->_sync = constant; }

        private:
            CallbackManager<void(float)> callback_;
            CallbackManager<void(std::string)> callback_text_;
            void on_sensor_update(sensor::Sensor *obj, float state);
#ifdef USE_BINARY_SENSOR
            void on_binary_sensor_update(binary_sensor::BinarySensor *obj, float state);
#endif
#ifdef USE_TEXT_SENSOR
            void on_text_sensor_update(text_sensor::TextSensor *obj, std::string state);
#endif
            GPIOPin *_cs{0};
            GPIOPin *_reset{0};
            GPIOPin *_dio0{0};
            long _frequency{0};
            long _bandwidth{0};
            long _spread{0};
            long _coding{0};
            long _sync{0};
        };

        class ESPLoraSendTrigger : public Trigger<float>
        {
        public:
            explicit ESPLoraSendTrigger(Lora_MQTTComponent *parent)
            {
                parent->add_on_state_callback([this](float value)
                                              { this->trigger(value); });
            }
        };

    } // namespace lora_mqtt
} // namespace esphome
