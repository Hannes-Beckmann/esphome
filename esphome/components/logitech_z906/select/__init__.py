import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from .. import LOGITECH_Z906_COMPONENT_SCHEMA, CONF_LOGITECH_Z906_ID, logitech_z906_ns

CONF_SOURCE = "source"
CONF_EFFECT = "effect"

TYPES = [CONF_SOURCE, CONF_EFFECT]

OPTIONS = {
    CONF_SOURCE: ["Line", "Chinch", "Optical1", "Optical2", "Coaxial", "Aux"],
    CONF_EFFECT: ["3D", "2.1", "4.1", "None"],
}

LogitechZ906Select = logitech_z906_ns.class_(
    "LogitechZ906Select", select.Select, cg.Component
)

CONFIG_SCHEMA = LOGITECH_Z906_COMPONENT_SCHEMA.extend(
    {
        cv.Required(type): select.select_schema(LogitechZ906Select).extend(
            cv.COMPONENT_SCHEMA
        )
        for type in TYPES
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LOGITECH_Z906_ID])
    for type in TYPES:
        if type in config:
            var = await select.new_select(config[type], options=OPTIONS[type])
            await cg.register_component(var, config[type])
            cg.add(var.set_select_type(type))
            cg.add(var.set_parent(parent))
            cg.add(var.registrate())
