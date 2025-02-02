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
