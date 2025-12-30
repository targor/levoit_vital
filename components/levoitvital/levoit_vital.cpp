#include "levoit_vital.h"
#include "esphome/components/network/util.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/util.h"
#include "esphome/components/wifi/wifi_component.h"
#include "switch/levoit_vital_switch.h"
#include "select/levoit_vital_select.h"
#include "sensor/levoit_vital_sensor.h"
#include "number/levoit_vital_number.h"
#include "button/levoit_vital_button.h"
#include "text_sensor/levoit_vital_text_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "settings.h"
#include "helper.h"

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <time.h>
#include <vector>
#include "freertos/task.h"

// could be that less values generat errors(but not tested troughoutly).
std::uint8_t messageUpCounter = 16;

namespace esphome
{
    namespace levoit_vital
    {
        static const char *const TAG = "levoitvital";

        /*Methods to set switches,selects,sensors,number that are coming from the yaml file */
        void LevoitVital::set_switch(LevoitSwitch *sw, LevoitSwitchPurpose purpose)
        {
            switch (purpose)
            {
            case DEVICE_POWER:
            {
                this->device_power_switch = sw;
                break;
            }
            case LIGHT_DETECT:
            {
                this->light_detect_switch = sw;
                break;
            }
            case DISPLAY_LOCK:
            {
                this->display_lock_switch = sw;
                break;
            }
            case DISPLAY:
            {
                this->display_switch = sw;
                break;
            }
            }
        }

        void LevoitVital::set_select(LevoitSelect *select, LevoitSelectPurpose purpose)
        {
            switch (purpose)
            {
            case AUTOMODE:
            {
                this->select_automode = select;
                break;
            }
            case FANLEVEL:
            {
                this->select_fanlevel = select;
                break;
            }
            case FANMODE:
            {
                this->select_fanmode = select;
                break;
            }
            }
        }

        void LevoitVital::set_sensor(LevoitSensor *sensor, LevoitSensorPurpose purpose)
        {
            switch (purpose)
            {
            case PARTICLE_DENSITY:
            {
                this->particle_density = sensor;
                break;
            }
            case AIRQUALITY_SCORE:
            {
                this->airquality_score = sensor;
                break;
            }
            }
        }

        void LevoitVital::set_text_sensor(LevoitTextSensor *sensor, LevoitTextSensorPurpose purpose)
        {
            switch (purpose)
            {
            case AIRQUALITY_LEVEL:
            {
                this->airquality_level = sensor;
                break;
            }
            case DISPLAY_STATE:
            {
                this->display_state = sensor;
                break;
            }
            case DEVICE_FW_VERSION_TEXT:
            {
                this->device_fw_version_text = sensor;
                break;
            }
            case REPLACE_AIRFILTER:
            {
                this->replace_airfilter = sensor;
                break;
            }
            }
        }

        void LevoitVital::set_number(LevoitNumber *number, LevoitNumberPurpose purpose)
        {
            switch (purpose)
            {
            case EFFICIENT_NUM:
            {
                this->efficient_num = number;
                break;
            }
            case POWERMODE_TIME:
            {
                this->powermode_time = number;
                break;
            }
            }
        }

        void LevoitVital::set_button(LevoitButton *button, LevoitButtonPurpose purpose)
        {
            switch (purpose)
            {
            case POWERMODE:
            {
                this->powermode = button;
                break;
            }
            }
        }
        /* end defining yaml setters */

        void LevoitVital::setup()
        {
            ESP_LOGI(TAG, "Setting up Levoit %s", model_ == VITAL200S ? "VITAL200S" : "NONE");
            Vital200Settings::getInstance().init();
 
            auto &settings = Vital200Settings::getInstance();
            this->powermode_time->publish_state(settings.powerModeValue);

            srand((unsigned int)time(NULL));
            this->device_fw_version_text->publish_state("1.0.3");
        }

        /// @brief The main loop, that is triggeed by the esphome framework automatically
        void LevoitVital::loop()
        {

            //*** reads bytes from the uart buffer an waits for 10ms (to have a complete message block) before processing the message ***
            while (available())
            {
                uint8_t b;
                read_byte(&b);
                buffer_[buf_len_++] = b;
                last_byte_time = millis();
            }

            if (buf_len_ > 0 && millis() - last_byte_time > 10)
            {
                process_message(buffer_, buf_len_);
                buf_len_ = 0;
            }
            //*** end ***

            // checks for wifi an sets the air purifier wifi led to BLINKING or to ON depending on wifi state
            if (wifi::global_wifi_component->is_connected())
            {
                if (lastWifiState == 0)
                {
                    ESP_LOGI(TAG, "Wifi is connected");
                    lastWifiState = 1;
                    sendCommand(setWifiLedOn);
                }
            }
            else
            {
                if (lastWifiState == 1)
                {
                    ESP_LOGI(TAG, "Wifi is disconnected");
                    lastWifiState = 0;
                    sendCommand(setWifiLedBlinking);
                }
            }

            delay(1);
        }

        void LevoitVital::dump_config() {}

        void LevoitVital::set_device_model(std::string model)
        {
            if (model == "VITAL200S")
                model_ = ModelType::VITAL200S;
        }

        /// @brief Processes all messages that are coming from the air purifier secondary chip on UART1
        /// @param msg
        /// @param len
        void LevoitVital::process_message(std::uint8_t *msg, int len)
        {
            ESP_LOGI(TAG, "Received packet (%d bytes)", len);
            if (msg[0] == 0xA5 && msg[1] == 0x22) // this indicates a status message
            {

                // this is the default ack with variable bytes.
                // all bytes but number 3 and 6 are static.
                // byte 3 is the counting byte from the message, value 6 must be calculated appropriate.
                std::uint8_t data[10] = {0xA5, 0x12, msg[2], 0x04, 0x00, 0x00, 0x02, 0x00, 0x55, 0x00};

                // calculate the checksum and put it into byte number 6
                data[5] = calculateCommandOrAck(data, sizeof(data));

                // send the ack
                size_t len2 = sizeof(data) / sizeof(data[0]);
                write_array(data, len2);
                this->flush();

                // for debugging.
                log_hex(msg, len);

                // Reads settings. Message must have at least 113 bytes to read all current known settings.
                // Checks for changed states and updates all sensors,switches,numbers and so on.
                if (len >= 113)
                {
                    auto &settings = Vital200Settings::getInstance();

                    std::uint8_t countUp = msg[2];   // not processed (also currently no need to process as this is the secondary chip counter)
                    std::uint8_t countDown = msg[5]; // not processed (also currently no need to process as thi sis the secondary chip checksum)

                    if (this->device_power_switch && checkValChanged(settings.onoffState, "onoffState", msg[20]))
                    {
                        this->device_power_switch->publish_state(settings.onoffState);
                    }

                    // displayOnoffState2 indicates if the display is set on, but if light detect is also on and the room is dark, the display is not illuminated
                    // displayOnOffState displays the real led state, if the led of the display is illuminated or not.
                    if (this->display_switch && checkValChanged(settings.displayOnOffState, "displayOnOffState", msg[32]) ||
                        this->display_state && checkValChanged(settings.displayOnOffState2, "displayOnoffState2", msg[35]))
                    {
                        this->display_switch->publish_state(settings.displayOnOffState);

                        if (settings.displayOnOffState2 == 1 && settings.displayOnOffState == 0)
                        {
                            this->display_state->publish_state("The display is not illuminated because lightdetect is on and it is dark right now.");
                        }
                        if (settings.displayOnOffState2 == 0 && settings.displayOnOffState == 0)
                        {
                            this->display_state->publish_state("The display is not illuminated.");
                        }
                        if (settings.displayOnOffState2 == 1 && settings.displayOnOffState == 1)
                        {
                            this->display_state->publish_state("The display is illuminated");
                        }
                    }

                    if (this->display_switch && checkValChanged(settings.airfilter_state, "airfilter_state", msg[38]))
                    {
                        switch (settings.airfilter_state)
                        {
                        case 0:
                            this->replace_airfilter->publish_state("Filter is ok.");
                            break;
                        case 1:
                            this->replace_airfilter->publish_state("Filter must be replaced.");
                            break;
                        }
                    }

                    if (this->display_lock_switch && checkValChanged(settings.displayLock, "displayLock", msg[51]))
                    {
                        this->display_lock_switch->publish_state(settings.displayLock);
                    }

                    if (this->light_detect_switch && checkValChanged(settings.lightDetection, "lightDetection", msg[68]))
                    {
                        this->light_detect_switch->publish_state(settings.lightDetection);
                    }

                    if (this->select_fanmode && checkValChanged(settings.fanMode, "fanMode", msg[23]))
                    {
                        switch (settings.fanMode)
                        {
                        case 0:
                        {
                            this->select_fanmode->publish_state("Manual");
                            break;
                        }
                        case 1:
                        {
                            this->select_fanmode->publish_state("Sleep");
                            break;
                        }
                        case 2:
                        {
                            this->select_fanmode->publish_state("Automatic");
                            break;
                        }
                        case 3:
                        {
                            this->select_fanmode->publish_state("Pet");
                            break;
                        }
                        }
                    }

                    if (this->select_fanlevel && checkValChanged(settings.fanLevel, "fanLevel", msg[26]))
                    {
                        switch (settings.fanLevel)
                        {
                        case 0:
                        case 255:
                        {
                            this->select_fanlevel->publish_state("NOT SET");
                            break;
                        }
                        case 1:
                        {
                            this->select_fanlevel->publish_state("1");
                            break;
                        }
                        case 2:
                        {
                            this->select_fanlevel->publish_state("2");
                            break;
                        }
                        case 3:
                        {
                            this->select_fanlevel->publish_state("3");
                            break;
                        }
                        case 4:
                        {
                            this->select_fanlevel->publish_state("4");
                            break;
                        }
                        }
                    }

                    if (this->airquality_level && checkValChanged(settings.airQualityLevel, "airQualityLevel", msg[41]))
                    {
                        std::string level = "";
                        switch (settings.airQualityLevel)
                        {
                        case 1:
                        {
                            level = "Very good";
                            break;
                        }
                        case 2:
                        {
                            level = "Good";
                            break;
                        }
                        case 3:
                        {
                            level = "Moderate";
                            break;
                        }
                        case 4:
                        {
                            level = "Bad";
                            break;
                        }
                        }
                        this->airquality_level->publish_state(level);
                    }

                    if (this->particle_density && checkValChanged(settings.particleDensity, "particleDensity", msg[47]))
                    {
                        this->particle_density->publish_state(settings.particleDensity);
                    }

                    if (this->airquality_score && checkValChanged(settings.airQualityScore, "airQualityScore", msg[44]))
                    {
                        this->airquality_score->publish_state(settings.airQualityScore);
                    }

                    if (this->select_automode && checkValChanged(settings.autoMode, "autoMode", msg[54]))
                    {
                        switch (settings.autoMode)
                        {
                        case 0:
                        {
                            this->select_automode->publish_state("Default");
                            break;
                        }
                        case 1:
                        {
                            this->select_automode->publish_state("Quiet");
                            break;
                        }
                        case 2:
                        {
                            this->select_automode->publish_state("Efficient");
                            break;
                        }
                        }
                    }

                    // this is currently not processed, as configuring the sleep mode takes more effort and will come later on maybe :)
                    checkValChanged(settings.sleepMode, "sleepMode", msg[77]);

                    std::uint16_t efficientTempValue = parse16BitIntegerValue(msg[57], msg[58]);
                    if (efficientTempValue < 100)
                        efficientTempValue = 100;
                    if (efficientTempValue > 1800)
                        efficientTempValue = 1800;

                    if (this->efficient_num && checkValChanged16(settings.efficientValue, "efficient value", efficientTempValue))
                    {
                        float f = static_cast<float>(efficientTempValue);
                        this->efficient_num->publish_state(f);
                    }

                    // The clean settings are  currently not processed, as configuring this takes more effort and will come later on maybe :)
                    //  but the values are already all known, maybe if i have more time i will implement this feature.
                    //********************************* */
                    checkValChanged(settings.quickClean.fanLevel, "QuickClean-FanLevel", msg[87]);
                    checkValChanged16(settings.quickClean.timeInMinutes, "QuickClean-Minutes", msg[83]);

                    checkValChanged(settings.whiteNoise.fanLevel, "WhiteNoise-FanLevel", msg[97]);
                    checkValChanged16(settings.whiteNoise.timeInMinutes, "WhiteNoise-Minutes", msg[93]);

                    checkValChanged(settings.sleep.fanLevel, "SleepMode-FanLevel", msg[104]);

                    std::uint16_t sleepModeTempValue = parse16BitIntegerValue(msg[100], msg[101]);
                    if (sleepModeTempValue < 0)
                        sleepModeTempValue = 100;
                    if (sleepModeTempValue > 719)
                        sleepModeTempValue = 719;
                    checkValChanged16(settings.sleep.timeInMinutes, "SleepMode-Minutes", sleepModeTempValue);
                    checkValChanged(settings.dayTime.fanLevel, "SleepModeDayTime-FanLevel", msg[112]);

                    checkValChanged(settings.dayTime.fanMode, "SleepModeDayTime-FanMode", msg[110]);

                    checkValChanged(settings.dayTime.turnOffOnDayTime, "SleepModeDayTime-turnOffOnDayTime", msg[107]);
                    /************************************ */

                    settings.initialized = true;
                }
            }
            else if (msg[0] == 0xA5 && msg[1] == 0x12) // this indicates a ack from the chip and can be ignored for now.
            {
            }
            else
            {
                // this may be a unknown package and could be logged for debug purposes
                // log_hex(msg, len);
            }
        }

        /// @brief Sends the commands depending on its type. long method but plain simple.
        /// There are some extra bytes that i did not understand completely, but it works so far :).
        /// @param commandType
        void LevoitVital::sendCommand(CommandType commandType)
        {

            if (this->timer_active_)
            {
                this->cancel_timeout("delayed_action");
                this->timer_active_=false;
            }

            auto &settings = Vital200Settings::getInstance();
            if (!settings.initialized)
            {
                return;
            }

            std::uint8_t command = 0x03;
            std::uint8_t commandCategory = 0x01;
            std::uint8_t cmdValue = 0x03;

            std::uint8_t fixed1 = 0x07;
            std::uint8_t fixed2 = 0x55;
            std::uint8_t fixed3 = 0x01;

            std::vector<uint8_t> extraBytes;
            const char *str = CommandTypeStr[commandType];
            bool removeLastByte = false;

            switch (commandType)
            {
            case setDeviceON:
            {
                // 0xA5	0x22	0x10	0x07	0x00	0xCC	0x02	0x00	0x50	0x00	0x01	0x01	0x01
                command = 0x00;
                commandCategory = 0x01;
                fixed2 = 0x50;
                cmdValue = 0x01;
                str = CommandTypeStr[commandType];
                break;
            }
            case setDeviceOFF:
            {
                // 0xA5	0x22	0x11	0x07	0x00	0xCC	0x02	0x00	0x50	0x00	0x01	0x01	0x00
                command = 0x00;
                commandCategory = 0x01;
                fixed2 = 0x50;
                cmdValue = 0x00;
                break;
            }
            case setDeviceFanLvl1:
            {
                // 0xA5	0x22 0x1A 0x07	0x00	0xBA	0x02	0x03	0x55	0x00	0x01	0x01	0x01
                command = 0x03;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x01;
                break;
            }
            case setDeviceFanLvl2:
            {
                // 0xA5	0x22	0x17	0x07	0x00	0xBC	0x02	0x03	0x55	0x00	0x01	0x01	0x02
                command = 0x03;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x02;

                break;
            }
            case setDeviceFanLvl3:
            {
                // 0xA5	0x22	0x18	0x07	0x00	0xBA	0x02	0x03	0x55	0x00	0x01	0x01	0x03
                command = 0x03;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x03;
                break;
            }
            case setDeviceFanLvl4:
            {
                // 0xA5	0x22	0x19	0x07	0x00	0xB8	0x02	0x03	0x55	0x00	0x01	0x01	0x04
                command = 0x03;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x04;
                break;
            }
            case setLightDetectOn:
            {
                // 0xA5	0x22	0x1C	0x07	0x00	0xAA	0x02	0x11	0x55	0x00	0x01	0x01	0x01
                command = 0x11;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x01;
                break;
            }
            case setLightDetectOff:
            {
                // 0xA5	0x22	0x1D	0x07	0x00	0xAA	0x02	0x11	0x55	0x00	0x01	0x01	0x00
                command = 0x11;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x00;
                break;
            }
            case setDisplayLockOn:
            {
                // 0xA5	0x22	0x20	0x07	0x00	0x7B	0x02	0x40	0x51	0x00	0x01	0x01	0x01
                command = 0x40;
                commandCategory = 0x01;
                fixed2 = 0x51;
                cmdValue = 0x01;
                break;
            }
            case setDisplayLockOff:
            {
                // 0xA5	0x22	0x21	0x07	0x00	0x7B	0x02	0x40	0x51	0x00	0x01	0x01	0x00
                command = 0x40;
                commandCategory = 0x01;
                fixed2 = 0x51;
                cmdValue = 0x00;
                break;
            }
            case setDisplayOn:
            {
                // 0xA5	0x22	0x25	0x07	0x00	0xAF	0x02	0x04	0x55	0x00	0x01	0x01	0x00
                command = 0x04;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x64;
                break;
            }
            case setDisplayOff:
            {
                // 0xA5	0x22	0x26	0x07	0x00	0x4A	0x02	0x04	0x55	0x00	0x01	0x01	0x64
                command = 0x04;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x00;
                break;
            }
            case setAutomodeQuiet:
            {
                // 0xA5	0x22	0x29	0x0B	0x00	0xA2	0x02	0x02	0x55	0x00	0x02	0x01	0x01	0x03	0x02	0x00	0x00
                command = 0x02;
                commandCategory = 0x02;
                fixed1 = 0x0B;
                fixed2 = 0x55;
                cmdValue = 0x01;
                extraBytes = {0x03, 0x02, 0x00, 0x00};
                break;
            }
            case setAutoModeDefault:
            {
                // 0xA5	0x22	0x2A	0x0B	0x00	0xA2	0x02	0x02	0x55	0x00	0x02	0x01	0x00	0x03	0x02	0x00	0x00
                command = 0x02;
                commandCategory = 0x02;
                fixed1 = 0x0B;
                fixed2 = 0x55;
                cmdValue = 0x00;
                extraBytes = {0x03, 0x02, 0x00, 0x00};
                break;
            }
            case setAutoModeEfficient:
            {
                command = 0x02;
                commandCategory = 0x02;
                fixed1 = 0x0B;
                fixed2 = 0x55;
                cmdValue = 0x02;
                extraBytes = generateEfficencyValue(settings.efficientValue);
                break;
            }
            case setSleepModeDefault:
            {
                // 0xA5	0x22	0x16	0x2B	0x00	0x1D	0x02	0x02	0x55	0x00	0x04	0x01	0x00	0x05	0x01	0x00	0x06	0x02	0x00	0x00	0x07	0x01	0x00	0x08	0x01	0x00	0x09	0x02	0x00	0x00	0x0A	0x01	0x00	0x0B	0x01	0x00	0x0C	0x02	0x00	0x00	0x0D	0x01	0x00	0x0E	0x01	0x00	0x0F	0x01	0x00
                // 0x05	0x01	0x00	0x06	0x02	0x00	0x00	0x07	0x01	0x00	0x08	0x01	0x00	0x09	0x02	0x00	0x00	0x0A	0x01	0x00	0x0B	0x01	0x00	0x0C	0x02	0x00	0x00	0x0D	0x01	0x00	0x0E	0x01	0x00	0x0F	0x01	0x00
                command = 0x02;
                commandCategory = 0x04;
                fixed1 = 0x2B;
                fixed2 = 0x55;
                cmdValue = 0x00;
                extraBytes = {
                    0x05, 0x01, 0x00, 0x06, 0x02, 0x00, 0x00, 0x07, 0x01, 0x00,
                    0x08, 0x01, 0x00, 0x09, 0x02, 0x00, 0x00, 0x0A, 0x01, 0x00,
                    0x0B, 0x01, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x0D, 0x01, 0x00,
                    0x0E, 0x01, 0x00, 0x0F, 0x01, 0x00};
                break;
            }
            case setFanModeManual:
            {
                // 0xA5	0x22	0x3C	0x07	0x00	0x98	0x02	0x02	0x55	0x00	0x01	0x01	0x02
                command = 0x02;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x00;
                break;
            }
            case setFanModeAuto:
            {
                // 0xA5	0x22	0x3C	0x07	0x00	0x98	0x02	0x02	0x55	0x00	0x01	0x01	0x02
                command = 0x02;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x02;
                break;
            }
            case setFanModeSleep:
            {
                // 0xA5	0x22	0x3D	0x07	0x00	0x98	0x02	0x02	0x55	0x00	0x01	0x01	0x01
                command = 0x02;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x01;
                break;
            }
            case setFanModePet:
            {
                // 0xA5	0x22	0x3E	0x07	0x00	0x93	0x02	0x02	0x55	0x00	0x01	0x01	0x05
                command = 0x02;
                commandCategory = 0x01;
                fixed2 = 0x55;
                cmdValue = 0x05;
                break;
            }
            case resetFilter:
            {
                // 0xA5	0x22	0x41	0x06	0x00	0x92	0x02	0x05	0x55	0x00	0x03	0x00
                //  this command has only 12 bytes.
                command = 0x05;
                commandCategory = 0x03;
                fixed2 = 0x55;
                removeLastByte = true;
                break;
            }
            case setWifiLedOff:
            {
                // 0xA5	0x22	0x01	0x12	0x00	0xC1	0x02	0x18	0x50	0x00	0x01	0x01	0x00
                command = 0x18;
                commandCategory = 0x01;
                fixed1 = 0x12;
                fixed2 = 0x50;
                cmdValue = 0x00;
                extraBytes = {0x02, 0x02, 0xF4, 0x01, 0x03, 0x02, 0xF4, 0x01, 0x04, 0x01, 0x00};
                break;
            }
            case setWifiLedOn:
            {
                // 0xA5	0x22	0x05	0x12	0x00	0xAC	0x02	0x18	0x50	0x00	0x01	0x01	0x01
                command = 0x18;
                commandCategory = 0x01;
                fixed1 = 0x12;
                fixed2 = 0x50;
                cmdValue = 0x01;
                extraBytes = {0x02, 0x02, 0x7D, 0x00, 0x03, 0x02, 0x7D, 0x00, 0x04, 0x01, 0x00};
                break;
            }
            case setWifiLedBlinking:
            {
                // 0xA5	0x22	0x05	0x12	0x00	0xAC	0x02	0x18	0x50	0x00	0x01	0x01	0x01
                command = 0x18;
                commandCategory = 0x01;
                fixed1 = 0x12;
                fixed2 = 0x50;
                cmdValue = 0x02;
                extraBytes = {0x02, 0x02, 0x7D, 0x02, 0x03, 0x02, 0x7D, 0x00, 0x04, 0x01, 0x00};
                break;
            }
            case setPowerMode:
            {
                if (this->timer_active_)
                {
                    this->cancel_timeout("delayed_action");
                }

                this->timer_active_=true;
                this->sendCommand(setDeviceFanLvl4);
                this->set_timeout("delayed_action", (settings.powerModeValue * 60 * 1000), [this]()
                                { this->sendCommand(setFanModeAuto); });
                
                return;
            }
            }

            // this is the default messageblock
            // some messages can have some extra bytes (e.g. custom sleep mode).
            std::vector<uint8_t> messageBlock = {0xA5, 0x22, messageUpCounter, fixed1, 0x00, 0x00, 0x02, command, fixed2, 0x00, commandCategory, fixed3, cmdValue};

            // removes the last byte of the messageblock (this is only used for filter reset, as this message only has 12 bytes).
            if (removeLastByte)
            {
                messageBlock.pop_back();
            }

            // adds all extrabytes to the message if there are any
            if (!extraBytes.empty())
            {
                messageBlock.insert(messageBlock.end(), extraBytes.begin(), extraBytes.end());
            }

            // calculate the messages checksum
            messageBlock[5] = calculateCommandOrAck(messageBlock.data(), messageBlock.size());

            // update the message counter
            if (messageUpCounter == 255)
            {
                messageUpCounter = 16;
            }
            else
            {
                messageUpCounter++;
            }

            ESP_LOGI(TAG, "%s", str);
            log_hex(messageBlock.data(), messageBlock.size());

            // send message to the chip.
            this->write_array(messageBlock.data(), messageBlock.size());
            this->flush();
        }
    } // namespace levoit_vital
} // namespace esphome
