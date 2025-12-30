#pragma once
#include "esphome/core/component.h"
#include "esphome/components/button/button.h"
#include "../levoit_vital.h"
#include "../enums.h"

namespace esphome
{
    namespace levoit_vital
    {
        class LevoitButton : public button::Button, public Component
        {
        public:
            LevoitButton(LevoitVital *parent, LevoitButtonPurpose purpose) : parent_(parent), purpose_(purpose) {}
            void setup() override;
            void dump_config() override;
            void press_action() override;

        protected:
            LevoitVital *parent_;
            LevoitButtonPurpose purpose_;
        };

    } // namespace levoit
} // namespace esphome
