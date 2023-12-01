#include "logitech_z906.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906";

float LogitechZ906Component::get_setup_priority() const { return setup_priority::DATA; }

void LogitechZ906Component::setup() {
  // Udp.begin(localUdpPort);
  // Udp.beginPacket("192.168.179.4", 5007);
  // Udp.print("Wifi connected");
  // Udp.endPacket();
}

void LogitechZ906Component::loop() { this->feed_console(); }

void LogitechZ906Component::dump_config() { ESP_LOGCONFIG(TAG, "LogitechZ906:"); }

void LogitechZ906Component::set_amplifier_uart_parent(uart::UARTComponent *amplifier_uart) {
  this->amplifier_uart_ = amplifier_uart;
  this->z906_.set_amplifier_uart(amplifier_uart);
  this->z906_.update();
  this->update_internal_state();
}

void LogitechZ906Component::update_internal_state() {
  this->state_->input = (Input) this->z906_.status[STATUS_CURRENT_INPUT];
  this->state_->effect[0] = (Effect) this->z906_.status[STATUS_FX_INPUT_1];
  this->state_->effect[1] = (Effect) this->z906_.status[STATUS_FX_INPUT_2];
  this->state_->effect[2] = (Effect) this->z906_.status[STATUS_FX_INPUT_3];
  this->state_->effect[3] = (Effect) this->z906_.status[STATUS_FX_INPUT_4];
  this->state_->effect[4] = (Effect) this->z906_.status[STATUS_FX_INPUT_5];
  this->state_->effect[5] = (Effect) this->z906_.status[STATUS_FX_INPUT_AUX];
  this->state_->master_volume = this->z906_.status[STATUS_MAIN_LEVEL];
  this->state_->bass_volume = this->z906_.status[STATUS_SUB_LEVEL];
  this->state_->center_volume = this->z906_.status[STATUS_CENTER_LEVEL];
  this->state_->rear_volume = this->z906_.status[STATUS_READ_LEVEL];
  // state->power = this->z906_.status[STATUS_STBY] == 0;
}

void LogitechZ906Component::publish_internal_state() {
  this->source_->publish_state(input_to_string(this->state_.input));
  this->effect_->publish_state(effect_to_string(this->state_.effect[this->state_.input]));
  this->master_volume_->publish_state(this->state_.master_volume);
  this->bass_volume_->publish_state(this->state_.bass_volume);
  this->center_volume_->publish_state(this->state_.center_volume);
  this->rear_volume_->publish_state(this->state_.rear_volume);
  this->power_->publish_state(this->state_.power);
  this->mute_->publish_state(this->state_.mute);
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
      this->set_volume(&(this->state_.master_volume), value, LEVEL_MAIN_UP, LEVEL_MAIN_DOWN);
      this->master_volume_->publish_state(this->state_.master_volume);
      break;
    case BASS:
      this->set_volume(&(this->state_.bass_volume), value, LEVEL_SUB_UP, LEVEL_SUB_DOWN);
      this->bass_volume_->publish_state(this->state_.bass_volume);
      break;
    case CENTER:
      this->set_volume(&(this->state_.center_volume), value, LEVEL_CENTER_UP, LEVEL_CENTER_DOWN);
      this->center_volume_->publish_state(this->state_.center_volume);
      break;
    case REAR:
      this->set_volume(&(this->state_.rear_volume), value, LEVEL_REAR_UP, LEVEL_REAR_DOWN);
      this->rear_volume_->publish_state(this->state_.rear_volume);
      break;
  }
  this->force_update = true;
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
      this->power_->publish_state(this->state_.power);
      ESP_LOGD(TAG, "Power is state: %s", this->state_.power ? "true" : "false");
      break;
    case MUTE:
      this->set_mute(state);
      this->mute_->publish_state(this->state_.mute);
      break;
  }
}

void LogitechZ906Component::set_source(const std::string &source) {
  ESP_LOGD(TAG, "Setting source to %s", source.c_str());
  Input input = string_to_input(source);
  switch (input) {
    case AUX:
      this->z906_.cmd(SELECT_INPUT_AUX);
      break;
    case LINE:
      this->z906_.cmd(SELECT_INPUT_1);
      break;
    case CHINCH:
      this->z906_.cmd(SELECT_INPUT_2);
      break;
    case OPTICAL1:
      this->z906_.cmd(SELECT_INPUT_3);
      break;
    case OPTICAL2:
      this->z906_.cmd(SELECT_INPUT_4);
      break;
    case COAXIAL:
      this->z906_.cmd(SELECT_INPUT_5);
      break;
      this->state_.input = input;
      this->source_->publish_state(source);
  }
}

void LogitechZ906Component::set_effect(const std::string &effect) {
  ESP_LOGD(TAG, "Setting effect to %s", effect.c_str());
  Effect effect_e = string_to_effect(effect);
  switch (effect_e) {
    case NONE:
      this->z906_.cmd(SELECT_EFFECT_NO);
      break;
    case EFFECT_3D:
      this->z906_.cmd(SELECT_EFFECT_3D);
      break;
    case EFFECT_21:
      this->z906_.cmd(SELECT_EFFECT_21);
      break;
    case EFFECT_41:
      this->z906_.cmd(SELECT_EFFECT_41);
      break;

      this->state_.effect[this->state_.input] = effect_e;
      this->effect_->publish_state(effect);
  }
}

void LogitechZ906Component::set_volume(float *current_volume, float value, uint8_t cmd_up, uint8_t cmd_down) {
  ESP_LOGD(TAG, "Setting volume to %f", value);
  int8_t amount = ((uint8_t) value) - *current_volume;
  ESP_LOGD(TAG, "Amount: %d", amount);
  uint8_t cmd = amount > 0 ? cmd_up : cmd_down;
  if (amount < 0) {
    for (int i = 0; i > amount; i--) {
      ESP_LOGD(TAG, "Iteration: %d", i);
      this->z906_.cmd(cmd_down);
      delay(20);
    }
  } else {
    for (int i = 0; i < amount; i++) {
      ESP_LOGD(TAG, "Iteration: %d", i);
      this->z906_.cmd(cmd_up);
      delay(20);
    }
  }
  *current_volume += amount;
}

void LogitechZ906Component::set_power(bool power) {
  ESP_LOGD(TAG, "Setting power to %s", power ? "ON" : "OFF");
  if (!power && this->state_.power || power && !this->state_.power) {
    uint8_t cmd = power ? PWM_ON : PWM_OFF;
    if (this->state_.master_volume > 0) {
      this->set_mute(true);
    }
    this->z906_.cmd(cmd);
    this->z906_.cmd(cmd);
    this->set_mute(false);
    this->state_.power = power;
  }
}
void LogitechZ906Component::set_mute(bool mute) {
  ESP_LOGD(TAG, "Setting mute to %s", mute ? "ON" : "OFF");
  if (mute) {
    this->master_volume_before_mute_ = this->state_.master_volume;
    //this->set_volume(&(this->state_.master_volume), 0, LEVEL_MAIN_UP, LEVEL_MAIN_DOWN);
    this->z906_.cmd(MUTE_ON);
  } else {
    //this->set_volume(&(this->state_.master_volume), this->master_volume_before_mute_, LEVEL_MAIN_UP, LEVEL_MAIN_DOWN);
    this->z906_.cmd(MUTE_OFF);
  }
  this->state_.mute = mute;
}

}  // namespace logitech_z906
}  // namespace esphome
