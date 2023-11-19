#include "Z906.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "logitech_z906";

Z906::Z906() {}

void Z906::set_amplifier_uart(uart::UARTComponent *uart) {
  this->amplifier_uart_ = uart;
  // dev_serial->begin(BAUD_RATE,SERIAL_CONFIG);
}

void Z906::set_console_uart(uart::UARTComponent *uart) {
  this->console_uart_ = uart;
  // dev_serial->begin(BAUD_RATE,SERIAL_CONFIG);
}

// Longitudinal Redundancy Check {-1,-1}

uint8_t Z906::LRC(uint8_t *pData, uint8_t length) {
  uint8_t LRC = 0;

  for (int i = 1; i < length - 1; i++)
    LRC -= pData[i];

  return LRC;
}



int Z906::update() {
  ESP_LOGD(TAG, "Fetch update");
  ESP_LOGD(TAG, "Middle: %x", GET_STATUS);
  this->amplifier_uart_->write_byte(GET_STATUS);

  this->receive_status();
}

int Z906::receive_status() {
  unsigned long currentMillis = millis();

  while (this->amplifier_uart_->available() < STATUS_TOTAL_LENGTH)
    if (millis() - currentMillis > SERIAL_TIME_OUT)
      return 0;

  for (int i = 0; i < STATUS_TOTAL_LENGTH; i++){
    uint8_t data = 0;
    this->amplifier_uart_->read_byte(&data);
    status[i] = data;
    ESP_LOGD(TAG, "Mid Amp: %x", data);
  }
  if (status[STATUS_STX] != EXP_STX)
    return 0;
  if (status[STATUS_MODEL] != EXP_MODEL_STATUS)
    return 0;
  if (status[STATUS_CHECKSUM] == LRC(status, STATUS_TOTAL_LENGTH))
    return 1;

  return 0;
}

int Z906::request(uint8_t cmd) {
  if (update()) {
    if (cmd == VERSION)
      return status[STATUS_VER_C] + 10 * status[STATUS_VER_B] + 100 * status[STATUS_VER_A];
    if (cmd == CURRENT_INPUT)
      return status[STATUS_CURRENT_INPUT] + 1;

    return status[cmd];
  }
  return 0;
}



int Z906::cmd(uint8_t cmd) {
  ESP_LOGD(TAG, "Single cmd");
  this->amplifier_uart_->write_byte(cmd);
  ESP_LOGD(TAG, "Middle: %x", cmd);
  unsigned long currentMillis = millis();

  while (this->amplifier_uart_->available() == 0){
    if (millis() - currentMillis > SERIAL_TIME_OUT){
      return 0;
    }
  }
  uint8_t data = 0;
  this->amplifier_uart_->read_byte(&data);
  ESP_LOGD(TAG, "Mid Amp: %x", data);
  return data;
}

int Z906::cmd(uint8_t cmd_a, uint8_t cmd_b) {
  ESP_LOGD(TAG, "Long cmd");
  update();

  status[cmd_a] = cmd_b;

  status[STATUS_CHECKSUM] = LRC(status, STATUS_TOTAL_LENGTH);

  for (int i = 0; i < STATUS_TOTAL_LENGTH; i++){
    this->amplifier_uart_->write_byte(status[i]);
    ESP_LOGD(TAG, "Middle: %x", status[i]);
  }
  unsigned long currentMillis = millis();

  while (this->amplifier_uart_->available() < ACK_TOTAL_LENGTH)
    if (millis() - currentMillis > SERIAL_TIME_OUT)
      return 0;

  for (int i = 0; i < ACK_TOTAL_LENGTH; i++){
    uint8_t data = 0;
    this->amplifier_uart_->read_byte(&data);
    ESP_LOGD(TAG, "Mid Amp: %x", data);
  }
  return 1;
}

void Z906::log_status() {
  update();

  for (int i = 0; i < STATUS_TOTAL_LENGTH; i++) {
    ESP_LOGD("logitech_z906", "status %d: %x", i, status[i]);
  }
}

uint8_t Z906::main_sensor() {
  this->amplifier_uart_->write_byte(GET_TEMP);

  unsigned long currentMillis = millis();

  while (this->amplifier_uart_->available() < TEMP_TOTAL_LENGTH)
    if (millis() - currentMillis > SERIAL_TIME_OUT)
      return 0;

  uint8_t temp[TEMP_TOTAL_LENGTH];

  for (int i = 0; i < TEMP_TOTAL_LENGTH; i++){
    uint8_t data = 0;
    this->amplifier_uart_->read_byte(&data);
    temp[i] = data;
  }

  if (temp[2] != EXP_MODEL_TEMP)
    return 0;

  return temp[7];
}

}  // namespace logitech_z906

}  // namespace esphome