#include "logitech_z906_select.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906.select";

void LogitechZ906Select::dump_config() { LOG_SELECT("", "LogitechZ906 Select", this); }

void LogitechZ906Select::set_select_type(const std::string &type) { this->type_ = resolve_select_type(type); }
void LogitechZ906Select::set_parent(LogitechZ906Component *parent) { this->parent_ = parent; };
void LogitechZ906Select::registrate() { this->parent_->set_select(this->type_, this); }

void LogitechZ906Select::control(const std::string &value) { this->parent_->set_select_value(this->type_, value); }

}  // namespace logitech_z906
}  // namespace esphome
