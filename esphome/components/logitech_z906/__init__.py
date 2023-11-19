import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_UART_ID
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["select"]
MULTI_CONF = True

CONF_LOGITECH_Z906_ID = "logitech_z906_id"

CONF_AMPLIFIER_UART_ID = "amplifier_uart_id"
CONF_CONSOLE_UART_ID = "console_uart_id"

logitech_z906_ns = cg.esphome_ns.namespace("logitech_z906")
LogitechZ906Component = logitech_z906_ns.class_(
    "LogitechZ906Component", cg.Component
)

LOGITECH_Z906_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_LOGITECH_Z906_ID): cv.use_id(LogitechZ906Component),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(CONF_ID): cv.declare_id(LogitechZ906Component),
        cv.Required(CONF_AMPLIFIER_UART_ID): cv.use_id(uart.UARTComponent),
        cv.Required(CONF_CONSOLE_UART_ID): cv.use_id(uart.UARTComponent),
        })
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    amplifier_uart = await cg.get_variable(config[CONF_AMPLIFIER_UART_ID])
    console_uart = await cg.get_variable(config[CONF_CONSOLE_UART_ID])
    cg.add(var.set_amplifier_uart_parent(amplifier_uart))
    cg.add(var.set_console_uart_parent(console_uart))

