# tuya_battery
An esphome external component that adds support for switch-based battery on some tuya devices.

# How do I know I need it?
Check the internal tuya config (see [Tuya Pinout Config](https://docs.libretiny.eu/docs/resources/tuya-pin-config/)) of your device, its "user_param_key" will contain the following keys:

* **samp_pin** - this is the pin number of the ADC pin that should be read to read the voltage. On bk72xx this is usually 23 (which corresponds to P23)
* **samp_sw_pin** - this is the pin number that should be pulled (high or low) to enable the divider and voltage reading. This can be any gpio pin.
* **samp_sw_lv** - how should the `samp_sw_pin` be pulled: high (1) or low (0)
* **max_V** - maximum battery voltage allowed by the device (in mV)
* **min_V** - minimum battery voltage allowed by the device (in mV)


# What I need to know
Apart from the pinout above you need to know the following:
* **vref** vref of the ADC pin (default 2400 and static on BK7231)
* **vdivider** voltage divider, may vary per device, needs to be found empirically (start with 2.29)
* **adc_bits** number of bits available in ADC (usually 12)


# Example config

``` yaml
sensor:
  - platform: tuya_battery
    adc_pin: P23
    switch_pin: P14
    vref: 2400
    vdivider: 2.29
    min_battery_voltage: 2.0
    max_battery_voltage: 3.0
    adc_bits: 12
    poll_interval: 30s
    stabilize_time: 500ms
    measure_time: 100ms
    battery_level:
      name: "Battery Level"
      unit_of_measurement: "%"
      accuracy_decimals: 0
      device_class: "battery"
      state_class: "measurement"
      entity_category: "diagnostic"
    battery_voltage:
      name: "Battery Voltage"
      unit_of_measurement: "V"
      accuracy_decimals: 2
      device_class: "battery"
      state_class: "measurement"
      entity_category: "diagnostic"

```
