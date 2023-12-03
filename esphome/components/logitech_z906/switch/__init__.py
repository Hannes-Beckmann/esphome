import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from .. import LOGITECH_Z906_COMPONENT_SCHEMA, CONF_LOGITECH_Z906_ID, logitech_z906_ns

CONF_MUTE = "mute"
CONF_POWER = "power"
CONF_STANDBY = "standby"

TYPES = [CONF_MUTE, CONF_POWER, CONF_STANDBY]

LogitechZ906Switch = logitech_z906_ns.class_(
    "LogitechZ906Switch", switch.Switch, cg.Component
)

CONFIG_SCHEMA = LOGITECH_Z906_COMPONENT_SCHEMA.extend(
    {
        cv.Required(type): switch.switch_schema(LogitechZ906Switch).extend(
            cv.COMPONENT_SCHEMA
        )
        for type in TYPES
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LOGITECH_Z906_ID])
    for type in TYPES:
        if type in config:
            var = await switch.new_switch(config[type])
            await cg.register_component(var, config[type])
            cg.add(var.set_switch_type(type))
            cg.add(var.set_parent(parent))
            cg.add(var.registrate())
