import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    STATE_CLASS_MEASUREMENT,
    DEVICE_CLASS_TEMPERATURE
)

DEPENDENCIES = ["i2c"]
CONF_BETA = "beta"

mod_ntc_ns = cg.esphome_ns.namespace("mod_ntc")

Mod_NTCSensor = mod_ntc_ns.class_(
    "Mod_NTCSensor", sensor.Sensor, cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        Mod_NTCSensor,
        unit_of_measurement="°C",
        accuracy_decimals=1,
        state_class=STATE_CLASS_MEASUREMENT,
        device_class=DEVICE_CLASS_TEMPERATURE,
        icon="mdi:fire"
    )
    .extend(
        {
            cv.Optional(CONF_BETA, default=3977): cv.float_range(
                min=3000, max=5000
            )
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x0c))
)

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_beta(config[CONF_BETA]))