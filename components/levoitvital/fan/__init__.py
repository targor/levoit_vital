import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fan
from esphome.const import CONF_ID
from .. import LevoitVital, CONF_LEVOIT_ID, levoit_vital_ns

DEPENDENCIES = ["levoitvital"]

LevoitFan = levoit_vital_ns.class_("LevoitFan", fan.Fan, cg.Component)

CONFIG_SCHEMA = fan.fan_schema(LevoitFan).extend(
    {
        cv.GenerateID(CONF_LEVOIT_ID): cv.use_id(LevoitVital),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LEVOIT_ID])
    var = await fan.new_fan(config)
    await cg.register_component(var, config)
    cg.add(var.set_parent(parent))
    cg.add(parent.set_fan(var))
