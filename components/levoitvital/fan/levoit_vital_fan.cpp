#include "esphome/core/log.h"
#include "levoit_vital_fan.h"
#include "../settings.h"

namespace esphome
{
  namespace levoit_vital
  {

    static const char *const TAG = "levoit_vital.fan";

    void LevoitFan::setup() 
    {
        // Initialize fan as off with no speed
        this->state = false;
        this->speed = 0;
    }

    void LevoitFan::dump_config() 
    {
        ESP_LOGCONFIG(TAG, "Levoit Vital Fan");
        ESP_LOGCONFIG(TAG, "  Speed levels: 4 (0=OFF, 1-4)");
        ESP_LOGCONFIG(TAG, "  Preset modes: Manual, Sleep, Automatic, Pet");
    }

    fan::FanTraits LevoitFan::get_traits()
    {
        auto traits = fan::FanTraits();
        traits.set_speed(true);
        traits.set_supported_speed_count(4); // 4 speed levels (1, 2, 3, 4)
        traits.set_supported_preset_modes({PRESET_MODE_MANUAL, PRESET_MODE_SLEEP, PRESET_MODE_AUTOMATIC, PRESET_MODE_PET});
        return traits;
    }

    void LevoitFan::control(const fan::FanCall &call)
    {
        if (call.get_state().has_value())
        {
            this->state = *call.get_state();
            if (!this->state)
            {
                // Fan turned off - speed 0
                this->speed = 0;
                ESP_LOGI(TAG, "Fan turned OFF (speed 0)");
                parent_->sendCommand(setDeviceOFF);
            }
        }

        if (call.has_preset_mode())
        {
            const char *preset = call.get_preset_mode();
            this->state = true; // Presets always turn the fan on

            ESP_LOGI(TAG, "Fan preset mode set to: %s", preset);

            if (strcmp(preset, PRESET_MODE_MANUAL) == 0)
            {
                parent_->sendCommand(setFanModeManual);
            }
            else if (strcmp(preset, PRESET_MODE_SLEEP) == 0)
            {
                parent_->sendCommand(setFanModeSleep);
            }
            else if (strcmp(preset, PRESET_MODE_AUTOMATIC) == 0)
            {
                parent_->sendCommand(setFanModeAuto);
            }
            else if (strcmp(preset, PRESET_MODE_PET) == 0)
            {
                parent_->sendCommand(setFanModePet);
            }
            else
            {
                ESP_LOGW(TAG, "Unknown preset mode: %s", preset);
            }
        }

        if (call.get_speed().has_value())
        {
            int speed_level = *call.get_speed();
            this->speed = speed_level;
            this->state = (speed_level > 0); // Auto turn on if speed > 0

            ESP_LOGI(TAG, "Fan speed set to: %d", speed_level);

            // When speed is manually set, switch to Manual mode
            if (speed_level > 0 && !this->has_preset_mode())
            {
                parent_->sendCommand(setFanModeManual);
            }

            switch (speed_level)
            {
            case 0:
                // Speed 0 = OFF
                ESP_LOGI(TAG, "Fan speed 0 - turning OFF");
                parent_->sendCommand(setDeviceOFF);
                break;
            case 1:
                parent_->sendCommand(setDeviceFanLvl1);
                break;
            case 2:
                parent_->sendCommand(setDeviceFanLvl2);
                break;
            case 3:
                parent_->sendCommand(setDeviceFanLvl3);
                break;
            case 4:
                parent_->sendCommand(setDeviceFanLvl4);
                break;
            default:
                ESP_LOGW(TAG, "Invalid fan speed: %d", speed_level);
                break;
            }
        }

        this->publish_state();
    }

  } // namespace levoit_vital
} // namespace esphome
