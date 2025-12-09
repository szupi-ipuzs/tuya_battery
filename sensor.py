from esphome import pins
import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.components.adc import validate_adc_pin
from esphome.const import (
      CONF_ID,
      CONF_BATTERY_LEVEL,
      CONF_BATTERY_VOLTAGE,
      UNIT_PERCENT,
      UNIT_VOLT,
      DEVICE_CLASS_BATTERY,
      DEVICE_CLASS_VOLTAGE,
      ENTITY_CATEGORY_DIAGNOSTIC,
      STATE_CLASS_MEASUREMENT,
)

CONF_ADC_PIN = "adc_pin"
CONF_SWITCH_PIN = "switch_pin"
CONF_POLL_INTERVAL = "poll_interval"
CONF_STABILIZE_TIME = "stabilize_time"
CONF_MEASURE_TIME = "measure_time"
CONF_VREF = "vref"
CONF_VDIVIDER = "vdivider"
CONF_MAX_BATTERY_VOLTAGE = "max_battery_voltage"
CONF_MIN_BATTERY_VOLTAGE = "min_battery_voltage"
CONF_ADC_BITS = "adc_bits"

DEPENDENCIES = ["adc"]

tuya_battery_ns = cg.esphome_ns.namespace("tuya_battery")
TuyaBatteryComponent = tuya_battery_ns.class_("TuyaBatteryComponent", cg.Component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(TuyaBatteryComponent),
            cv.Required(CONF_ADC_PIN): validate_adc_pin,
            cv.Required(CONF_SWITCH_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_POLL_INTERVAL): cv.positive_time_period_milliseconds,
            cv.Required(CONF_STABILIZE_TIME): cv.positive_time_period_milliseconds,
            cv.Required(CONF_MEASURE_TIME): cv.positive_time_period_milliseconds,
            cv.Required(CONF_VREF): cv.positive_float,
            cv.Required(CONF_VDIVIDER): cv.positive_float,
            cv.Required(CONF_MAX_BATTERY_VOLTAGE): cv.positive_float,
            cv.Required(CONF_MIN_BATTERY_VOLTAGE): cv.positive_float,
            cv.Required(CONF_ADC_BITS): cv.positive_int,
            cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    adc_pin = await config[CONF_ADC_PIN]
    switch_pin = await cg.gpio_pin_expression(config[CONF_SWITCH_PIN])
    var = cg.new_Pvariable(config[CONF_ID],
                           config[CONF_POLL_INTERVAL],
                           config[CONF_STABILIZE_TIME],
                           config[CONF_MEASURE_TIME],
                           adc_pin, switch_pin)
    await cg.register_component(var, config)

    if vref := config.get(CONF_VREF):
        cg.add(var.set_vref(vref))
    if vdivider := config.get(CONF_VDIVIDER):
        cg.add(var.set_vdivider(vdivider))
    if max_batt := config.get(CONF_MAX_BATTERY_VOLTAGE):
        cg.add(var.set_max_battery_voltage(max_batt))
    if min_batt := config.get(CONF_MIN_BATTERY_VOLTAGE):
        cg.add(var.set_min_battery_voltage(min_batt))
    if adc_bits := config.get(CONF_ADC_BITS):
        cg.add(var.set_adc_bits(adc_bits))

    if battery_level_config := config.get(CONF_BATTERY_LEVEL):
        sens = await sensor.new_sensor(battery_level_config)
        cg.add(var.set_battery_level(sens))
    if battery_voltage_config := config.get(CONF_BATTERY_VOLTAGE):
        sens = await sensor.new_sensor(battery_voltage_config)
        cg.add(var.set_battery_voltage(sens))