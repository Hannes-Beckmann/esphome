enum SwitchType { POWER, MUTE };

SwitchType resolve_switch_type(const std::string &type) {
  if (type == "power") {
    return POWER;
  } else if (type == "mute") {
    return MUTE;
  }
  return POWER;
}

enum SelectType { SOURCE, EFFECT };

SelectType resolve_select_type(const std::string &type) {
  if (type == "source") {
    return SOURCE;
  } else if (type == "effect") {
    return EFFECT;
  }
  return SOURCE;
}

enum NumberType { MASTER, BASS, CENTER, REAR };

NumberType resolve_number_type(const std::string &type) {
  if (type == "master") {
    return MASTER;
  } else if (type == "bass") {
    return BASS;
  } else if (type == "center") {
    return CENTER;
  } else if (type == "rear") {
    return REAR;
  }
  return MASTER;
}