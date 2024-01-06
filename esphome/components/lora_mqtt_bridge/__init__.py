import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c, mqtt
from esphome.const import (
    CONF_ID,
    CONF_FREQUENCY,
    CONF_RESET_PIN,
    CONF_DIO_PIN,
    CONF_CS_PIN,
)

from esphome.core import coroutine_with_priority, CORE

lora_mqtt_bridge_ns = cg.esphome_ns.namespace("lora_mqtt_bridge")

Lora_MQTT_BridgeComponent = lora_mqtt_bridge_ns.class_(
    "Lora_MQTT_BridgeComponent", cg.Component,
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Lora_MQTT_BridgeComponent),
    cv.Optional(CONF_CS_PIN, default="GPIO18"): pins.gpio_output_pin_schema,
    cv.Optional(CONF_RESET_PIN, default="GPIO14"): pins.gpio_output_pin_schema,
    cv.Optional(CONF_DIO_PIN, default="GPIO26"): pins.gpio_output_pin_schema,
    cv.Optional(CONF_FREQUENCY, default=915000000): cv.positive_int,
    cv.Optional("bandwidth", default=125E3): cv.positive_int,
    cv.Optional("spread", default=7): cv.positive_int,
    cv.Optional("coding", default=5): cv.positive_int,
    cv.Optional("sync", default=0x12): cv.positive_int,
})

@coroutine_with_priority(1.0)
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
