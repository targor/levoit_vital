
#pragma once

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <time.h>
#include <sstream>
#include <iomanip>

inline void log_hex(const std::uint8_t *data, int len);
std::uint8_t calculateCommandOrAck(uint8_t *data, std::uint8_t dataLen);
std::vector<uint8_t> generateEfficencyValue(std::uint16_t value);
inline bool checkValChanged(std::uint8_t &value, const char *fieldName, std::uint8_t newValue);
std::uint16_t parse16BitIntegerValue(std::uint8_t high, std::uint8_t low);
inline bool checkValChanged16(std::uint16_t &value, const char *fieldName, std::uint16_t newValue);

/// @brief Creates a string from the incoming status message in hex format to reverse enigneer more data of the protocol.
/// @param data 
/// @param len 
inline void log_hex(const std::uint8_t *data, int len)
{
    char buffer[3 * len + 1]; // 2 Zeichen pro Byte + Leerzeichen + Nullterminator
    char *p = buffer;

    for (int i = 0; i < len; i++)
    {
        sprintf(p, "%02X ", data[i]);
        p += 3; // 2 Zeichen + Leerzeichen
    }
    buffer[3 * len] = '\0'; // Nullterminator

    ESP_LOGD("STATUS", "%s", buffer);
}

/// @brief The levoit vital uses an additive checksum with modulo 256 that is calculated in this method
/// @param data 
/// @param dataLen 
/// @return 
inline std::uint8_t calculateCommandOrAck(std::uint8_t *data, std::uint8_t dataLen)
{
    std::uint16_t sum = 0;
    for (std::uint8_t i = 0; i < dataLen; i++)
    {
        if (i != 5)
            sum += data[i]; // skip the checksum byte
    }
    return (0x1FF - sum) & 0xFF; // 511 - sum, modulo 256
}

/// @brief The "Efficient" value (in the Auto Mode Settings menu) seems to be a value from 100 to 1800 with the factor 1.3. .
/// We can calulate it and add these bytes to the efficency command.
/// @param value
/// @param efficencyValue
inline std::vector<uint8_t> generateEfficencyValue(std::uint16_t value)
{
    std::vector<uint8_t> efficencyValue(4);

    // lock value to min/max
    if (value < 100)
        value = 100;
    if (value > 1800)
        value = 1800;

    // Header
    efficencyValue[0] = 0x03;
    efficencyValue[1] = 0x02;

    // calc raw value (Skalierung 1.3)
    std::uint16_t raw = (std::uint16_t)(value * 1.3 + 0.5); // +0.5 rounding

    // Little Endian
    efficencyValue[2] = raw & 0xFF;        // Low-Byte
    efficencyValue[3] = (raw >> 8) & 0xFF; // High-Byte
    return efficencyValue;
}


inline std::uint16_t parse16BitIntegerValue(std::uint8_t high, std::uint8_t low)
{
    // merge bytes (Little Endian)
    std::uint16_t raw = (uint16_t)high | ((std::uint16_t)low << 8);

    std::uint16_t value = (std::uint16_t)(raw / 1.3 + 0.5);
    return value;
}

/// @brief Simply checks if a value from the incoming status message has changed.
/// @param value 
/// @param fieldName 
/// @param newValue 
/// @return 
inline bool checkValChanged(std::uint8_t &value, const char *fieldName, std::uint8_t newValue)
{
    if (value != newValue)
    {
        value = newValue;
        return true;
    }
    return false;
}

/// @brief Simply checks if a value(16 bit) from the incoming status message has changed.
/// @param value 
/// @param fieldName 
/// @param newValue 
/// @return 
inline bool checkValChanged16(std::uint16_t &value, const char *fieldName, std::uint16_t newValue)
{
    if (value != newValue)
    {
        // ESP_LOGI("CHANGE", "%s was changed to: %u", fieldName, newValue);
        value = newValue;
        return true;
    }
    return false;
}

/// @brief Calculates the Air Quality Index (AQI) from PM2.5 concentration using EPA breakpoints.
/// @param pm25 PM2.5 concentration in µg/m³
/// @return AQI value (0-500)
inline int calculateAQI(float pm25)
{
    if (pm25 < 0) return 0;
    
    // EPA PM2.5 breakpoints (µg/m³)
    const float breakpoints[7][2] = {
        {0.0, 12.0},    // 0-50 AQI
        {12.1, 35.4},   // 51-100 AQI
        {35.5, 55.4},   // 101-150 AQI
        {55.5, 150.4},  // 151-200 AQI
        {150.5, 250.4}, // 201-300 AQI
        {250.5, 350.4}, // 301-400 AQI
        {350.5, 500.4}  // 401-500 AQI
    };
    
    const int aqi_ranges[7][2] = {
        {0, 50},
        {51, 100},
        {101, 150},
        {151, 200},
        {201, 300},
        {301, 400},
        {401, 500}
    };
    
    // Find the appropriate breakpoint
    for (int i = 0; i < 7; i++) {
        if (pm25 <= breakpoints[i][1]) {
            if (i == 0 && pm25 < breakpoints[0][0]) return 0;
            
            float conc_low = (i == 0) ? 0 : breakpoints[i][0];
            float conc_high = breakpoints[i][1];
            int aqi_low = aqi_ranges[i][0];
            int aqi_high = aqi_ranges[i][1];
            
            // Linear interpolation
            return (int)((aqi_high - aqi_low) / (conc_high - conc_low) * (pm25 - conc_low) + aqi_low + 0.5);
        }
    }
    
    // Above 500.4 µg/m³
    return 500;
}