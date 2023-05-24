import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
)
from esphome.core import coroutine_with_priority, CORE

now_mqtt_ns = cg.esphome_ns.namespace("now_mqtt")

Now_MQTTComponent = now_mqtt_ns.class_(
    "Now_MQTTComponent", cg.Component
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Now_MQTTComponent),
})

@coroutine_with_priority(40.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

