#include "logitech_z906.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906";

float LogitechZ906Component::get_setup_priority() const { return setup_priority::DATA; }

void LogitechZ906Component::setup() { ESP_LOGCONFIG(TAG, "Setting up LogitechZ906..."); }

void LogitechZ906Component::loop() { this->feed_console(); }

void LogitechZ906Component::dump_config() { ESP_LOGCONFIG(TAG, "LogitechZ906:"); }

void LogitechZ906Component::set_amplifier_uart_parent(uart::UARTComponent *amplifier_uart) {
  this->amplifier_uart_ = amplifier_uart;
  this->z906_.set_amplifier_uart(amplifier_uart);
}
void LogitechZ906Component::set_console_uart_parent(uart::UARTComponent *console_uart) {
  this->console_uart_ = console_uart;
  this->z906_.set_console_uart(console_uart);
}

void LogitechZ906Component::update_internal_state(State *state) {
  state->input = (Input) this->z906_.status[STATUS_CURRENT_INPUT];
  state->effect[0] = (Effect) this->z906_.status[STATUS_FX_INPUT_1];
  state->effect[1] = (Effect) this->z906_.status[STATUS_FX_INPUT_2];
  state->effect[2] = (Effect) this->z906_.status[STATUS_FX_INPUT_3];
  state->effect[3] = (Effect) this->z906_.status[STATUS_FX_INPUT_4];
  state->effect[4] = (Effect) this->z906_.status[STATUS_FX_INPUT_5];
  state->effect[5] = (Effect) this->z906_.status[STATUS_FX_INPUT_AUX];
  state->master_volume = this->z906_.status[STATUS_MAIN_LEVEL];
  state->bass_volume = this->z906_.status[STATUS_SUB_LEVEL];
  state->center_volume = this->z906_.status[STATUS_CENTER_LEVEL];
  state->rear_volume = this->z906_.status[STATUS_READ_LEVEL];
  //state->power = this->z906_.status[STATUS_STBY] == 0;
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

void LogitechZ906Component::synchronize_volume_command(float *console_volume, float *amplifier_volume, uint8_t cmd,
                                                       int8_t direction) {
  // when volume is changed by console unmute amplifier
  if (this->state_.mute) {
    this->set_mute(false);
  }
  if (direction > 0) {
    ESP_LOGD(TAG, "Volume Up");
    ESP_LOGD(TAG, "Console Volume: %f", *console_volume);
    *console_volume += *console_volume < 43 ? 1 : 0;
    ESP_LOGD(TAG, "New Console Volume: %f", *console_volume);
    ESP_LOGD(TAG, "Amplifier Volume: %f", *amplifier_volume);
    if (*console_volume > *amplifier_volume) {
      *amplifier_volume += 1;
      this->amplifier_uart_->write_byte(cmd);
    } else {
      // fake receive from amplifier
      this->console_uart_->write_byte(cmd);
    }
  } else if (direction < 0) {
    *console_volume -= *console_volume > 0 ? 1 : 0;
    if (*console_volume < *amplifier_volume) {
      *amplifier_volume -= 1;
      this->amplifier_uart_->write_byte(cmd);
    } else {
      // fake receive from amplifier
      this->console_uart_->write_byte(cmd);
    }
  }
}

void LogitechZ906Component::synchronize_input_command(Input *console_input, Input *amplifier_input,
                                                      const Input &input_to_set, uint8_t cmd) {
  *console_input = input_to_set;
  *amplifier_input = input_to_set;
  this->amplifier_uart_->write_byte(cmd);
}

void LogitechZ906Component::synchronize_effect_command(Effect *console_effect, Effect *amplifier_effect,
                                                       const Effect &effect_to_set, uint8_t cmd) {
  *console_effect = effect_to_set;
  *amplifier_effect = effect_to_set;
  this->amplifier_uart_->write_byte(cmd);
}

void LogitechZ906Component::synchronize_console_command(uint8_t cmd) {
  switch (cmd) {
    case SELECT_INPUT_AUX:
      synchronize_input_command(&(this->console_state_.input), &(this->state_.input), Input::AUX, cmd);
      break;
    case SELECT_INPUT_1:
      synchronize_input_command(&(this->console_state_.input), &(this->state_.input), Input::LINE, cmd);
      break;
    case SELECT_INPUT_2:
      synchronize_input_command(&(this->console_state_.input), &(this->state_.input), Input::CHINCH, cmd);
      break;
    case SELECT_INPUT_3:
      synchronize_input_command(&(this->console_state_.input), &(this->state_.input), Input::OPTICAL1, cmd);
      break;
    case SELECT_INPUT_4:
      synchronize_input_command(&(this->console_state_.input), &(this->state_.input), Input::OPTICAL2, cmd);
      break;
    case SELECT_INPUT_5:
      synchronize_input_command(&(this->console_state_.input), &(this->state_.input), Input::COAXIAL, cmd);
      break;
    case SELECT_EFFECT_NO:
      synchronize_effect_command(&(this->console_state_.effect[this->console_state_.input]),
                                 &(this->state_.effect[this->state_.input]), Effect::NONE, cmd);
      break;
    case SELECT_EFFECT_3D:
      synchronize_effect_command(&(this->console_state_.effect[this->console_state_.input]),
                                 &(this->state_.effect[this->state_.input]), Effect::EFFECT_3D, cmd);
      break;
    case SELECT_EFFECT_21:
      synchronize_effect_command(&(this->console_state_.effect[this->console_state_.input]),
                                 &(this->state_.effect[this->state_.input]), Effect::EFFECT_21, cmd);
      break;
    case SELECT_EFFECT_41:
      synchronize_effect_command(&(this->console_state_.effect[this->console_state_.input]),
                                 &(this->state_.effect[this->state_.input]), Effect::EFFECT_41, cmd);
      break;
    case LEVEL_MAIN_UP:
      synchronize_volume_command(&(this->console_state_.master_volume), &(this->state_.master_volume), cmd, 1);
      break;
    case LEVEL_MAIN_DOWN:
      synchronize_volume_command(&(this->console_state_.master_volume), &(this->state_.master_volume), cmd, -1);
      break;
    case LEVEL_SUB_UP:
      synchronize_volume_command(&(this->console_state_.bass_volume), &(this->state_.bass_volume), cmd, 1);
      break;
    case LEVEL_SUB_DOWN:
      synchronize_volume_command(&(this->console_state_.bass_volume), &(this->state_.bass_volume), cmd, -1);
      break;
    case LEVEL_CENTER_UP:
      synchronize_volume_command(&(this->console_state_.center_volume), &(this->state_.center_volume), cmd, 1);
      break;
    case LEVEL_CENTER_DOWN:
      synchronize_volume_command(&(this->console_state_.center_volume), &(this->state_.center_volume), cmd, -1);
      break;
    case LEVEL_REAR_UP:
      synchronize_volume_command(&(this->console_state_.rear_volume), &(this->state_.rear_volume), cmd, 1);
      break;
    case LEVEL_REAR_DOWN:
      synchronize_volume_command(&(this->console_state_.rear_volume), &(this->state_.rear_volume), cmd, -1);
      break;
    case MUTE_ON:
      this->amplifier_uart_->write_byte(cmd);
      this->state_.mute = true;
      this->console_state_.mute = true;
      break;
    case MUTE_OFF:
      this->amplifier_uart_->write_byte(cmd);
      this->state_.mute = false;
      this->console_state_.mute = false;
      break;
    case PWM_ON:
      this->amplifier_uart_->write_byte(cmd);
      this->state_.power = true;
      this->console_state_.power = true;
      break;
    case PWM_OFF:
      this->amplifier_uart_->write_byte(cmd);
      this->state_.power = false;
      this->console_state_.power = false;
      break;
    case EEPROM_SAVE:
      this->amplifier_uart_->write_byte(cmd);
      this->state_.power = false;
      this->console_state_.power = false;
      break;
    case 0x37:
      this->amplifier_uart_->write_byte(cmd);
      this->state_.power = false;
      this->console_state_.power = false;
      break;
    case GET_STATUS:
      this->z906_.update(false);
      this->do_synchronization_when_communication_clear = true;
      break;
    case GET_PWR_UP_TIME:
      this->time_since_last_reset_request = millis();
      this->amplifier_uart_->write_byte(cmd);
      break;
    case RESET_PWR_UP_TIME:
      this->time_since_last_reset_request = millis();
      this->amplifier_uart_->write_byte(cmd);
      break;
    default:
      this->amplifier_uart_->write_byte(cmd);
  }

  if (cmd == SELECT_INPUT_AUX || cmd == SELECT_INPUT_1 || cmd == SELECT_INPUT_2 || cmd == SELECT_INPUT_3 ||
      cmd == SELECT_INPUT_4 || cmd == SELECT_INPUT_5 || cmd == SELECT_EFFECT_21 || cmd == SELECT_EFFECT_3D ||
      cmd == SELECT_EFFECT_41 || cmd == SELECT_EFFECT_NO || cmd == LEVEL_MAIN_UP || cmd == LEVEL_MAIN_DOWN ||
      cmd == LEVEL_SUB_UP || cmd == LEVEL_SUB_DOWN || cmd == LEVEL_CENTER_UP || cmd == LEVEL_CENTER_DOWN ||
      cmd == LEVEL_REAR_UP || cmd == LEVEL_REAR_DOWN || cmd == MUTE_ON || cmd == MUTE_OFF || cmd == PWM_ON ||
      cmd == PWM_OFF || cmd == EEPROM_SAVE || cmd == 0x37) {
    this->publish_state_when_communication_clear = true;
  }
}

void LogitechZ906Component::feed_console() {
  if (this->console_uart_->available()) {
    unsigned long last_console_time = millis();
    unsigned long last_amplifier_time = millis();
    uint8_t data = 0;
    while (this->amplifier_uart_->available()) {
      this->amplifier_uart_->read_byte(&data);
    }

    while (millis() - last_console_time < 100 || millis() - last_amplifier_time < 100) {
      if (this->console_uart_->available()) {
        this->console_uart_->read_byte(&data);
        ESP_LOGD(TAG, "Feed Con ->: %x", data);
        this->synchronize_console_command(data);
        last_console_time = millis();
      }
      if (this->amplifier_uart_->available()) {
        this->amplifier_uart_->read_byte(&data);
        this->console_uart_->write_byte(data);
        ESP_LOGD(TAG, "Feed Amp ->: %x", data);
        last_amplifier_time = millis();
      }
    }
  } else {
    unsigned long last_reset_duration = millis() - this->time_since_last_reset_request;
    if (this->do_synchronization_when_communication_clear) {
      this->do_synchronization_when_communication_clear = false;
      this->update_internal_state(&(this->state_));
      this->update_internal_state(&(this->console_state_));
      this->publish_state_when_communication_clear = true;
    } else if (this->force_update && last_reset_duration < 57000) {
      this->force_update = false;
      this->z906_.update(true);
      this->update_internal_state(&(this->state_));
      this->publish_state_when_communication_clear = true;
    } else if (this->publish_state_when_communication_clear &&
               (last_reset_duration < 57000 || last_reset_duration > 61000)) {
      this->publish_state_when_communication_clear = false;
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
    this->set_volume(&(this->state_.master_volume), 0, LEVEL_MAIN_UP, LEVEL_MAIN_DOWN);
  } else {
    this->set_volume(&(this->state_.master_volume), this->master_volume_before_mute_, LEVEL_MAIN_UP, LEVEL_MAIN_DOWN);
  }
  this->state_.mute = mute;
}

}  // namespace logitech_z906
}  // namespace esphome
