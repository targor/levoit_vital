#include "levoit_vital_binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace levoit_vital
    {

        static const char *const TAG = "levoit_vital.binary_sensor";

        void LevoitBinarySensor::dump_config()
        {
            LOG_BINARY_SENSOR("", "Levoit Vital Binary Sensor", this);
        }

    } // namespace levoit_vital
} // namespace esphome
