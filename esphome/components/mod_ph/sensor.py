import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    STATE_CLASS_MEASUREMENT,
    UNIT_PH,
)

DEPENDENCIES = ["i2c"]
CONF_TEMPERATURE_SENSOR = "temperature_sensor"

mod_ph_ns = cg.esphome_ns.namespace("mod_ph")

Mod_pHSensor = mod_ph_ns.class_(
    "Mod_pHSensor", sensor.Sensor, cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        Mod_pHSensor,
        unit_of_measurement=UNIT_PH,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
        icon="mdi:ph"
    )
    .extend(
        {
            cv.Exclusive(CONF_TEMPERATURE_SENSOR, "temperature"): cv.use_id(
                sensor.Sensor
            )
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x0b))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_TEMPERATURE_SENSOR in config:
        sens = await cg.get_variable(config[CONF_TEMPERATURE_SENSOR])
        cg.add(var.set_temperature_sensor_external(sens))
