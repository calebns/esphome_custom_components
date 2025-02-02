#include "esphome/core/log.h"
#include "light_sleep_switch.h"

#include <esp_pm.h>
#include <esp_sleep.h>
#include "driver/gpio.h"

namespace esphome {
namespace light_sleep_switch {

static const char *TAG = "light_sleep_switch.switch";

void LightSleepSwitch::setup() {

}

void LightSleepSwitch::write_state(bool state) {
    if (state == true) {
        esp_pm_config_t pm_conf {
            .max_freq_mhz = 80,
            .min_freq_mhz = 40,
            .light_sleep_enable = true,
          };
          esp_pm_configure(&pm_conf);
    } else {
        esp_pm_config_t pm_conf {
            .max_freq_mhz = 240,
            .min_freq_mhz = 80,
            .light_sleep_enable = false,
          };
          esp_pm_configure(&pm_conf);
    }
}

void LightSleepSwitch::dump_config(){

}

} //namespace light_sleep_switch
} //namespace esphome