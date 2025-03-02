#pragma once

#include <esp_pm.h>
#include <esp_sleep.h>

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace light_sleep_switch {

class LightSleepSwitch : public switch_::Switch, public Component {
 public:
  void setup() override;
  void write_state(bool state) override;
  void dump_config() override;
 private:
  esp_pm_lock_handle_t light_sleep_lock;
};

} //namespace light_sleep_switch
} //namespace esphome
