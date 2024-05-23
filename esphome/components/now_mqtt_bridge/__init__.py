import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, mqtt
from esphome.const import (
    CONF_ID,
)
from esphome.core import coroutine_with_priority, CORE

CONF_CHANNEL = "wifi_channel"

now_mqtt_bridge_ns = cg.esphome_ns.namespace("now_mqtt_bridge")

Now_MQTT_BridgeComponent = now_mqtt_bridge_ns.class_(
    "Now_MQTT_BridgeComponent", cg.Component
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Now_MQTT_BridgeComponent),
    cv.Optional(CONF_CHANNEL, default=1): cv.int_range(1, 14),
})


@coroutine_with_priority(1.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_wifi_channel(config[CONF_CHANNEL]))
    await cg.register_component(var, config)

    cg.add_define("ESPNOW_CHANNEL", config[CONF_CHANNEL])
