#include "tuya_battery.h"

namespace esphome::tuya_battery
{

static const char *const TAG = "tuya_batt";

void TuyaBatteryComponent::setup()
{
#ifndef USE_ADC_SENSOR_VCC
  this->adc_pin_->setup();
#endif  // !USE_ADC_SENSOR_VCC
  this->switch_pin_->digital_write(false);

   // force initial read
   this->next_state_change_ts_ = millis();
   this->state_ = State::IDLE;
}

void TuyaBatteryComponent::dump_config()
{
   ESP_LOGCONFIG(TAG, "BK72XX Battery:");
#ifdef USE_ADC_SENSOR_VCC
  ESP_LOGCONFIG(TAG, "  ADC Pin: VCC");
#else   // USE_ADC_SENSOR_VCC
  LOG_PIN("  ADC Pin: ", this->adc_pin_);
#endif  // USE_ADC_SENSOR_VCC

   LOG_PIN("  Switch Pin: ", this->switch_pin_);
   ESP_LOGCONFIG(TAG, "  VRef: %.2f mV", this->vref_);
   ESP_LOGCONFIG(TAG, "  VDivider: %.2f", this->vdivider_);
   ESP_LOGCONFIG(TAG, "  Max Battery Voltage: %.2f mV", this->maxbatt_);
   ESP_LOGCONFIG(TAG, "  Min Battery Voltage: %.2f mV", this->minbatt_);
   ESP_LOGCONFIG(TAG, "  ADC Bits: %u", this->adc_bits_);
   ESP_LOGCONFIG(TAG, "  Polling Interval: %u seconds", this->pollTimeSeconds_);
   ESP_LOGCONFIG(TAG, "  Stabilizing Time: %u ms", this->stabilizeTimeMs_);
   ESP_LOGCONFIG(TAG, "  Measurement Time: %u ms", this->measureTimeMs_);
}

void TuyaBatteryComponent::loop()
{
  uint32_t now = millis();

  if (state_ == State::IDLE)
  {
    if (now >= this->next_state_change_ts_)
    {
      state_ = State::STABILIZING;
      this->next_state_change_ts_ = millis() + this->stabilizeTimeMs_;
      enable_measurement_();
    }
  }
  else if (state_ == State::STABILIZING)
  {
    if (now >= this->next_state_change_ts_)
    {
      state_ = State::MEASURING;
      this->next_state_change_ts_ = now + this->measureTimeMs_;
    }
  }
  else if (state_ == State::MEASURING)
  {
    add_sample_();
    if (now >= this->next_state_change_ts_)
    {
      state_ = State::IDLE;
      this->next_state_change_ts_ = now + this->pollTimeSeconds_ * 1000u;
      finish_measurement_();
    }
  }
}

void TuyaBatteryComponent::enable_measurement_()
{
  if (vdivider_ > 1.0f)
  {
    this->switch_pin_->digital_write(true);
  }
}

void TuyaBatteryComponent::finish_measurement_()
{
  assert(!this->voltage_samples_.empty());
  float average_voltage = 0.0f;
  for (const float &sample : this->voltage_samples_)
  {
    average_voltage += sample;
  }
  average_voltage /= static_cast<float>(this->voltage_samples_.size());
  this->voltage_samples_.clear();

  float level = (average_voltage - this->minbatt_) / (this->maxbatt_ - this->minbatt_);
  level = std::clamp(level, 0.0f, 1.0f) * 100.0f;
  if (vdivider_ > 1.0f)
  {
    this->switch_pin_->digital_write(false);
  }
  this->voltage_sensor_->publish_state(average_voltage);
  this->level_sensor_->publish_state(level);
}

void TuyaBatteryComponent::add_sample_()
{
  if (this->voltage_samples_.size() >= MAX_SAMPLES_)
  {
    return;
  }
  uint32_t raw = analogRead(this->adc_pin_->get_pin());
  float voltage = (static_cast<float>(raw) / static_cast<float>((1 << this->adc_bits_) - 1)) * this->vref_ * this->vdivider_;
  this->voltage_samples_.push_back(voltage);
}

}

