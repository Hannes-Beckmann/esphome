import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from .. import LOGITECH_Z906_COMPONENT_SCHEMA, CONF_LOGITECH_Z906_ID, logitech_z906_ns

CONF_SOURCE = "source_selection"
LogitechZ906Select = logitech_z906_ns.class_(
    "LogitechZ906Select", select.Select, cg.Component
)

CONFIG_SCHEMA = LOGITECH_Z906_COMPONENT_SCHEMA.extend(
    {
        cv.Required(CONF_SOURCE): select.select_schema(
            LogitechZ906Select, options=["Aux, Optical"]
        ).extend(cv.COMPONENT_SCHEMA)
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LOGITECH_Z906_ID])
    var = await select.new_select(config[CONF_SOURCE])
    await cg.register_component(var, config[CONF_SOURCE])
    cg.add(parent.set_source_select(var))
    cg.add(var.set_parent(parent))
