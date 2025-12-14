#pragma once

#include <cstdint>
#include "enums.h"

class Vital200Settings
{
public:
    // Zugriff auf die einzige Instanz
    static Vital200Settings &getInstance()
    {
        static Vital200Settings instance; // Singleton-Instanz
        return instance;
    }

    // Variablen
    bool initialized;
    std::uint8_t autoMode;           // 0=default, 1=quiet, 2=efficient
    std::uint16_t efficientValue;    // 100..1800
    std::uint8_t onoffState;         // 0=off, 1=on
    std::uint8_t fanMode;            // 0=manual,1=sleep,2=auto,5=pet
    std::uint8_t fanLevel;           // 0=minimum..5=automatic, 0xFF=NULL(not set)
    std::uint8_t displayOnOffState2; // 0=off, 1=on
    std::uint8_t displayOnOffState;  // 0=off, 1=on
    std::uint8_t airQualityLevel;    // 1=very good..4=bad
    std::uint8_t particleDensity;    // 0-254
    std::uint8_t airQualityScore;    // 0 to 100
    std::uint8_t displayLock;        // 0=off, 1=on
    std::uint8_t lightDetection;     // 0=off, 1=on
    std::uint8_t sleepMode;          // 0=default, 1=custom
    std::uint8_t airfilter_state;    // 0 = clean, 1 = must be replaced

    // Sleep Modes
    struct SleepModeQuickClean
    {
        std::uint8_t fanLevel;
        std::uint16_t timeInMinutes;
    } quickClean;

    struct SleepModeWhiteNoise
    {
        std::uint8_t fanLevel;
        std::uint16_t timeInMinutes;
    } whiteNoise;

    struct SleepModeSleep
    {
        std::uint8_t fanLevel;
        std::uint16_t timeInMinutes;
    } sleep;

    struct SleepModeDayTime
    {
        std::uint8_t turnOffOnDayTime;
        std::uint8_t fanLevel;
        std::uint8_t fanMode;
    } dayTime;

    // Init-Methode
    void init()
    {
        initialized = false;
        autoMode = 255;
        efficientValue = 255;
        onoffState = 255;
        fanMode = 255;
        fanLevel = 255;
        displayOnOffState2 = 255;
        displayOnOffState = 255;
        airQualityLevel = 255;
        particleDensity = 255;
        airQualityScore = 255;
        displayLock = 255;
        lightDetection = 255;
        sleepMode = 255;
        airfilter_state=255;

        quickClean.fanLevel = 255;      // default is 4
        quickClean.timeInMinutes = 255; // default is 5

        whiteNoise.fanLevel = 255;      // default is 1
        whiteNoise.timeInMinutes = 255; // default is 45

        sleep.fanLevel = 255;      // default is 5
        sleep.timeInMinutes = 255; // default is 480

        dayTime.turnOffOnDayTime = 255; // default is 0
        dayTime.fanLevel = 255;         // default is 2
        dayTime.fanMode = 255;          // default is 1
    }

private:
    Vital200Settings() = default; // Konstruktor privat
    Vital200Settings(const Vital200Settings &) = delete;
    Vital200Settings &operator=(const Vital200Settings &) = delete;
};