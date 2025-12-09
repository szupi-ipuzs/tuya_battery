#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/gpio.h"

#include <vector>

namespace esphome::tuya_battery
{

class TuyaBatteryComponent : public Component{
  SUB_SENSOR(voltage)
  SUB_SENSOR(level)

public:

  TuyaBatteryComponent(const uint32_t pollTimeSeconds,
                 const uint32_t stabilizeTimeMs,
                 const uint32_t measureTimeMs,
                 InternalGPIOPin *adc_pin, InternalGPIOPin *switch_pin):
  pollTimeSeconds_(pollTimeSeconds),
  stabilizeTimeMs_(stabilizeTimeMs),
  measureTimeMs_(measureTimeMs),
  adc_pin_(adc_pin),
  switch_pin_(switch_pin)
  {}

  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_vref(float vref){vref_ = vref;}
  void set_vdivider(float vdivider){vdivider_ = vdivider;}
  void set_maxbatt(float maxbatt){maxbatt_ = maxbatt;}
  void set_minbatt(float minbatt){minbatt_ = minbatt;}
  void set_adc_bits(uint8_t adc_bits){adc_bits_ = adc_bits;}

protected:

  enum class State
  {
    IDLE,
    STABILIZING,
    MEASURING,
  };

  void enable_measurement_();
  void finish_measurement_();
  void add_sample_();

  static const std::size_t MAX_SAMPLES_ = 10u;

  const uint32_t pollTimeSeconds_;
  const uint32_t stabilizeTimeMs_;
  const uint32_t measureTimeMs_;

  InternalGPIOPin *adc_pin_{nullptr};
  InternalGPIOPin *switch_pin_{nullptr};
  float vref_{2400.0f};
  float vdivider_{2.29f};
  float maxbatt_{3000.0f};
  float minbatt_{2000.0f};
  uint8_t adc_bits_{10u};

  State state_{State::IDLE};
  uint32_t next_state_change_ts_{0u};

  std::vector<float> voltage_samples_;
};

}

