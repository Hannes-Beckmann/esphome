#include "logitech_z906_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906.switch";
void LogitechZ906Number::set_parent(LogitechZ906Component *parent) { this->parent_ = parent; }
void LogitechZ906Number::dump_config() { LOG_NUMBER("", "LogitechZ906 Number", this); }
void LogitechZ906Number::set_number_type(const std::string &type) { this->type_ = resolve_number_type(type); }
void LogitechZ906Number::registrate() { this->parent_->set_volume_number(this->type_, this); }
void LogitechZ906Number::control(float value) {
  this->parent_->set_number_value(this->type_, value);
}

}  // namespace logitech_z906
}  // namespace esphome
