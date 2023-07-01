#include "Z906.h"
#include "esphome/core/log.h"

namespace esphome {
namespace logitech_z906 {

Z906::Z906() {}

void Z906::set_uart(uart::UARTDevice *uart) {
  this->uart_ = uart;
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
  this->uart_->write(GET_STATUS);

  unsigned long currentMillis = millis();

  while (this->uart_->available() < STATUS_TOTAL_LENGTH)
    if (millis() - currentMillis > SERIAL_TIME_OUT)
      return 0;

  for (int i = 0; i < STATUS_TOTAL_LENGTH; i++)
    status[i] = this->uart_->read();

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
  this->uart_->write(cmd);

  unsigned long currentMillis = millis();

  while (this->uart_->available() == 0)
    if (millis() - currentMillis > SERIAL_TIME_OUT)
      return 0;

  return this->uart_->read();
}

int Z906::cmd(uint8_t cmd_a, uint8_t cmd_b) {
  update();

  status[cmd_a] = cmd_b;

  status[STATUS_CHECKSUM] = LRC(status, STATUS_TOTAL_LENGTH);

  for (int i = 0; i < STATUS_TOTAL_LENGTH; i++)
    this->uart_->write(status[i]);

  unsigned long currentMillis = millis();

  while (this->uart_->available() < ACK_TOTAL_LENGTH)
    if (millis() - currentMillis > SERIAL_TIME_OUT)
      return 0;

  for (int i = 0; i < ACK_TOTAL_LENGTH; i++)
    this->uart_->read();
}

void Z906::log_status() {
  update();

  for (int i = 0; i < STATUS_TOTAL_LENGTH; i++) {
    ESP_LOGD("logitech_z906", "status %d: %x", i, status[i]);
  }
}

uint8_t Z906::main_sensor() {
  this->uart_->write(GET_TEMP);

  unsigned long currentMillis = millis();

  while (this->uart_->available() < TEMP_TOTAL_LENGTH)
    if (millis() - currentMillis > SERIAL_TIME_OUT)
      return 0;

  uint8_t temp[TEMP_TOTAL_LENGTH];

  for (int i = 0; i < TEMP_TOTAL_LENGTH; i++)
    temp[i] = this->uart_->read();

  if (temp[2] != EXP_MODEL_TEMP)
    return 0;

  return temp[7];
}

}  // namespace logitech_z906

}  // namespace esphome