import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from .. import LOGITECH_Z906_COMPONENT_SCHEMA, CONF_LOGITECH_Z906_ID, logitech_z906_ns

CONF_MASTER_VOLUME = "master_volume"
CONF_BASS_VOLUME = "bass_volume"
CONF_CENTER_VOLUME = "center_volume"
CONF_REAR_VOLUME = "rear_volume"

VOLUMES = [CONF_MASTER_VOLUME, CONF_BASS_VOLUME, CONF_CENTER_VOLUME, CONF_REAR_VOLUME]


LogitechZ906Number = logitech_z906_ns.class_(
    "LogitechZ906Number", number.Number, cg.Component
)

CONFIG_SCHEMA = LOGITECH_Z906_COMPONENT_SCHEMA.extend(
    {
        cv.Required(volume_type): number.number_schema(LogitechZ906Number).extend(
            cv.COMPONENT_SCHEMA
        )
        for volume_type in VOLUMES
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LOGITECH_Z906_ID])
    for volume_type in VOLUMES:
        if volume_type in config:
            var = await number.new_number(
                config[volume_type], min_value=0.0, max_value=43.0, step=1.0
            )
            await cg.register_component(var, config[volume_type])
            cg.add(var.set_number_type(volume_type))
            cg.add(var.set_parent(parent))
            cg.add(var.registrate())
