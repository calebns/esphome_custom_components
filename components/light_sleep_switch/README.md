```yaml
# example configuration:

esp32:
  board: esp32dev
  framework:
    type: esp-idf
    version: recommended
    sdkconfig_options:
      CONFIG_FREERTOS_UNICORE: y
      CONFIG_FREERTOS_HZ: '1000'
      CONFIG_PM_USE_RTC_TIMER_REF: y
      CONFIG_XTAL_FREQ_40: y
      CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ: '80000000'
      CONFIG_PM_DFS_INIT_AUTO: n
      CONFIG_FREERTOS_USE_TICKLESS_IDLE: y
      CONFIG_PM_ENABLE: y
      CONFIG_PM_SLP_IRAM_OPT: y
      CONFIG_PM_RTOS_IDLE_OPT: y
      CONFIG_ESP_WIFI_SLP_IRAM_OPT: y
      CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP: y
      CONFIG_PM_PROFILING: y

switch:
  - platform: light_sleep_switch
    name: Light Sleep Switch
    id: light_sleep_sw
    internal: true

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  power_save_mode: high
  reboot_timeout: 10min
  on_connect:
    - switch.turn_on: light_sleep_sw
  on_disconnect:
    - switch.turn_off: light_sleep_sw
```