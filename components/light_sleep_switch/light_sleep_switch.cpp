#include "esphome/core/log.h"
#include "light_sleep_switch.h"

#include <esp_pm.h>
#include <esp_sleep.h>
#include "driver/gpio.h"

namespace esphome {
namespace light_sleep_switch {

static const char *TAG = "light_sleep_switch.switch";

void LightSleepSwitch::setup() {
    esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "early", &light_sleep_lock);
    esp_pm_lock_acquire(light_sleep_lock);
    esp_pm_config_t pm_conf {
        .max_freq_mhz = cpu_max_freq,
        .min_freq_mhz = cpu_min_freq,
        .light_sleep_enable = true,
    };
    esp_pm_configure(&pm_conf);
}

void LightSleepSwitch::write_state(bool state) {
    if (state == true) {
	esp_pm_lock_release(light_sleep_lock);
    } else {
	esp_pm_lock_acquire(light_sleep_lock);
    }
}

void LightSleepSwitch::set_lss_cpu_min_freq_key(int freq) {
	cpu_min_freq = freq;
}

void LightSleepSwitch::set_lss_cpu_max_freq_key(int freq) {
	cpu_max_freq = freq;
}

void LightSleepSwitch::dump_config(){

}

} //namespace light_sleep_switch
} //namespace esphome
