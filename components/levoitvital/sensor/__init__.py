import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import ICON_AIR_FILTER, ICON_WEATHER_WINDY
from .. import LevoitVital, CONF_LEVOIT_ID, levoit_vital_ns

DEPENDENCIES = ["levoitvital"]


CONF_PARTICLE_DENSITY = "particle_density"
CONF_AIRQUALITY_SCORE = "airquality_score"
CONF_AIR_QUALITY_INDEX = "air_quality_index"

LevoitSensor = levoit_vital_ns.class_("LevoitSensor", sensor.Sensor, cg.Component)
LevoitSensorPurpose = levoit_vital_ns.enum("LevoitSensorPurpose")


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LEVOIT_ID): cv.use_id(LevoitVital),
        cv.Optional(CONF_PARTICLE_DENSITY): sensor.sensor_schema(
            LevoitSensor, icon=ICON_WEATHER_WINDY
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_AIRQUALITY_SCORE): sensor.sensor_schema(
            LevoitSensor, icon=ICON_AIR_FILTER
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_AIR_QUALITY_INDEX): sensor.sensor_schema(
            LevoitSensor, icon=ICON_AIR_FILTER
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LEVOIT_ID])

    if config_particle_density := config.get(CONF_PARTICLE_DENSITY):
        var = await sensor.new_sensor(
            config_particle_density, parent, LevoitSensorPurpose.PARTICLE_DENSITY
        )
        cg.add(parent.set_sensor(var, LevoitSensorPurpose.PARTICLE_DENSITY))
        await cg.register_component(var, config_particle_density)

    if config_airquality_score := config.get(CONF_AIRQUALITY_SCORE):
        var = await sensor.new_sensor(
            config_airquality_score, parent, LevoitSensorPurpose.AIRQUALITY_SCORE
        )
        cg.add(parent.set_sensor(var, LevoitSensorPurpose.AIRQUALITY_SCORE))
        await cg.register_component(var, config_airquality_score)

    if config_air_quality_index := config.get(CONF_AIR_QUALITY_INDEX):
        var = await sensor.new_sensor(
            config_air_quality_index, parent, LevoitSensorPurpose.AIR_QUALITY_INDEX
        )
        cg.add(parent.set_sensor(var, LevoitSensorPurpose.AIR_QUALITY_INDEX))
        await cg.register_component(var, config_air_quality_index)
