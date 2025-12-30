
#include "esphome/core/log.h"
#include "levoit_vital_button.h"
#include "../settings.h"
namespace esphome
{
  namespace levoit_vital
  {

    static const char *const TAG = "levoit_vital.button";

    void LevoitButton::setup() {}
    void LevoitButton::press_action()
    {
      switch (this->purpose_)
      {
      case POWERMODE:
      {
        this->parent_->sendCommand(setPowerMode);
        break;
      }
      }
    }

    void LevoitButton::dump_config() {}
  } // namespace levoit_vital
} // namespace esphome
