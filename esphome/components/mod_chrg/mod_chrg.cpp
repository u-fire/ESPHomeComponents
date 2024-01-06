#include "mod_chrg.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace mod_chrg
    {
        static const char *const TAG = "mod_chrg.sensor";
        static const std::string CHARGE_STATES[4] = {"discharging", "trickle", "charging", "top-off"};
        static const std::string TS_STATES[8] = {"normal", "cold", "hot", "cool", "warm", "precool", "prewarm", "fault"};

        void Mod_CHRGSensor::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up Mod-CHRG...");

            uint8_t data = _read_byte(ADC_CONTROL);
            data |= (1UL << ADC_EN);         // enable ADC
            data |= (1UL << ADC_AVG);        // average readings
            data &= ~(1ULL << ADC_RATE);     // continuous samples
            data &= ~(1ULL << ADC_SAMPLE_1); // 12 bit
            data &= ~(1ULL << ADC_SAMPLE_2); // 12 bit
            _write_byte(ADC_CONTROL, data);

            data = _read_byte(0x14);
            data |= (1UL << 5);

            _write_byte(0x14, data);
        }

        void Mod_CHRGSensor::update()
        {
            float _adc = 0;

            if (this->state_sensor != nullptr)
            {
                uint8_t data = _read_byte(CHARGER_STATUS_1);
                data = (data & 0b00011000) >> 3;
                this->state_sensor->publish_state(CHARGE_STATES[data].c_str());
            }

            uint8_t fault = _read_byte(FAULT_STATUS);
            if (this->ts_sensor != nullptr)
            {
                fault = (fault & 0b00000111); // temperature status in last 3 bytes
                this->ts_sensor->publish_state(TS_STATES[fault].c_str());
            }

            if (this->vbus_fault_sensor != nullptr)
            {
                this->vbus_fault_sensor->publish_state((fault & 0b10000000) >> VBUS_FAULT_STAT ? true : false);
            }

            if (this->vbat_fault_sensor != nullptr)
            {
                this->vbat_fault_sensor->publish_state((fault & 0b01000000) >> BAT_FAULT_STAT ? true : false);
            }

            if (this->vsys_fault_sensor != nullptr)
            {
                this->vsys_fault_sensor->publish_state((fault & 0b00100000) >> SYS_FAULT_STAT ? true : false);
            }

            if (this->tshut_fault_sensor != nullptr)
            {
                this->tshut_fault_sensor->publish_state((fault & 0b00001000) >> TSHUT_STAT ? true : false);
            }

            if (this->vbat_sensor != nullptr)
            {
                _adc = _getVBAT();
                this->vbat_sensor->publish_state(_adc / 1000.0);
            }

            if (this->vsys_sensor != nullptr)
            {
                _adc = _getVSYS();
                this->vsys_sensor->publish_state(_adc / 1000.0);
            }

            if (this->vbus_sensor != nullptr)
            {
                _adc = _getVBUS();
                this->vbus_sensor->publish_state(_adc / 1000.0);
            }

            if (this->vpmid_sensor != nullptr)
            {
                _adc = _getVPMID();
                this->vpmid_sensor->publish_state(_adc / 1000.0);
            }

            if (this->ibat_sensor != nullptr)
            {
                _adc = _getIBAT();
                this->ibat_sensor->publish_state(_adc);
            }

            if (this->ibus_sensor != nullptr)
            {
                _adc = _getIBUS();
                this->ibus_sensor->publish_state(_adc);
            }

            if (this->tdie_sensor != nullptr)
            {
                _adc = _getTDIE();
                this->tdie_sensor->publish_state(_adc);
            }

            _resetWatchdog();
        }

        float Mod_CHRGSensor::get_setup_priority() const { return setup_priority::DATA; }

        void Mod_CHRGSensor::dump_config()
        {
            // ESP_LOGI(TAG, "Single point %f", _read_4_bytes(CALIBRATE_SINGLE_OFFSET_REGISTER));
        }

        float Mod_CHRGSensor::_getIBAT()
        {
            int16_t data = _read_int(IBAT_ADC);
            data = data >> 2;
            data = data * 4;
            return data / 1000.0;
        }

        float Mod_CHRGSensor::_getIBUS()
        {
            int16_t data = _read_int(IBUS_ADC);
            data = data >> 1;
            data = data * 2;
            return data / 1000.0;
        }

        float Mod_CHRGSensor::_getTDIE()
        {
            int16_t data = _read_int(TDIE_ADC);
            float f_data = 0.0;
            data &= ~(1ULL << (15));
            data &= ~(1ULL << (14));
            data &= ~(1ULL << (13));
            data &= ~(1ULL << (12));
            f_data = (data * 0.5);
            return f_data;
        }

        void Mod_CHRGSensor::_resetWatchdog()
        {
            uint8_t data = _read_byte(CHARGER_CONTROL);
            data |= (1UL << WD_RST);
            _write_byte(CHARGER_CONTROL, data);
        }

        float Mod_CHRGSensor::_getVBAT()
        {
            int16_t data = _read_int(VBAT_ADC);
            float f_data = 0.0;
            data &= ~(1ULL << (15)); // reserved
            data &= ~(1ULL << (14)); // reserved
            data &= ~(1ULL << (13)); // reserved
            data = data >> 1;        // bit-0 reserved
            f_data = (data * 1.99);
            return f_data;
        }

        float Mod_CHRGSensor::_getVSYS()
        {
            int16_t data = _read_int(VSYS_ADC);
            float f_data = 0.0;
            data &= ~(1ULL << (15)); // reserved
            data &= ~(1ULL << (14)); // reserved
            data &= ~(1ULL << (13)); // reserved
            data = data >> 1;        // bit-0 reserved
            f_data = (data * 1.99);
            return f_data;
        }

        float Mod_CHRGSensor::_getVBUS()
        {
            int16_t data = _read_int(VBUS_ADC);
            float f_data = 0.0;
            data &= ~(1ULL << (15)); // reserved
            data = data >> 2;        // bits 1, 0 reserved
            f_data = (data * 3.97);
            return f_data;
        }

        float Mod_CHRGSensor::_getVPMID()
        {
            int16_t data = _read_int(VPMID_ADC);
            float f_data = 0.0;
            data &= ~(1ULL << (15));
            data &= ~(1ULL << (14));
            data &= ~(1ULL << (13));
            data = data >> 2;
            f_data = (data * 3.97);
            return f_data;
        }

        void Mod_CHRGSensor::_write_byte(uint8_t reg, uint8_t command)
        {
            this->write_byte(reg, command);
        }

        uint8_t Mod_CHRGSensor::_read_byte(uint8_t reg)
        {
            float f;
            uint8_t data;

            this->write(&reg, 1);
            this->read(&data, 1);

            return data;
        }

        uint16_t Mod_CHRGSensor::_read_int(uint8_t reg)
        {
            uint8_t data = 0;
            uint8_t data1 = 0;

            this->write(&reg, 1);
            this->read(&data, 1);
            this->read(&data1, 1);

            return (data1 << 8) + data;
        }

    } // namespace mod_chrg
} // namespace esphome
