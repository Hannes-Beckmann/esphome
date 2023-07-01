#include "logitech_z906_switch.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906.switch";

void LogitechZ906Switch::dump_config() { LOG_SWITCH("", "LogitechZ906 Switch", this); }
void LogitechZ906Switch::set_switch_type(const std::string &type) { this->type_ = resolve_switch_type(type); }
void LogitechZ906Switch::set_parent(LogitechZ906Component *parent) { this->parent_ = parent; }
void LogitechZ906Switch::registrate() { this->parent_->set_switch(this->type_, this); }
void LogitechZ906Switch::write_state(bool state) {
  this->parent_->set_switch_state(this->type_, state);
  publish_state(state);
}

}  // namespace logitech_z906
}  // namespace esphome
