# esphome custom components

This repository contains a collection of my custom components
for [ESPHome](https://esphome.io/).

## 1. Usage

Use latest [ESPHome](https://esphome.io/) (at least v1.18.0)
with external components and add this to your `.yaml` definition:

```yaml
external_components:
  - source: github://calebns/esphome_custom_components
```

## 2. Components

### 2.1. `light_sleep_switch`

A component to support entering and exiting Light Sleep on ESP32.
With example config applied to Sonoff MINIR4 the power consumption dropped from 0.6W to 0.08W in idle while still maintaining stable WiFi connection.

### 2.2 `bl0939_lss`

The fork of ESPHome version of BL0939 driver but suitable to be used with Light Sleep (BL0939 Light Sleep Safe).
This power sensor can be found in Sonoff Dual R3 rev2.

### 2.3 `bl0942_lss`

The fork of ESPHome version of BL0942 driver but suitable to be used with Light Sleep (BL0942Light Sleep Safe).
This power sensor can be found in Shelly PM Mini gen3.
