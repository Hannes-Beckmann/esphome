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

void LogitechZ906Component::update_internal_state() {
  this->state_.input = (Input)this->z906_.status[STATUS_CURRENT_INPUT];
  this->state_.effect[0] = (Effect)this->z906_.status[STATUS_FX_INPUT_1];
  this->state_.effect[1] = (Effect)this->z906_.status[STATUS_FX_INPUT_2];
  this->state_.effect[2] = (Effect)this->z906_.status[STATUS_FX_INPUT_3];
  this->state_.effect[3] = (Effect)this->z906_.status[STATUS_FX_INPUT_4];
  this->state_.effect[4] = (Effect)this->z906_.status[STATUS_FX_INPUT_5];
  this->state_.effect[5] = (Effect)this->z906_.status[STATUS_FX_INPUT_AUX];
  this->state_.master_volume = this->z906_.status[STATUS_MAIN_LEVEL];
  this->state_.bass_volume = this->z906_.status[STATUS_SUB_LEVEL];
  this->state_.center_volume = this->z906_.status[STATUS_CENTER_LEVEL];
  this->state_.rear_volume = this->z906_.status[STATUS_READ_LEVEL];
  this->state_.power = this->z906_.status[STATUS_STBY] == 0;
  this->do_synchronization = true;
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

void LogitechZ906Component::synchronize_console_command(uint8_t cmd) {
  switch (cmd){
    case SELECT_INPUT_AUX:
      this->state_.input = Input::AUX;
      this->source_->publish_state(input_to_string(Input::AUX));
      break;
    case SELECT_INPUT_1:
      this->state_.input = Input::LINE;
      this->source_->publish_state(input_to_string(Input::LINE));
      break;
    case SELECT_INPUT_2:
      this->state_.input = Input::CHINCH;
      this->source_->publish_state(input_to_string(Input::CHINCH));
      break;
    case SELECT_INPUT_3:
      this->state_.input = Input::OPTICAL1;
      this->source_->publish_state(input_to_string(Input::OPTICAL1));
      break;
    case SELECT_INPUT_4:
      this->state_.input = Input::OPTICAL2;
      this->source_->publish_state(input_to_string(Input::OPTICAL2));
      break;
    case SELECT_INPUT_5:
      this->state_.input = Input::COAXIAL;
      this->source_->publish_state(input_to_string(Input::COAXIAL));
      break;
    case SELECT_EFFECT_NO:
      this->state_.effect[this->state_.input] = Effect::NONE;
      this->effect_->publish_state(effect_to_string(Effect::NONE));
      break;
    case SELECT_EFFECT_3D:
      this->state_.effect[this->state_.input] = Effect::EFFECT_3D;
      this->effect_->publish_state(effect_to_string(Effect::EFFECT_3D));
      break;
    case SELECT_EFFECT_21:
      this->state_.effect[this->state_.input] = Effect::EFFECT_21;
      this->effect_->publish_state(effect_to_string(Effect::EFFECT_21));
      break;
    case SELECT_EFFECT_41:
      this->state_.effect[this->state_.input] = Effect::EFFECT_41;
      this->effect_->publish_state(effect_to_string(Effect::EFFECT_41));
      break;
    case LEVEL_MAIN_UP:
      this->state_.master_volume += this->state_.master_volume < 43 ? 1 : 0;
      this->master_volume_->publish_state(this->state_.master_volume);
      break;
    case LEVEL_MAIN_DOWN:
      this->state_.master_volume -= this->state_.master_volume > 0 ? 1 : 0;
      this->master_volume_->publish_state(this->state_.master_volume);
      break;
    case LEVEL_SUB_UP:
      this->state_.bass_volume += this->state_.bass_volume < 43 ? 1 : 0;
      this->bass_volume_->publish_state(this->state_.bass_volume + 1);
      break;
    case LEVEL_SUB_DOWN:
      this->state_.bass_volume -= this->state_.bass_volume > 0 ? 1 : 0;
      this->bass_volume_->publish_state(this->state_.bass_volume - 1);
      break;
    case LEVEL_CENTER_UP:
      this->state_.center_volume += this->state_.center_volume < 43 ? 1 : 0;
      this->center_volume_->publish_state(this->state_.center_volume + 1);
      break;
    case LEVEL_CENTER_DOWN:
      this->state_.center_volume -= this->state_.center_volume > 0 ? 1 : 0;
      this->center_volume_->publish_state(this->state_.center_volume - 1);
      break;
    case LEVEL_REAR_UP:
      this->state_.rear_volume += this->state_.rear_volume < 43 ? 1 : 0;
      this->rear_volume_->publish_state(this->state_.rear_volume + 1);
      break;
    case LEVEL_REAR_DOWN:
      this->state_.rear_volume -= this->state_.rear_volume > 0 ? 1 : 0;
      this->rear_volume_->publish_state(this->state_.rear_volume - 1);
      break;
    case MUTE_ON:
      this->state_.mute = true;
      this->mute_->publish_state(true);
      break;
    case MUTE_OFF:
      this->state_.mute = false;
      this->mute_->publish_state(false);
      break;
    case PWM_ON:
      this->state_.power = true;
      this->power_->publish_state(true);
      break;
    case PWM_OFF:
      this->state_.power = false;
      this->power_->publish_state(false);
      break;
  }
}

void LogitechZ906Component::feed_console() {
  if (this->console_uart_->available()) {
    unsigned long last_console_time = millis();
    unsigned long last_amplifier_time = millis();
    uint8_t status[STATUS_TOTAL_LENGTH];
    uint8_t cmd = 0;
    this->amplifier_uart_->flush();
    while (millis() - last_console_time < 50 || millis() - last_amplifier_time < 50) {
      if (this->console_uart_->available()) {
        uint8_t data = 0;
        this->console_uart_->read_byte(&data);
        this->amplifier_uart_->write_byte(data);
        ESP_LOGD(TAG, "Feed Con: %x", data);
        if (data == GET_STATUS) {
          this->z906_.receive_status();
        }
        cmd = data != RESET_PWR_UP_TIME ? data : cmd;
        last_console_time = millis();
      }
      if (this->amplifier_uart_->available()) {
        uint8_t data = 0;
        this->amplifier_uart_->read_byte(&data);
        this->console_uart_->write_byte(data);
        ESP_LOGD(TAG, "Feed Amp: %x", data);
        last_amplifier_time = millis();
      }
    }
    if (cmd != 0) {
      if(cmd = GET_STATUS){
        this->update_internal_state();
      }
      else{
        this->synchronize_console_command(cmd);
      }
    }
  }
  else{
    if (this->do_synchronization) {
      this->do_synchronization = false;
      this->publish_internal_state();
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
  this->effect_->publish_state(source);
  this->state_.input = input;
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

void LogitechZ906Component::set_volume_rear(float volume) {
  ESP_LOGD(TAG, "Setting rear volume to %f", volume);
  uint8_t amount = ((uint8_t) volume) - this->state_.rear_volume;
  uint8_t cmd = amount > 0 ? LEVEL_REAR_UP : LEVEL_REAR_DOWN;
  this->turn_volume(cmd, amount);
  this->state_.rear_volume += amount;
  this->rear_volume_->publish_state(this->state_.rear_volume);
}
void LogitechZ906Component::set_volume_center(float volume) {
  ESP_LOGD(TAG, "Setting center volume to %f", volume);
  uint8_t amount = ((uint8_t) volume) - this->state_.center_volume;
  uint8_t cmd = amount > 0 ? LEVEL_CENTER_UP : LEVEL_CENTER_DOWN;
  this->turn_volume(cmd, amount);
  this->state_.center_volume += amount;
  this->center_volume_->publish_state(this->state_.center_volume);
}
void LogitechZ906Component::set_volume_bass(float volume) {
  ESP_LOGD(TAG, "Setting bass volume to %f", volume);
  uint8_t amount = ((uint8_t) volume) - this->state_.bass_volume;
  uint8_t cmd = amount > 0 ? LEVEL_SUB_UP : LEVEL_SUB_DOWN;
  this->turn_volume(cmd, amount);
  this->state_.bass_volume += amount;
  this->bass_volume_->publish_state(this->state_.bass_volume);
}
void LogitechZ906Component::set_volume_master(float volume) {
  ESP_LOGD(TAG, "Setting master volume to %f", volume);
  uint8_t amount = ((uint8_t) volume) - this->state_.master_volume;
  uint8_t cmd = amount > 0 ? LEVEL_MAIN_UP : LEVEL_MAIN_DOWN;
  this->turn_volume(cmd, amount);
  this->state_.master_volume += amount;
  this->master_volume_->publish_state(this->state_.master_volume);
}

void LogitechZ906Component::turn_volume(const uint8_t &cmd, int8_t amount) {
  if (amount < 0) {
    amount = -amount;
    for (int i = 0; i < amount; i++) {
      this->z906_.cmd(cmd);
    }
  } else {
    for (int i = 0; i < amount; i++) {
      this->z906_.cmd(cmd);
    }
  }
}

void LogitechZ906Component::set_power(bool power) {
  ESP_LOGD(TAG, "Setting power to %s", power ? "ON" : "OFF");
  this->state_.power = this->z906_.cmd(power ? PWM_ON : PWM_OFF) ? power : this->state_.power;
  this->power_->publish_state(this->state_.power);
}
void LogitechZ906Component::set_mute(bool mute) {
  ESP_LOGD(TAG, "Setting mute to %s", mute ? "ON" : "OFF");
  this->state_.mute = this->z906_.cmd(mute ? MUTE_ON : MUTE_OFF) ? mute : this->state_.mute;
  this->mute_->publish_state(this->state_.mute);
}

}  // namespace logitech_z906
}  // namespace esphome
