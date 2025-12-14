#pragma once
#include <cstdint>

namespace esphome
{
    namespace levoit_vital
    {
        enum LevoitSwitchPurpose : uint8_t
        {
            DEVICE_POWER,
            LIGHT_DETECT,
            DISPLAY_LOCK,
            DISPLAY
        };

        enum LevoitSelectPurpose : uint8_t
        {
            AUTOMODE,
            FANLEVEL,
            FANMODE
        };

        enum LevoitSensorPurpose : uint8_t
        {
            PARTICLE_DENSITY,
            AIRQUALITY_SCORE
        };

        enum LevoitTextSensorPurpose : uint8_t
        {
            AIRQUALITY_LEVEL,
            DISPLAY_STATE,
            DEVICE_FW_VERSION_TEXT,
            REPLACE_AIRFILTER
        };

        enum LevoitNumberPurpose : uint8_t
        {
            EFFICIENT_NUM
        };

        // used fo sending different commands
        typedef enum
        {
            setDeviceON,
            setDeviceOFF,
            setDeviceFanLvl1,
            setDeviceFanLvl2,
            setDeviceFanLvl3,
            setDeviceFanLvl4,
            setLightDetectOn,
            setLightDetectOff,
            setDisplayLockOn,
            setDisplayLockOff,
            setDisplayOn,
            setDisplayOff,
            setAutomodeQuiet,
            setAutoModeDefault,
            setAutoModeEfficient,
            setSleepModeDefault,
            // setSleeoModeCustom, not implemented yet
            setFanModeManual,
            setFanModeAuto,
            setFanModeSleep,
            setFanModePet,
            resetFilter, // use with care, the filter value is stored in the SC95F8617/ chip and cannot be manipulated
            setWifiLedOn,
            setWifiLedOff,
            setWifiLedBlinking
        } CommandType;

        // used fo logs
        inline const char *CommandTypeStr[] = {
            "setDeviceON",
            "setDeviceOFF",
            "setDeviceFanLvl1",
            "setDeviceFanLvl2",
            "setDeviceFanLvl3",
            "setDeviceFanLvl4",
            "setLightDetectOn",
            "setLightDetectOff",
            "setDisplayLockOn",
            "setDisplayLockOff",
            "setDisplayOn",
            "setDisplayOff",
            "setAutomodeQuiet",
            "setAutoModeDefault",
            "setAutoModeEfficient",
            "setSleepModeDefault",
            //"setSleeoModeCustom", not implemented yet
            "setFanModeManual",
            "setFanModeAuto",
            "setFanModeSleep",
            "setFanModePet",
            "resetFilter", // use with care, the filter value is stored in the SC95F8617/ chip and cannot be manipulated
            "setWifiLedOn",
            "setWifiLedOff",
            "setWifiLedBlinking"};
    }
}
