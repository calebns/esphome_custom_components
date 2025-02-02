import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch

light_sleep_switch_ns = cg.esphome_ns.namespace("light_sleep_switch")
LightSleepSwitch = light_sleep_switch_ns.class_("LightSleepSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = switch.switch_schema(LightSleepSwitch).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)