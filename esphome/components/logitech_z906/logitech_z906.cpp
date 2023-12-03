#include "logitech_z906.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906";

float LogitechZ906Component::get_setup_priority() const { return setup_priority::DATA; }

void LogitechZ906Component::setup() {
  // if (!this->z906_.update()){
  //   mark_failed();
  //   return;
  // }
  // this->update_internal_state();
  // this->publish_internal_state();
}

void LogitechZ906Component::loop() {}

void LogitechZ906Component::dump_config() { ESP_LOGCONFIG(TAG, "LogitechZ906:"); }

void LogitechZ906Component::set_amplifier_uart_parent(uart::UARTComponent *amplifier_uart) {
  this->amplifier_uart_ = amplifier_uart;
  this->z906_.set_amplifier_uart(amplifier_uart);
}

void LogitechZ906Component::update_internal_state() {
  //TODO: Check if you can do this conversion from input
  this->state_.input = (Input) this->z906_.status[STATUS_CURRENT_INPUT];
  this->state_.effect[0] = (Effect) this->z906_.status[STATUS_FX_INPUT_1];
  this->state_.effect[1] = (Effect) this->z906_.status[STATUS_FX_INPUT_2];
  this->state_.effect[2] = (Effect) this->z906_.status[STATUS_FX_INPUT_3];
  this->state_.effect[3] = (Effect) this->z906_.status[STATUS_FX_INPUT_4];
  this->state_.effect[4] = (Effect) this->z906_.status[STATUS_FX_INPUT_5];
  this->state_.effect[5] = (Effect) this->z906_.status[STATUS_FX_INPUT_AUX];
  this->state_.master_volume = this->z906_.status[STATUS_MAIN_LEVEL];
  this->state_.bass_volume = this->z906_.status[STATUS_SUB_LEVEL];
  this->state_.center_volume = this->z906_.status[STATUS_CENTER_LEVEL];
  this->state_.rear_volume = this->z906_.status[STATUS_READ_LEVEL];
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
      this->source_->publish_state(input_to_string(state_.input));
      break;
    case EFFECT:
      this->set_effect(value);
      this->effect_->publish_state(effect_to_string(this->state_.effect[this->state_.input]));
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
      //umute if master volume is adjusted
      this->set_mute(false);
      this->set_volume(this->state_.master_volume, value, LEVEL_MAIN_UP, LEVEL_MAIN_DOWN);
      this->state_.master_volume = (uint8_t) value;
      this->master_volume_->publish_state(this->state_.master_volume);
      break;
    case BASS:
      this->set_volume(this->state_.bass_volume, value, LEVEL_SUB_UP, LEVEL_SUB_DOWN);
      this->state_.bass_volume = (uint8_t) value;
      this->bass_volume_->publish_state(this->state_.bass_volume);
      break;
    case CENTER:
      this->set_volume(this->state_.center_volume, value, LEVEL_CENTER_UP, LEVEL_CENTER_DOWN);
      this->state_.center_volume = (uint8_t) value;
      this->center_volume_->publish_state(this->state_.center_volume);
      break;
    case REAR:
      this->set_volume(this->state_.rear_volume, value, LEVEL_REAR_UP, LEVEL_REAR_DOWN);
      this->state_.rear_volume = (uint8_t) value;
      this->rear_volume_->publish_state(this->state_.rear_volume);
      break;
  }
}


void LogitechZ906Component::set_binary_output_parent(output::BinaryOutput *binary_output){
  this->power_output_ = binary_output;
}

void LogitechZ906Component::set_switch(SwitchType type, switch_::Switch *switch_component) {
  switch (type) {
    case STANDBY:
      this->standby_ = switch_component;
      break;
    case POWER:
      this->standby_ = switch_component;
      break;
    case MUTE:
      this->mute_ = switch_component;
      break;
  }
}

void LogitechZ906Component::set_switch_state(SwitchType type, bool state) {
  switch (type) {
    case STANDBY:
      this->set_standby(state);
      this->standby_->publish_state(this->state_.standby);
      ESP_LOGD(TAG, "Standby is state: %s", this->state_.standby ? "true" : "false");
      break;
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
  }
  this->state_.input = input;
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
  }
  this->state_.effect[this->state_.input] = effect_e;
}

void LogitechZ906Component::set_volume(float current_volume, float value, uint8_t cmd_up, uint8_t cmd_down) {
  //TODO: make this async
  ESP_LOGD(TAG, "Setting volume to %f", value);
  if (this->state_.power){
    int8_t amount = ((uint8_t) value) - current_volume;
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
  }
}

void LogitechZ906Component::set_power(bool power){
  ESP_LOGD(TAG, "Setting power to %s", power ? "ON" : "OFF");
  if (power){
    this->power_output_->turn_on();
    //TODO: make this async
    unsigned long timeout_millis = millis();
    while (!this->z906_.is_powered_on()){
      delay(1000);
      if (millis() - timeout_millis > 100000){
        return;
      }
    }
    this->state_.power = true;
    //this->z906_.set_state((uint8_t)this->state_.master_volume, (uint8_t)this->state_.bass_volume,(uint8_t) this->state_.rear_volume,(uint8_t) this->state_.center_volume, (uint8_t)this->state_.input, (uint8_t*)this->state_.effect);
    if(!this->state_.standby){
      this->set_standby(false);
    }
    //TODO: Check if z906 update is needed 
  }
  else{
    this->set_mute(true);
    //TODO: Maybe an eeprom save is neccessary here. Since we restore state in when powering on this might not be neccessary. This would be useful to restore state if the sp was turned off.  
    this->power_output_->turn_off();
  }
}

void LogitechZ906Component::set_standby(bool standby) {
  ESP_LOGD(TAG, "Setting standby to %s", standby ? "ON" : "OFF");
  uint8_t cmd = standby ? PWM_ON : PWM_OFF;
  if (this->state_.power){
    bool mute_state_before = this->state_.mute;
    this->set_mute(true);
    this->z906_.cmd(cmd);
    this->set_mute(mute_state_before);
  }
  this->state_.standby = standby;
}
void LogitechZ906Component::set_mute(bool mute) {
  ESP_LOGD(TAG, "Setting mute to %s", mute ? "ON" : "OFF");
  if (this->state_.power){
    if(!this->state_.mute && mute || this->state_.mute && !mute){
      if (mute) {
        this->set_volume(this->state_.master_volume, 0, LEVEL_MAIN_UP, LEVEL_MAIN_DOWN);
        //this->z906_.cmd(MUTE_ON);
      } else {
        this->set_volume(0, this->state_.master_volume, LEVEL_MAIN_UP, LEVEL_MAIN_DOWN);
        //this->z906_.cmd(MUTE_OFF);
      }
    }
  }
  this->state_.mute = mute;
}

}  // namespace logitech_z906
}  // namespace esphome
