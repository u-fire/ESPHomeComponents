import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, mqtt
from esphome.const import (
    CONF_ID,
)
from esphome.core import coroutine_with_priority, CORE

now_mqtt_bridge_ns = cg.esphome_ns.namespace("now_mqtt_bridge")

Now_MQTT_BridgeComponent = now_mqtt_bridge_ns.class_(
    "Now_MQTT_BridgeComponent", cg.Component
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Now_MQTT_BridgeComponent),
})


@coroutine_with_priority(1.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

