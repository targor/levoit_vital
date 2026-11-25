
#include "esphome/core/log.h"
#include "levoit_vital_select.h"
#include "../settings.h"

namespace esphome
{
  namespace levoit_vital
  {

    static const char *const TAG = "levoit_vital.select";

    void LevoitSelect::setup() {}
    void LevoitSelect::dump_config() {}

    void LevoitSelect::control(const std::string &value)
    {
      switch (this->purpose_)
      {
      case AUTOMODE:
      {
        if (value == "Default")
        {
          parent_->sendCommand(setAutoModeDefault);
        }
        else if (value == "Quiet")
        {
          parent_->sendCommand(setAutomodeQuiet);
        }
        else if (value == "Efficient")
        {
          parent_->sendCommand(setAutoModeEfficient);
        }
        break;
      }
      }
      ESP_LOGI(TAG, "Selected option: %s", value.c_str());
    }

  } // namespace levoit_vital
} // namespace esphome
