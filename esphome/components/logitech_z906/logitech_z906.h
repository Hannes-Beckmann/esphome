#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/select/select.h"
#include "esphome/components/number/number.h"
#include "esphome/components/switch/switch.h"
#include "logitech_z906_component_types.h"
#include "logitech_z906_state.h"

#include "Z906.h"

namespace esphome {
namespace logitech_z906 {

class LogitechZ906Component : public Component{
 public:
  float get_setup_priority() const override;

  void setup() override;
  void loop() override;
  void dump_config() override;

  void feed_console();

  void set_amplifier_uart_parent(uart::UARTComponent *amplifier_uart);
  void set_console_uart_parent(uart::UARTComponent *console_uart);
  
  void set_select_value(SelectType type, const std::string &value);
  void set_select(SelectType type, select::Select *select);

  void set_number_value(NumberType type, float value);
  void set_volume_number(NumberType type, number::Number *volume_number);

  void set_switch_state(SwitchType type, bool state);
  void set_switch(SwitchType type, switch_::Switch *switch_component);

  void set_source(const std::string &source);
  void set_effect(const std::string &effect);


  void set_volume(float* current_volume, float value, uint8_t cmd_up, uint8_t cmd_down);

  void set_power(bool power);
  void set_mute(bool mute);

  void update_internal_state(State* state);
  void publish_internal_state();

  void synchronize_console_command(uint8_t cmd);
  void synchronize_volume_command(float* console_volume, float* amplifier_volume, uint8_t cmd, int8_t direction);
  void synchronize_input_command(Input* console_input, Input* amplifier_input, const Input& input_to_set, uint8_t cmd);
  void synchronize_effect_command(Effect* console_effect, Effect* amplifier_effect, const Effect& effect_to_set, uint8_t cmd);


 protected:
  State state_;
  State console_state_;
  float master_volume_before_mute_;

  bool do_synchronization_when_communication_clear = false;
  bool force_update = false;

  uart::UARTComponent *console_uart_{nullptr};
  uart::UARTComponent *amplifier_uart_{nullptr};

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