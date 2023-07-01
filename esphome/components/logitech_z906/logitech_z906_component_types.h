#pragma once

enum SwitchType { POWER, MUTE };

inline SwitchType resolve_switch_type(const std::string &type) {
  if (type == "power") {
    return POWER;
  } else if (type == "mute") {
    return MUTE;
  }
  return POWER;
}

enum SelectType { SOURCE, EFFECT };

inline SelectType resolve_select_type(const std::string &type) {
  if (type == "source") {
    return SOURCE;
  } else if (type == "effect") {
    return EFFECT;
  }
  return SOURCE;
}

enum NumberType { MASTER, BASS, CENTER, REAR };

inline NumberType resolve_number_type(const std::string &type) {
  if (type == "master_volume") {
    return MASTER;
  } else if (type == "bass_volume") {
    return BASS;
  } else if (type == "center_volume") {
    return CENTER;
  } else if (type == "rear_volume") {
    return REAR;
  }
  return MASTER;
}