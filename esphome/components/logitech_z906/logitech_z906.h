#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/select/select.h"

namespace esphome {
namespace logitech_z906 {

class LogitechZ906Component : public Component, public uart::UARTDevice {
 public:
  float get_setup_priority() const override;

  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_source(const std::string &source);
  void set_source_select(select::Select *source_select);

 protected:
  select::Select *source_select_;
};

}  // namespace logitech_z906

}  // namespace esphome