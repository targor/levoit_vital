import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import ICON_WEATHER_WINDY
from .. import LevoitVital, CONF_LEVOIT_ID, levoit_vital_ns

DEPENDENCIES = ["levoitvital"]


CONF_AIRQUALITY_LEVEL = "airquality_level"
CONF_DISPLAY_STATE = "display_state"
CONF_DEVICE_FW_VERSION_TEXT = "device_fw_version_text"

LevoitTextSensor = levoit_vital_ns.class_(
    "LevoitTextSensor", text_sensor.TextSensor, cg.Component
)
LevoitTextSensorPurpose = levoit_vital_ns.enum("LevoitTextSensorPurpose")


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LEVOIT_ID): cv.use_id(LevoitVital),
        
        cv.Optional(CONF_AIRQUALITY_LEVEL): text_sensor.text_sensor_schema(
            LevoitTextSensor, icon=ICON_WEATHER_WINDY
        ).extend(cv.COMPONENT_SCHEMA),
        
        cv.Optional(CONF_DISPLAY_STATE): text_sensor.text_sensor_schema(
            LevoitTextSensor, icon=ICON_WEATHER_WINDY
        ).extend(cv.COMPONENT_SCHEMA),
        
        cv.Optional(CONF_DEVICE_FW_VERSION_TEXT): text_sensor.text_sensor_schema(
            LevoitTextSensor, icon=ICON_WEATHER_WINDY
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LEVOIT_ID])

    if config_airquality_level := config.get(CONF_AIRQUALITY_LEVEL):
        var = await text_sensor.new_text_sensor(
            config_airquality_level, parent, LevoitTextSensorPurpose.AIRQUALITY_LEVEL
        )
        cg.add(parent.set_text_sensor(var, LevoitTextSensorPurpose.AIRQUALITY_LEVEL))
        await cg.register_component(var, config_airquality_level)

    if config_display_2 := config.get(CONF_DISPLAY_STATE):
        var = await text_sensor.new_text_sensor(
            config_display_2, parent, LevoitTextSensorPurpose.DISPLAY_STATE
        )
        cg.add(parent.set_text_sensor(var, LevoitTextSensorPurpose.DISPLAY_STATE))
        await cg.register_component(var, config_display_2)

    if config_device_fw_version_text := config.get(CONF_DEVICE_FW_VERSION_TEXT):
        var = await text_sensor.new_text_sensor(
            config_device_fw_version_text, parent, LevoitTextSensorPurpose.DEVICE_FW_VERSION_TEXT
        )
        cg.add(parent.set_text_sensor(var, LevoitTextSensorPurpose.DEVICE_FW_VERSION_TEXT))
        await cg.register_component(var, config_device_fw_version_text)
