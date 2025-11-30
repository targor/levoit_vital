#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/helpers.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/application.h"
#include "enums.h"

namespace esphome
{
    namespace levoit_vital
    {
        class LevoitSwitch;     // forward declaration to prevent errors when including switch
        class LevoitSelect;     // forward declaration to prevent errors when including select
        class LevoitSensor;     // forward declaration to prevent errors when including sensor
        class LevoitTextSensor; // forward declaration to prevent errors when including textsensor
        class LevoitNumber;     // forward declaration to prevent errors when including number

        enum ModelType
        {
            VITAL200S = 0
        };

        class LevoitVital : public Component, public uart::UARTDevice, public Application
        {
            // alo methods that can be called by esphonme
        public:
            // Constructor
            LevoitVital() = default;

            void set_device_model(std::string model);

            // Standard component functions to override
            void setup() override;
            void dump_config() override;
            void loop() override;
            void set_switch(LevoitSwitch *sw, LevoitSwitchPurpose purpose);
            void set_select(LevoitSelect *select, LevoitSelectPurpose purpose);
            void set_sensor(LevoitSensor *sensor, LevoitSensorPurpose purpose);
            void set_text_sensor(LevoitTextSensor *sensor, LevoitTextSensorPurpose purpose);
            void set_number(LevoitNumber *number, LevoitNumberPurpose purpose);
            void sendCommand(CommandType commandType);

            // switches
            LevoitSwitch *device_power_switch;
            LevoitSwitch *light_detect_switch;
            LevoitSwitch *display_lock_switch;
            LevoitSwitch *display_switch;

            // select controls
            LevoitSelect *select_automode;
            LevoitSelect *select_fanlevel;
            LevoitSelect *select_fanmode;

            // number sensors
            LevoitSensor *particle_density;
            LevoitSensor *airquality_score;

            // textsensors
            LevoitTextSensor *airquality_level;
            LevoitTextSensor *display_state;
            LevoitTextSensor *device_fw_version_text;

            // number
            LevoitNumber *efficient_num;

        protected:
            // variables
            uint8_t lastWifiState = 0;
            uint8_t buffer_[256];
            int buf_len_ = 0;
            uint32_t last_byte_time = 0;
            ModelType model_;

            // functions
            void process_message(std::uint8_t *msg, int len);
        };
    } // namespace levoit_vital
} // namespace esphome
