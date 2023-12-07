#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome
{
    namespace mod_chrg
    {
        static const uint8_t ADC_CONTROL = 0x26;
        static const uint8_t NTC_CONTROL = 0x1A;
        static const uint8_t CHARGER_CONTROL = 0x16;
        static const uint8_t CHARGER_STATUS_0 = 0x1d;
        static const uint8_t CHARGER_STATUS_1 = 0x1e;
        static const uint8_t FAULT_STATUS = 0x1f;
        static const uint8_t VBAT_ADC = 0x30;
        static const uint8_t VSYS_ADC = 0x32;
        static const uint8_t VBUS_ADC = 0x2c;
        static const uint8_t VPMID_ADC = 0x2E;
        static const uint8_t TDIE_ADC = 0x36;
        static const uint8_t IBAT_ADC = 0x2A;
        static const uint8_t IBUS_ADC = 0x28;

        static const uint8_t WD_RST = 2;
        static const uint8_t ADC_EN = 7;
        static const uint8_t ADC_RATE = 6;
        static const uint8_t ADC_SAMPLE_1 = 5;
        static const uint8_t ADC_SAMPLE_2 = 4;
        static const uint8_t ADC_AVG = 3;
        static const uint8_t VBUS_FAULT_STAT = 7;
        static const uint8_t BAT_FAULT_STAT = 6;
        static const uint8_t SYS_FAULT_STAT = 5;
        static const uint8_t TSHUT_STAT = 3;
        static const uint8_t TS_IGNORE = 7;

        /// This class implements support for the i2c Microfire Mod-CHRG battery management system based on the BQ25628E.
        class Mod_CHRGSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice
        {
        public:
            void setup() override;
            void dump_config() override;
            void update() override;
            float get_setup_priority() const override;
            void set_vbat_sensor(sensor::Sensor *_sensor) { vbat_sensor = _sensor; }
            void set_vsys_sensor(sensor::Sensor *_sensor) { vsys_sensor = _sensor; }
            void set_vbus_sensor(sensor::Sensor *_sensor) { vbus_sensor = _sensor; }
            void set_vpmid_sensor(sensor::Sensor *_sensor) { vpmid_sensor = _sensor; }
            void set_ibat_sensor(sensor::Sensor *_sensor) { ibat_sensor = _sensor; }
            void set_ibus_sensor(sensor::Sensor *_sensor) { ibus_sensor = _sensor; }
            void set_tdie_sensor(sensor::Sensor *_sensor) { tdie_sensor = _sensor; }
            void set_state_sensor(text_sensor::TextSensor *_sensor) { state_sensor = _sensor; }
            void set_ts_sensor(text_sensor::TextSensor *_sensor) { ts_sensor = _sensor; }
            void set_vbus_fault_sensor(binary_sensor::BinarySensor *_sensor) { vbus_fault_sensor = _sensor; }
            void set_vbat_fault_sensor(binary_sensor::BinarySensor *_sensor) { vbat_fault_sensor = _sensor; }
            void set_vsys_fault_sensor(binary_sensor::BinarySensor *_sensor) { vsys_fault_sensor = _sensor; }
            void set_tshut_fault_sensor(binary_sensor::BinarySensor *_sensor) { tshut_fault_sensor = _sensor; }

        protected:
            sensor::Sensor *vbat_sensor{nullptr};
            sensor::Sensor *vsys_sensor{nullptr};
            sensor::Sensor *vbus_sensor{nullptr};
            sensor::Sensor *vpmid_sensor{nullptr};
            sensor::Sensor *ibat_sensor{nullptr};
            sensor::Sensor *ibus_sensor{nullptr};
            sensor::Sensor *tdie_sensor{nullptr};
            text_sensor::TextSensor *state_sensor{nullptr};
            text_sensor::TextSensor *ts_sensor{nullptr};
            binary_sensor::BinarySensor *vbus_fault_sensor{nullptr};
            binary_sensor::BinarySensor *vbat_fault_sensor{nullptr};
            binary_sensor::BinarySensor *vsys_fault_sensor{nullptr};
            binary_sensor::BinarySensor *tshut_fault_sensor{nullptr};
            

            void _send_command(uint8_t command);
            uint8_t _read_byte(uint8_t reg);
            uint16_t _read_int(uint8_t reg);
            void _write_byte(uint8_t reg, uint8_t command);
            void _resetWatchdog();
            float _getVBAT();
            float _getIBAT();
            float _getIBUS();
            float _getTDIE();
            float _getVSYS();
            float _getVBUS();
            float _getVPMID();
        };

    } // namespace mod_chrg
} // namespace esphome
