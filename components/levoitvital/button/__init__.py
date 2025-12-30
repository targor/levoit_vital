import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import ICON_AIR_FILTER
from .. import LevoitVital, CONF_LEVOIT_ID, levoit_vital_ns

DEPENDENCIES = ["levoitvital"]

CONF_POWERMODE = "powermode"

LevoitButton = levoit_vital_ns.class_("LevoitButton", button.Button, cg.Component)
LevoitButtonPurpose = levoit_vital_ns.enum("LevoitButtonPurpose")

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LEVOIT_ID): cv.use_id(LevoitVital),
        cv.Optional(CONF_POWERMODE): button.button_schema(
            LevoitButton, icon=ICON_AIR_FILTER
        ).extend(cv.COMPONENT_SCHEMA),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_LEVOIT_ID])

    if config_powermode := config.get(CONF_POWERMODE):
        var = await button.new_button(
            config_powermode,
            parent,
            LevoitButtonPurpose.POWERMODE
        )
        cg.add(parent.set_button(var, LevoitButtonPurpose.POWERMODE))
        await cg.register_component(var, config_powermode)
