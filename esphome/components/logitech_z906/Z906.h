#pragma once
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace logitech_z906 {
// Serial Settings

#define BAUD_RATE 57600
#define SERIAL_CONFIG SERIAL_8O1
#define SERIAL_TIME_OUT 1000

#define STATUS_TOTAL_LENGTH 0x18
#define ACK_TOTAL_LENGTH 0x05
#define TEMP_TOTAL_LENGTH 0x0A

// Single Commands

#define SELECT_INPUT_1 0x02
#define SELECT_INPUT_2 0x05
#define SELECT_INPUT_3 0x03
#define SELECT_INPUT_4 0x04
#define SELECT_INPUT_5 0x06
#define SELECT_INPUT_AUX 0x07

#define LEVEL_MAIN_UP 0x08
#define LEVEL_MAIN_DOWN 0x09
#define LEVEL_SUB_UP 0x0A
#define LEVEL_SUB_DOWN 0x0B
#define LEVEL_CENTER_UP 0x0C
#define LEVEL_CENTER_DOWN 0x0D
#define LEVEL_REAR_UP 0x0E
#define LEVEL_REAR_DOWN 0x0F

#define PWM_OFF 0x10
#define PWM_ON 0x11

#define SELECT_EFFECT_3D 0x14
#define SELECT_EFFECT_41 0x15
#define SELECT_EFFECT_21 0x16
#define SELECT_EFFECT_NO 0x35

#define EEPROM_SAVE 0x36

#define MUTE_ON 0x38
#define MUTE_OFF 0x39

#define BLOCK_INPUTS 0x22
#define RESET_PWR_UP_TIME 0x30
#define NO_BLOCK_INPUTS 0x33

// Double commmands

#define MAIN_LEVEL 0x03
#define READ_LEVEL 0x04
#define CENTER_LEVEL 0x05
#define SUB_LEVEL 0x06

// Requests

#define VERSION 0xF0
#define CURRENT_INPUT 0xF1
#define GET_INPUT_GAIN 0x2F
#define GET_TEMP 0x25
#define GET_PWR_UP_TIME 0x31
#define GET_STATUS 0x34

// Status

#define EXP_STX 0xAA
#define EXP_MODEL_STATUS 0x0A
#define EXP_MODEL_TEMP 0x0C

#define STATUS_STX 0x00
#define STATUS_MODEL 0x01
#define STATUS_LENGTH 0x02
#define STATUS_MAIN_LEVEL 0x03
#define STATUS_READ_LEVEL 0x04
#define STATUS_CENTER_LEVEL 0x05
#define STATUS_SUB_LEVEL 0x06
#define STATUS_CURRENT_INPUT 0x07
#define STATUS_UNKNOWN 0x08
#define STATUS_FX_INPUT_4 0x09
#define STATUS_FX_INPUT_5 0x0A
#define STATUS_FX_INPUT_2 0x0B
#define STATUS_FX_INPUT_AUX 0x0C
#define STATUS_FX_INPUT_1 0x0D
#define STATUS_FX_INPUT_3 0x0E
#define STATUS_SPDIF_STATUS 0x0F
#define STATUS_SIGNAL_STATUS 0x10
#define STATUS_VER_A 0x11
#define STATUS_VER_B 0x12
#define STATUS_VER_C 0x13
#define STATUS_STBY 0x14
#define STATUS_AUTO_STBY 0x15
#define STATUS_CHECKSUM 0x17

// MASK

// #define EFFECT_3D 0x00
// #define EFFECT_21 0x01
// #define EFFECT_41 0x02
// #define EFFECT_NO 0x03

// #define SPK_NONE 0x00
// #define SPK_ALL 0xFF
// #define SPK_FR 0x01
// #define SPK_FL 0x10
// #define SPK_RR 0x02
// #define SPK_RL 0x08
// #define SPK_CENTER 0x04
// #define SPK_SUB 0x20

class Z906 {
 public:
  Z906();

  void set_amplifier_uart(uart::UARTComponent *uart);

  int cmd(uint8_t);
  int cmd(uint8_t, uint8_t);
  int request(uint8_t);
  void log_status();
  bool is_powered_on();

  uint8_t main_sensor();
  uint8_t status[STATUS_TOTAL_LENGTH];
  int update();
  void Z906::set_state(uint8_t main_volume, uint8_t sub_volume, uint8_t rear_volume, uint8_t center_volume, uint8_t input, uint8_t* effects);
  int Z906::write_status();


 protected:
  uart::UARTComponent *amplifier_uart_;

  uint8_t LRC(uint8_t *, uint8_t);
};

}  // namespace logitech_z906
}  // namespace esphome