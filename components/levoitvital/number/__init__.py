import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import ICON_AIR_FILTER
from .. import LevoitVital, CONF_LEVOIT_ID, levoit_vital_ns

DEPENDENCIES = ["levoitvital"]

CONF_EFFICIENT_NUM = "efficient_num"
CONF_POWERMODE_TIME = "powermode_time"

LevoitNumber = levoit_vital_ns.class_("LevoitNumber", number.Number, cg.Component)
LevoitNumberPurpose = levoit_vital_ns.enum("LevoitNumberPurpose")

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LEVOIT_ID): cv.use_id(LevoitVital),
        cv.Optional(CONF_EFFICIENT_NUM): number.number_schema(
            LevoitNumber, icon=ICON_AIR_FILTER
        ).extend(cv.COMPONENT_SCHEMA),
         cv.Optional(CONF_POWERMODE_TIME): number.number_schema(
            LevoitNumber, icon=ICON_AIR_FILTER
        ).extend(cv.COMPONENT_SCHEMA),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_LEVOIT_ID])

    if config_efficient_num := config.get(CONF_EFFICIENT_NUM):
        var = await number.new_number(
            config_efficient_num,
            parent,
            LevoitNumberPurpose.EFFICIENT_NUM,
            min_value=100,
            max_value=1800,
            step=1,
        )
        cg.add(parent.set_number(var, LevoitNumberPurpose.EFFICIENT_NUM))
        await cg.register_component(var, config_efficient_num)
     
    if config_powermode_time := config.get(CONF_POWERMODE_TIME):
        var = await number.new_number(
            config_powermode_time,
            parent,
            LevoitNumberPurpose.POWERMODE_TIME,
            min_value=1,
            max_value=1440,
            step=1,
        )
        cg.add(parent.set_number(var, LevoitNumberPurpose.POWERMODE_TIME))
        await cg.register_component(var, config_powermode_time)
