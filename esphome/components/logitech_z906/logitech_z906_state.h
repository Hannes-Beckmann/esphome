#pragma once
namespace esphome {
namespace logitech_z906 {

enum Effect { EFFECT_3D, EFFECT_21, EFFECT_41, NONE };
enum Input { LINE, CHINCH, OPTICAL1, OPTICAL2, COAXIAL, AUX };

struct State {
  float master_volume;
  float bass_volume;
  float center_volume;
  float rear_volume;
  bool power;
  bool standby;
  bool mute;
  Effect effect[6];
  Input input;
};

// get String representation of effect
inline std::string effect_to_string(Effect effect) {
  switch (effect) {
    case NONE:
      return "None";
    case EFFECT_3D:
      return "3D";
    case EFFECT_21:
      return "2.1";
    case EFFECT_41:
      return "4.1";
  }
  return "None";
}

// get String representation of input
inline std::string input_to_string(Input input) {
  switch (input) {
    case LINE:
      return "Line";
    case CHINCH:
      return "Chinch";
    case OPTICAL1:
      return "Optical1";
    case OPTICAL2:
      return "Optical2";
    case COAXIAL:
      return "Coaxial";
    case AUX:
      return "Aux";
  }
  return "Line";
}

inline Effect string_to_effect(const std::string &input) {
  if (input == "3D") {
    return EFFECT_3D;
  } else if (input == "2.1") {
    return EFFECT_21;
  } else if (input == "4.1") {
    return EFFECT_41;
  } else if (input == "None") {
    return NONE;
  }
  return NONE;
}

inline Input string_to_input(const std::string &input) {
  if (input == "Line") {
    return LINE;
  } else if (input == "Chinch") {
    return CHINCH;
  } else if (input == "Optical1") {
    return OPTICAL1;
  } else if (input == "Optical2") {
    return OPTICAL2;
  } else if (input == "Coaxial") {
    return COAXIAL;
  } else if (input == "Aux") {
    return AUX;
  }
  return AUX;
}
}  // namespace logitech_z906
}  // namespace esphome