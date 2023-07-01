#pragma once

#include "../logitech_z906.h"

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace logitech_z906 {

class LogitechZ906Switch : public switch_::Switch, public Component {
 public:
  void set_parent(LogitechZ906Component *parent);
  void dump_config() override;
  void set_switch_type(const std::string &type);
  void registrate();

 protected:
  void write_state(bool state) override;
  LogitechZ906Component *parent_;
  SwitchType type_;
};

}  // namespace logitech_z906
}  // namespace esphome
