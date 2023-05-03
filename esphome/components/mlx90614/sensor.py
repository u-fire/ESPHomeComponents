import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    STATE_CLASS_MEASUREMENT,
    DEVICE_CLASS_TEMPERATURE,
    UNIT_CELSIUS,
    CONF_ID
)

DEPENDENCIES = ["i2c"]

mlx90614_ns = cg.esphome_ns.namespace("mlx90614")
MLX90614Sensor = mlx90614_ns.class_(
    "MLX90614Sensor", cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MLX90614Sensor),
            cv.Optional("ambient"): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional("object"): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x5A))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if "ambient" in config:
        conf = config["ambient"]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_ambient_temperature_sensor(sens))

    if "object" in config:
        conf = config["object"]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_object_temperature_sensor(sens))
