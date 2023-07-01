#pragma once

#include "../logitech_z906.h"
#include "../logitech_z906_component_types.h"

#include "esphome/components/number/number.h"
#include "esphome/core/component.h"

namespace esphome {
namespace logitech_z906 {

class LogitechZ906Number : public number::Number, public Component {
 public:
  void set_parent(LogitechZ906Component *parent);
  void set_number_type(const std::string &type);
  void registrate();
  void dump_config() override;

 protected:
  void control(float value) override;
  LogitechZ906Component *parent_;
  NumberType type_;
};

}  // namespace logitech_z906
}  // namespace esphome
