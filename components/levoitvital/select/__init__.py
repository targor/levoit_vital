import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import ICON_RADIATOR
from .. import LevoitVital, CONF_LEVOIT_ID, levoit_vital_ns

DEPENDENCIES = ["levoitvital"]
REQUIREMENTS = ["ArduinoJson>=6.27.0"]

CONF_AUTOMODE = "automode"
CONF_OPTIONS = "options"

LevoitSelect = levoit_vital_ns.class_("LevoitSelect", select.Select, cg.Component)
LevoitSelectPurpose = levoit_vital_ns.enum("LevoitSelectPurpose")

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LEVOIT_ID): cv.use_id(LevoitVital),
        cv.Optional(CONF_AUTOMODE): select.select_schema(
            LevoitSelect, icon=ICON_RADIATOR
        )
        .extend(
            {
                cv.Optional(
                    CONF_OPTIONS, default=["Default", "Quiet", "Efficient"]
                ): cv.ensure_list(cv.string),
                cv.Optional("optimistic", default=False): cv.boolean,
            }
        )
        .extend(cv.COMPONENT_SCHEMA),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_LEVOIT_ID])

    if config_automode := config.get(CONF_AUTOMODE):
        var = await select.new_select(
            config_automode,
            parent,
            LevoitSelectPurpose.AUTOMODE,
            options=config_automode[CONF_OPTIONS],
        )
        cg.add(parent.set_select(var, LevoitSelectPurpose.AUTOMODE))
        await cg.register_component(var, config_automode)
