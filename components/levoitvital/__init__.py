import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, switch
from esphome.const import CONF_ID

DEPENDENCIES = ["uart", "sensor", "text_sensor", "switch", "select","button"]
CODEOWNERS = ["@Targor"]
CONF_LEVOIT_MODEL = "model"
CONF_LABELS = "labels"
VALID_MODELS = ["VITAL200S"]
CONF_LEVOIT_ID = "levoitvital"

# Namespace
levoit_vital_ns = cg.esphome_ns.namespace("levoit_vital")

# Create main class
LevoitVital = levoit_vital_ns.class_("LevoitVital", cg.Component, uart.UARTDevice)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LevoitVital),
            cv.Optional(CONF_LEVOIT_MODEL, default="VITAL200S"): cv.All(
                cv.string, cv.one_of(*VALID_MODELS)
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


# Übersetzt YAML-Konfiguration in C++-Instanz
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_LEVOIT_MODEL in config:
        cg.add(var.set_device_model(config[CONF_LEVOIT_MODEL]))
