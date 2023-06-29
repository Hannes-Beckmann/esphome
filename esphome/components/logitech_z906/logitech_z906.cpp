#include "logitech_z906.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906";

void LogitechZ906Component::setup() { ESP_LOGCONFIG(TAG, "Setting up LogitechZ906..."); }

void LogitechZ906Component::loop() {}

void LogitechZ906Component::dump_config() { ESP_LOGCONFIG(TAG, "LogitechZ906:"); }

void LogitechZ906Component::set_source(const std::string &source) {
  ESP_LOGD(TAG, "Setting source to %s", source.c_str());
}

void LogitechZ906Component::set_source_select(select::Select *source_select) { this->source_select_ = source_select; }

}  // namespace logitech_z906
}  // namespace esphome
