import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    STATE_CLASS_MEASUREMENT,
    UNIT_MILLISIEMENS_PER_CENTIMETER,
)

DEPENDENCIES = ["i2c"]
CONF_TEMPERATURE_SENSOR = "temperature_sensor"
CONF_TEMPERATURE_CONSTANT = "temperature_constant"
CONF_TEMPERATURE_COEFFICIENT = "temperature_coefficient"

mod_ec_ns = cg.esphome_ns.namespace("mod_ec")

Mod_ECSensor = mod_ec_ns.class_(
    "Mod_ECSensor", sensor.Sensor, cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        Mod_ECSensor,
        unit_of_measurement=UNIT_MILLISIEMENS_PER_CENTIMETER,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
        icon="mdi:sprout"
    )
    .extend(
        {
            cv.Exclusive(CONF_TEMPERATURE_SENSOR, "temperature"): cv.use_id(
                sensor.Sensor
            ),
            cv.Optional(CONF_TEMPERATURE_CONSTANT, default=25.0): cv.temperature,
            cv.Optional(CONF_TEMPERATURE_COEFFICIENT, default=0.019): cv.float_range(
                min=0.01, max=0.05
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x0a))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_temperature_constant(config[CONF_TEMPERATURE_CONSTANT]))
    cg.add(var.set_temperature_coefficient(config[CONF_TEMPERATURE_COEFFICIENT]))

    if CONF_TEMPERATURE_SENSOR in config:
        sens = await cg.get_variable(config[CONF_TEMPERATURE_SENSOR])
        cg.add(var.set_temperature_sensor_external(sens))
