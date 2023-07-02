#pragma once

#include "esphome/core/component.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "esphome/core/hal.h"
#include "esp_wifi.h"

namespace esphome
{
    namespace lora_mqtt_bridge
    {
        class Lora_MQTT_BridgeComponent : public Component
        {
        public:
            void setup() override;
            void loop() override;
            void process_line();
            float get_setup_priority() const override;
            void set_cs_constant(GPIOPin *constant) { this->_cs = constant; }
            void set_reset_constant(GPIOPin *constant) { this->_reset = constant; }
            void set_dio0_constant(GPIOPin *constant) { this->_dio0 = constant; }
            void set_frequency_constant(long constant) { this->_frequency = constant; }
            
        private:
            GPIOPin *_cs{0};
            GPIOPin *_reset{0};
            GPIOPin *_dio0{0};
            long _frequency{0};
            void split(char **argv, int *argc, char *string, const char delimiter, int allowempty);
        };
    } // namespace lora_mqtt_bridge
} // namespace esphome
