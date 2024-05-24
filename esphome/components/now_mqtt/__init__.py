import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.automation import Condition
from esphome import automation
from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID
)
from esphome.core import coroutine_with_priority, CORE

CONF_CHANNEL = "wifi_channel"
CONF_ON_SEND = "on_sent"

now_mqtt_ns = cg.esphome_ns.namespace("now_mqtt")
ESPNowSendTrigger = now_mqtt_ns.class_("ESPNowSendTrigger", automation.Trigger.template())

Now_MQTTComponent = now_mqtt_ns.class_(
    "Now_MQTTComponent", cg.Component
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Now_MQTTComponent),
    cv.Optional(CONF_CHANNEL, default=1): cv.int_range(1, 14),
    cv.Optional(CONF_ON_SEND): automation.validate_automation(
        {
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ESPNowSendTrigger),
        }
    ),
})


@coroutine_with_priority(40.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_wifi_channel(config[CONF_CHANNEL]))
    await cg.register_component(var, config)

    for conf in config.get(CONF_ON_SEND, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(float, "x")], conf)