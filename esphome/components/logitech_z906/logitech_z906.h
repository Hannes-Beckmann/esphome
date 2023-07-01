#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/select/select.h"
#include "esphome/components/number/number.h"
#include "esphome/components/switch/switch.h"
#include "logitech_z906_component_types.h"

#include "Z906.h"

namespace esphome {
namespace logitech_z906 {

class LogitechZ906Component : public Component, public uart::UARTDevice {
 public:
  float get_setup_priority() const override;

  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_select_value(SelectType type, const std::string &value);
  void set_select(SelectType type, select::Select *select);

  void set_number_value(NumberType type, float value);
  void set_volume_number(NumberType type, number::Number *volume_number);

  void set_switch_state(SwitchType type, bool state);
  void set_switch(SwitchType type, switch_::Switch *switch_component);

  void set_source(const std::string &source);
  void set_effect(const std::string &effect);

  void set_volume_rear(float volume);
  void set_volume_center(float volume);
  void set_volume_bass(float volume);
  void set_volume_master(float volume);

  void set_power(bool power);
  void set_mute(bool mute);

 protected:
  select::Select *source_;
  select::Select *effect_;

  number::Number *master_volume_;
  number::Number *bass_volume_;
  number::Number *center_volume_;
  number::Number *rear_volume_;

  switch_::Switch *power_;
  switch_::Switch *mute_;

  Z906 z906_;
};

}  // namespace logitech_z906

}  // namespace esphome