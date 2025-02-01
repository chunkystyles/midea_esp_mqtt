#include "hvac.h"

dudanov::midea::ac::AirConditioner ac;
SoftwareSerial softwareSerial;
unsigned long hvac_update_timer;

void setup_hvac() {
  softwareSerial.begin(9600, SWSERIAL_8N1, 14, 12, false);
  ac.setStream(&softwareSerial);
  ac.addOnStateCallback(callback_function);
  ac.setBeeper(false);
  ac.setAutoconf(true);
  ac.setup();
  set_callback_function(set_state);
}

void send_capabilities() {
  dudanov::midea::ac::Capabilities capabilities = ac.getCapabilities();
  JsonDocument doc;
  // doc["activeClean"] = capabilities.activeClean();
  // doc["autoSetHumidity"] = capabilities.autoSetHumidity();
  // doc["breezeControl"] = capabilities.breezeControl();
  doc["buzzer"] = capabilities.buzzer();
  doc["decimals"] = capabilities.decimals();
  // doc["electricAuxHeating"] = capabilities.electricAuxHeating();
  doc["fanSpeedControl"] = capabilities.fanSpeedControl();
  // doc["indoorHumidity"] = capabilities.indoorHumidity();
  // doc["manualSetHumidity"] = capabilities.manualSetHumidity();
  // doc["maxTempAuto"] = capabilities.maxTempAuto();
  // doc["maxTempCool"] = capabilities.maxTempCool();
  // doc["maxTempHeat"] = capabilities.maxTempHeat();
  // doc["minTempAuto"] = capabilities.minTempAuto();
  // doc["minTempCool"] = capabilities.minTempCool();
  // doc["minTempHeat"] = capabilities.minTempHeat();
  // doc["nestCheck"] = capabilities.nestCheck();
  // doc["nestNeedChange"] = capabilities.nestNeedChange();
  // doc["oneKeyNoWindOnMe"] = capabilities.oneKeyNoWindOnMe();
  // doc["powerCal"] = capabilities.powerCal();
  // doc["powerCalSetting"] = capabilities.powerCalSetting();
  // doc["silkyCool"] = capabilities.silkyCool();
  // doc["smartEye"] = capabilities.smartEye();
  doc["supportAutoMode"] = capabilities.supportAutoMode();
  doc["supportBothSwing"] = capabilities.supportBothSwing();
  doc["supportCoolMode"] = capabilities.supportCoolMode();
  doc["supportDryMode"] = capabilities.supportDryMode();
  doc["supportEcoPreset"] = capabilities.supportEcoPreset();
  doc["supportFrostProtectionPreset"] = capabilities.supportFrostProtectionPreset();
  doc["supportHeatMode"] = capabilities.supportHeatMode();
  doc["supportHorizontalSwing"] = capabilities.supportHorizontalSwing();
  doc["supportLightControl"] = capabilities.supportLightControl();
  doc["supportTurboPreset"] = capabilities.supportTurboPreset();
  doc["supportHorizontalSwing"] = capabilities.supportHorizontalSwing();
  doc["supportVerticalSwing"] = capabilities.supportVerticalSwing();
  doc["unitChangeable"] = capabilities.unitChangeable();
  // doc["windOfMe"] = capabilities.windOfMe();
  // doc["windOnMe"] = capabilities.windOnMe();
  publish_capabilities_mqtt(doc);
}

void loop_hvac() {
  ac.loop();
  unsigned long now = millis();
  if (now - hvac_update_timer > MQTT_STATUS_UPDATE_TIME_MS)
  {
    hvac_update_timer = now;
    // publish_state_mqtt(convert_ac_to_json("periodic"));
    dudanov::midea::AutoconfStatus autoconfStatus = ac.getAutoconfStatus();
    if (autoconfStatus == dudanov::midea::AutoconfStatus::AUTOCONF_OK) {
      send_capabilities();
    } else if (autoconfStatus == dudanov::midea::AutoconfStatus::AUTOCONF_ERROR) {
      JsonDocument doc;
      doc["autoconf"] = "autoconf_error";
      publish_capabilities_mqtt(doc);
    } else if (autoconfStatus == dudanov::midea::AutoconfStatus::AUTOCONF_PROGRESS) {
      JsonDocument doc;
      doc["autoconf"] = "autoconf_progress";
      publish_capabilities_mqtt(doc);
    } else if (autoconfStatus == dudanov::midea::AutoconfStatus::AUTOCONF_DISABLED) {
      JsonDocument doc;
      doc["autoconf"] = "autoconf_disabled";
      publish_capabilities_mqtt(doc);
    }
  }
}

JsonDocument convert_ac_to_json(String updateType) {
  JsonDocument doc;
  doc["updateType"] = updateType;
  doc["targetTemp"] = ac.getTargetTemp();
  doc["indoorTemp"] = ac.getIndoorTemp();
  switch (ac.getMode()) {
    case dudanov::midea::ac::Mode::MODE_OFF:
      doc["mode"] = "off";
    case dudanov::midea::ac::Mode::MODE_COOL:
      doc["mode"] = "cool";
    case dudanov::midea::ac::Mode::MODE_HEAT:
      doc["mode"] = "heat";
    case dudanov::midea::ac::Mode::MODE_FAN_ONLY:
      doc["mode"] = "fan";
    case dudanov::midea::ac::Mode::MODE_DRY:
      doc["mode"] = "dry";
    case dudanov::midea::ac::Mode::MODE_AUTO:
      doc["mode"] = "auto";
    default:
      doc["mode"] = "NOT_SET";
  }
  switch (ac.getPreset()) {
    case dudanov::midea::ac::Preset::PRESET_SLEEP:
      doc["preset"] = "sleep";
    case dudanov::midea::ac::Preset::PRESET_TURBO:
      doc["preset"] = "turbo";
    case dudanov::midea::ac::Preset::PRESET_ECO:
      doc["preset"] = "eco";
    case dudanov::midea::ac::Preset::PRESET_FREEZE_PROTECTION:
      doc["preset"] = "freeze";
    case dudanov::midea::ac::Preset::PRESET_NONE:
      doc["preset"] = "none";
    default:
      doc["preset"] = "NOT_SET";
  }
  switch (ac.getSwingMode()) {
    case dudanov::midea::ac::SwingMode::SWING_BOTH:
      doc["swing"] = "both";
    case dudanov::midea::ac::SwingMode::SWING_VERTICAL:
      doc["swing"] = "vertical";
    case dudanov::midea::ac::SwingMode::SWING_HORIZONTAL:
      doc["swing"] = "horizontal";
    case dudanov::midea::ac::SwingMode::SWING_OFF:
      doc["swing"] = "off";
    default:
      doc["swing"] = "NOT_SET";
  }
  switch (ac.getFanMode()) {
    case dudanov::midea::ac::FanMode::FAN_SILENT:
      doc["fan"] = "silent";
    case dudanov::midea::ac::FanMode::FAN_LOW:
      doc["fan"] = "low";
    case dudanov::midea::ac::FanMode::FAN_MEDIUM:
      doc["fan"] = "medium";
    case dudanov::midea::ac::FanMode::FAN_HIGH:
      doc["fan"] = "high";
    case dudanov::midea::ac::FanMode::FAN_TURBO:
      doc["fan"] = "turbo";
    case dudanov::midea::ac::FanMode::FAN_AUTO:
      doc["fan"] = "auto";
    default:
      doc["fan"] = "NOT_SET";
  }
  dudanov::midea::AutoconfStatus autoconfStatus = ac.getAutoconfStatus();
  if (autoconfStatus == dudanov::midea::AutoconfStatus::AUTOCONF_OK) {
    doc["autoconf"] = "ok";
  } else if (autoconfStatus == dudanov::midea::AutoconfStatus::AUTOCONF_ERROR) {
    doc["autoconf"] = "error";
  } else if (autoconfStatus == dudanov::midea::AutoconfStatus::AUTOCONF_PROGRESS) {
    doc["autoconf"] = "progress";
  } else if (autoconfStatus == dudanov::midea::AutoconfStatus::AUTOCONF_DISABLED) {
    doc["autoconf"] = "disabled";
  } else {
    doc["autoconf"] = "NOT_SET";
  }
  return doc;
}

void callback_function() {
  publish_state_mqtt(convert_ac_to_json("stateChange"));
}

void set_state(JsonDocument doc) {
    dudanov::midea::ac::Control control;
    if (doc.containsKey("mode")) {
        String mode = doc["mode"] = doc["mode"].as<String>();
        if (mode.compareTo("off") == 0) {
            control.mode = dudanov::midea::ac::Mode::MODE_OFF;
        } else if (mode.compareTo("cool") == 0) {
            control.mode = dudanov::midea::ac::Mode::MODE_COOL;
        } else if (mode.compareTo("heat") == 0) {
            control.mode = dudanov::midea::ac::Mode::MODE_HEAT;
        } else if (mode.compareTo("fan") == 0) {
            control.mode = dudanov::midea::ac::Mode::MODE_FAN_ONLY;
        } else if (mode.compareTo("dry") == 0) {
            control.mode = dudanov::midea::ac::Mode::MODE_DRY;
        } else if (mode.compareTo("auto") == 0) {
            control.mode = dudanov::midea::ac::Mode::MODE_AUTO;
        }
    }
    if (doc.containsKey("targetTemp")) {
        String targetTemp = doc["targetTemp"].as<String>();
        control.targetTemp = strtof(targetTemp.c_str(), NULL);
    }
    if (doc.containsKey("fan")) {
        String fan = doc["fan"] = doc["fan"].as<String>();
        if (fan.compareTo("auto") == 0) {
            control.fanMode = dudanov::midea::ac::FanMode::FAN_AUTO;
        } else if (fan.compareTo("silent") == 0) {
            control.fanMode = dudanov::midea::ac::FanMode::FAN_SILENT;
        } else if (fan.compareTo("low") == 0) {
            control.fanMode = dudanov::midea::ac::FanMode::FAN_LOW;
        } else if (fan.compareTo("medium") == 0) {
            control.fanMode = dudanov::midea::ac::FanMode::FAN_MEDIUM;
        } else if (fan.compareTo("high") == 0) {
            control.fanMode = dudanov::midea::ac::FanMode::FAN_HIGH;
        } else if (fan.compareTo("turbo") == 0) {
            control.fanMode = dudanov::midea::ac::FanMode::FAN_TURBO;
        }
    }
    if (doc.containsKey("swing")) {
        String swing = doc["swing"] = doc["swing"].as<String>();
        if (swing.compareTo("off")) {
            control.swingMode = dudanov::midea::ac::SwingMode::SWING_OFF;
        } else if (swing.compareTo("both")) {
            control.swingMode = dudanov::midea::ac::SwingMode::SWING_BOTH;
        } else if (swing.compareTo("vertical")) {
            control.swingMode = dudanov::midea::ac::SwingMode::SWING_VERTICAL;
        } else if (swing.compareTo("horizontal")) {
            control.swingMode = dudanov::midea::ac::SwingMode::SWING_HORIZONTAL;
        }
    }
    if (doc.containsKey("preset")) {
        String preset = doc["preset"] = doc["preset"].as<String>();
        if (preset.compareTo("none")) {
            control.preset = dudanov::midea::ac::Preset::PRESET_NONE;
        } else if (preset.compareTo("sleep")) {
            control.preset = dudanov::midea::ac::Preset::PRESET_SLEEP;
        } else if (preset.compareTo("turbo")) {
            control.preset = dudanov::midea::ac::Preset::PRESET_TURBO;
        } else if (preset.compareTo("eco")) {
            control.preset = dudanov::midea::ac::Preset::PRESET_ECO;
        } else if (preset.compareTo("freeze")) {
            control.preset = dudanov::midea::ac::Preset::PRESET_FREEZE_PROTECTION;
        }
    }
    ac.control(control);
}