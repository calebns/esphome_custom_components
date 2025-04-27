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
  void set_lss_cpu_min_freq_key(int freq);
  void set_lss_cpu_max_freq_key(int freq);
 protected:
  int cpu_min_freq;
  int cpu_max_freq;
  esp_pm_lock_handle_t light_sleep_lock;
};

} //namespace light_sleep_switch
} //namespace esphome
