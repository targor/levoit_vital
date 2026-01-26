#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "../levoit_vital.h"

namespace esphome
{
    namespace levoit_vital
    {

        class LevoitBinarySensor : public binary_sensor::BinarySensor, public Component
        {
        public:
            LevoitBinarySensor() = default;
            LevoitBinarySensor(LevoitVital *parent, LevoitBinarySensorPurpose purpose) : parent_(parent), purpose_(purpose) {}
            void set_parent(LevoitVital *parent) { this->parent_ = parent; }
            void set_purpose(LevoitBinarySensorPurpose purpose) { this->purpose_ = purpose; }
            void dump_config() override;

        protected:
            LevoitVital *parent_;
            LevoitBinarySensorPurpose purpose_;
        };

    } // namespace levoit_vital
} // namespace esphome
