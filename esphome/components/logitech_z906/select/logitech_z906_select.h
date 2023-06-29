#pragma once

#include "../logitech_z906.h"

#include "esphome/components/select/select.h"
#include "esphome/core/component.h"

namespace esphome {
namespace logitech_z906 {

class LogitechZ906Select : public select::Select, public Component {
 public:
  void set_parent(LogitechZ906Component *parent) { this->parent_ = parent; };
  void dump_config() override;

 protected:
  void control(const std::string &value) override;
  LogitechZ906Component *parent_;
};

}  // namespace logitech_z906
}  // namespace esphome
