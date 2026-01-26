import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import DEVICE_CLASS_LIGHT
from .. import LevoitVital, CONF_LEVOIT_ID, levoit_vital_ns

DEPENDENCIES = ["levoitvital"]


CONF_LIGHT_DETECTED = "light_detected"

LevoitBinarySensor = levoit_vital_ns.class_("LevoitBinarySensor", binary_sensor.BinarySensor, cg.Component)
LevoitBinarySensorPurpose = levoit_vital_ns.enum("LevoitBinarySensorPurpose")


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LEVOIT_ID): cv.use_id(LevoitVital),
        cv.Optional(CONF_LIGHT_DETECTED): binary_sensor.binary_sensor_schema(
            LevoitBinarySensor, device_class=DEVICE_CLASS_LIGHT
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LEVOIT_ID])

    if config_light_detected := config.get(CONF_LIGHT_DETECTED):
        var = await binary_sensor.new_binary_sensor(
            config_light_detected, parent, LevoitBinarySensorPurpose.LIGHT_DETECTED
        )
        cg.add(parent.set_binary_sensor(var, LevoitBinarySensorPurpose.LIGHT_DETECTED))
        await cg.register_component(var, config_light_detected)
