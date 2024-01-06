import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.automation import Condition
from esphome import pins
from esphome import automation
from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID,
    CONF_FREQUENCY,
    CONF_RESET_PIN,
    CONF_DIO_PIN,
    CONF_CS_PIN,
)
from esphome.core import coroutine_with_priority, CORE

CONF_ON_SEND = "on_sent"

lora_mqtt_ns = cg.esphome_ns.namespace("lora_mqtt")
ESPLoraSendTrigger = lora_mqtt_ns.class_("ESPLoraSendTrigger", automation.Trigger.template())

Lora_MQTTComponent = lora_mqtt_ns.class_(
    "Lora_MQTTComponent", cg.Component
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Lora_MQTTComponent),
    cv.Optional(CONF_ON_SEND): automation.validate_automation(
        {
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ESPLoraSendTrigger),
        }
    ),
    cv.Optional(CONF_CS_PIN, default="GPIO18"): pins.gpio_output_pin_schema,
    cv.Optional(CONF_RESET_PIN, default="GPIO14"): pins.gpio_output_pin_schema,
    cv.Optional(CONF_DIO_PIN, default="GPIO26"): pins.gpio_output_pin_schema,
    cv.Optional(CONF_FREQUENCY, default=915000000): cv.positive_int,
    cv.Optional("bandwidth", default=125E3): cv.positive_int,
    cv.Optional("spread", default=7): cv.positive_int,
    cv.Optional("coding", default=5): cv.positive_int,
    cv.Optional("sync", default=0x12): cv.positive_int,
})

@coroutine_with_priority(40.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_CS_PIN in config:
        cs = await cg.gpio_pin_expression(config[CONF_CS_PIN])
        cg.add(var.set_cs_constant(cs))
    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_constant(reset))
    if CONF_DIO_PIN in config:
        dio0 = await cg.gpio_pin_expression(config[CONF_DIO_PIN])
        cg.add(var.set_dio0_constant(dio0))
    if "bandwidth" in config:
        cg.add(var.set_bandwidth_constant(config["bandwidth"]))
    if "spread" in config:
        cg.add(var.set_spread_constant(config["spread"]))
    if "coding" in config:
        cg.add(var.set_coding_constant(config["coding"]))
    if "sync" in config:
        cg.add(var.set_sync_constant(config["sync"]))
        
    cg.add(var.set_frequency_constant(config[CONF_FREQUENCY]))

    for conf in config.get(CONF_ON_SEND, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(float, "x")], conf)