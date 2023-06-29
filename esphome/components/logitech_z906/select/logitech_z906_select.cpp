#include "logitech_z906_select.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906.select";

void LogitechZ906Select::dump_config() { LOG_SELECT("", "LogitechZ906 Select", this); }
void LogitechZ906Select::control(const std::string &value) { this->parent_->set_source(value); }

}  // namespace logitech_z906
}  // namespace esphome
