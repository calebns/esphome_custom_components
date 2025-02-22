A component to support entering and exiting Light Sleep on ESP32. With example config applied to Sonoff MINIR4 the power consumption dropped from 0.7W to 0.09W in idle while still maintaining stable WiFi connection (values measured at wall outlet at 230V AC, should be about 0.06W at 110V AC).

WARNING: Due to different partition schema between Arduino and IDF SDKs it is suggested to program the ESP32 chip using UART for the first time. Otherwise the old partition with Arduino firmware might be started after power cycle.

```yaml
# example configuration:

external_components:
  - source: github://calebns/esphome_custom_components

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

If YAML configuration does not provide any periodic way to wake up the ESP32, the light sleep component refuses to enter LightSleep mode. To workaround this issue the dummy switch can be added which periodically toggles. Code below will guarantee the sustem wakes up at least every 200ms. 

The code below is required for Sonoff MINIR4 if GPIO wakeup is not provided. Espressif SDK can wake up only on GPIO_RISING_EDGE or GPIO_FALLING_EDGE, not GPIO_ANY_EDGE. This makes it difficult to be used with a bistable wall switch. Momentary switch could resolve the problem, but I've decided to integrate following workaround instead.

```yaml
switch:
  - platform: template
    name: "Dummy Switch"
    id: dummyswitch
    lambda: |-
        return false;
    turn_on_action:
      - lambda: return;
    turn_off_action:
      - lambda: return;
    internal: true

interval:
  - interval: 200ms
    then:
      - switch.toggle: dummyswitch
```

If GPIO switch is used then additional steps must be taken. When ESP32 enters light sleep it disables power to GPIO peripheral. To retain GPIO state the pin must be put into hold mode, which forces the GPIO to be always in the current state. When GPIO value needs to be changed, the hold mode must be disabled and reenabled to latch the new value. Example code:

```
wifi:
  on_connect:
    - lambda: |-
        gpio_hold_en(GPIO_NUM_14);
    - switch.turn_on: light_sleep_sw

switch:
- platform: gpio
    name: "1"
    pin: GPIO14
    id: relay_1
    on_turn_on:
    - lambda: "gpio_hold_dis(GPIO_NUM_14); gpio_hold_en(GPIO_NUM_14);"
    on_turn_off:
    - lambda: "gpio_hold_dis(GPIO_NUM_14); gpio_hold_en(GPIO_NUM_14);"
```
