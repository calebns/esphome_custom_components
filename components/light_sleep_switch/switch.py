import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch

light_sleep_switch_ns = cg.esphome_ns.namespace("light_sleep_switch")
LightSleepSwitch = light_sleep_switch_ns.class_("LightSleepSwitch", switch.Switch, cg.Component)

CONF_LSS_CPU_MIN_FREQ_KEY = 'cpu_min_freq'
CONF_LSS_CPU_MAX_FREQ_KEY = 'cpu_max_freq'

#CONFIG_SCHEMA = switch.switch_schema(LightSleepSwitch).extend(cv.COMPONENT_SCHEMA)

WM_CONFIG_SCHEMA = cv.Schema({
  cv.Optional(CONF_LSS_CPU_MIN_FREQ_KEY, default=40): cv.int_,
  cv.Optional(CONF_LSS_CPU_MAX_FREQ_KEY, default=80): cv.int_,
}).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = switch.switch_schema(LightSleepSwitch).extend(WM_CONFIG_SCHEMA)

async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)

    cg.add(var.set_lss_cpu_min_freq_key(config[CONF_LSS_CPU_MIN_FREQ_KEY]))
    cg.add(var.set_lss_cpu_max_freq_key(config[CONF_LSS_CPU_MAX_FREQ_KEY]))
