#include "logitech_z906.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906";

float LogitechZ906Component::get_setup_priority() const { return setup_priority::DATA; }

void LogitechZ906Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LogitechZ906...");
}

void LogitechZ906Component::loop() {this->feed_console();}

void LogitechZ906Component::dump_config() { ESP_LOGCONFIG(TAG, "LogitechZ906:"); }

void LogitechZ906Component::set_amplifier_uart_parent(uart::UARTComponent *amplifier_uart) {
  this->amplifier_uart_ = amplifier_uart;
  this->z906_.set_amplifier_uart(amplifier_uart);
}
void LogitechZ906Component::set_console_uart_parent(uart::UARTComponent *console_uart) {
  this->console_uart_ = console_uart;
  this->z906_.set_console_uart(console_uart);
}

void LogitechZ906Component::feed_console() {
  if (this->console_uart_->available()) {
    unsigned long last_console_time = millis();
    unsigned long last_amplifier_time = millis();
    while (millis() - last_console_time > 50 || millis() - last_amplifier_time > 50) {
      if (this->console_uart_->available()) {
        uint8_t data = 0;
        this->console_uart_->read_byte(&data);
        this->amplifier_uart_->write_byte(data);
        ESP_LOGD(TAG, "Con: %i", data);
        last_console_time = millis();
      }
      if (this->amplifier_uart_->available()) {
        uint8_t data = 0;
        this->amplifier_uart_->read_byte(&data);
        this->console_uart_->write_byte(data);
        ESP_LOGD(TAG, "Amp: %i", data);
        last_amplifier_time = millis();
      }
    }
  }
}

void LogitechZ906Component::set_select(SelectType type, select::Select *select) {
  switch (type) {
    case SOURCE:
      this->source_ = select;
      break;
    case EFFECT:
      this->effect_ = select;
      break;
  }
}

void LogitechZ906Component::set_select_value(SelectType type, const std::string &value) {
  switch (type) {
    case SOURCE:
      this->set_source(value);
      break;
    case EFFECT:
      this->set_effect(value);
      break;
  }
}

void LogitechZ906Component::set_volume_number(NumberType type, number::Number *number) {
  switch (type) {
    case MASTER:
      this->master_volume_ = number;
      break;
    case BASS:
      this->bass_volume_ = number;
      break;
    case CENTER:
      this->center_volume_ = number;
      break;
    case REAR:
      this->rear_volume_ = number;
      break;
  }
}

void LogitechZ906Component::set_number_value(NumberType type, float value) {
  switch (type) {
    case MASTER:
      this->set_volume_master(value);
      break;
    case BASS:
      this->set_volume_bass(value);
      break;
    case CENTER:
      this->set_volume_center(value);
      break;
    case REAR:
      this->set_volume_rear(value);
      break;
  }
}

void LogitechZ906Component::set_switch(SwitchType type, switch_::Switch *switch_component) {
  switch (type) {
    case POWER:
      this->power_ = switch_component;
      break;
    case MUTE:
      this->mute_ = switch_component;
      break;
  }
}

void LogitechZ906Component::set_switch_state(SwitchType type, bool state) {
  switch (type) {
    case POWER:
      this->set_power(state);
      break;
    case MUTE:
      this->set_mute(state);
      break;
  }
}

void LogitechZ906Component::set_source(const std::string &source) {
  ESP_LOGD(TAG, "Setting source to %s", source.c_str());

  if (source == "Aux") {
    this->z906_.cmd(SELECT_INPUT_AUX);
  } else if (source == "Line") {
    this->z906_.cmd(SELECT_INPUT_1);
  } else if (source == "Chinch") {
    this->z906_.cmd(SELECT_INPUT_2);
  } else if (source == "Optical1") {
    this->z906_.cmd(SELECT_INPUT_3);
  } else if (source == "Optical2") {
    this->z906_.cmd(SELECT_INPUT_4);
  } else if (source == "Coaxial") {
    this->z906_.cmd(SELECT_INPUT_5);
  }
}
void LogitechZ906Component::set_effect(const std::string &effect) {
  ESP_LOGD(TAG, "Setting effect to %s", effect.c_str());

  if (effect == "3D") {
    this->z906_.cmd(SELECT_EFFECT_3D);
  } else if (effect == "2.1") {
    this->z906_.cmd(SELECT_EFFECT_21);
  } else if (effect == "4.1") {
    this->z906_.cmd(SELECT_EFFECT_41);
  } else if (effect == "None") {
    this->z906_.cmd(SELECT_EFFECT_NO);
  }
}

void LogitechZ906Component::set_volume_rear(float volume) {
  ESP_LOGD(TAG, "Setting rear volume to %f", volume);
  this->z906_.cmd(READ_LEVEL, (uint8_t) volume);
}
void LogitechZ906Component::set_volume_center(float volume) {
  ESP_LOGD(TAG, "Setting center volume to %f", volume);
  this->z906_.cmd(CENTER_LEVEL, (uint8_t) volume);
}
void LogitechZ906Component::set_volume_bass(float volume) {
  ESP_LOGD(TAG, "Setting bass volume to %f", volume);
  this->z906_.cmd(SUB_LEVEL, (uint8_t) volume);
}
void LogitechZ906Component::set_volume_master(float volume) {
  ESP_LOGD(TAG, "Setting master volume to %f", volume);
  this->z906_.cmd(MAIN_LEVEL, (uint8_t) volume);
}

void LogitechZ906Component::set_power(bool power) {
  ESP_LOGD(TAG, "Setting power to %s", power ? "ON" : "OFF");
  if (power) {
    this->z906_.cmd(PWM_ON);
  } else {
    this->z906_.cmd(PWM_OFF);
  }
}
void LogitechZ906Component::set_mute(bool mute) {
  ESP_LOGD(TAG, "Setting mute to %s", mute ? "ON" : "OFF");
  if (mute) {
    this->z906_.cmd(MUTE_ON);
  } else {
    this->z906_.cmd(MUTE_OFF);
  }
}

}  // namespace logitech_z906
}  // namespace esphome
