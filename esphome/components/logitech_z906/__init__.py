import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["select"]
MULTI_CONF = True

CONF_LOGITECH_Z906_ID = "logitech_z906_id"

logitech_z906_ns = cg.esphome_ns.namespace("logitech_z906")
LogitechZ906Component = logitech_z906_ns.class_(
    "LogitechZ906Component", cg.Component, uart.UARTDevice
)

LOGITECH_Z906_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_LOGITECH_Z906_ID): cv.use_id(LogitechZ906Component),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.Schema({cv.GenerateID(): cv.declare_id(LogitechZ906Component)})
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
