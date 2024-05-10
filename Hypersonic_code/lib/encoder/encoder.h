#ifndef PERIPHERALS_ENCODER_H_
#define PERIPHERALS_ENCODER_H_

#include "Arduino.h"
#include "driver/pcnt.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "limits.h"

class Encoder {
private:

  pcnt_unit_t unit_;
  uint8_t pin_SW_;

public:

  Encoder(uint8_t pin_A, uint8_t pin_B, uint8_t pin_SW,
    pcnt_unit_t unit = PCNT_UNIT_0)
  {
    pin_SW_ = pin_SW;
    pinMode(pin_SW, INPUT_PULLUP);

    unit_ = unit;

    const char * TAG = "Encoder::Encoder()";

    ESP_LOGI(TAG, "install pcnt unit");

    pcnt_config_t dev_conf = {
      .pulse_gpio_num = pin_A,
      .ctrl_gpio_num = pin_B,
      .lctrl_mode = PCNT_MODE_REVERSE,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = PCNT_COUNT_DEC,
      .neg_mode = PCNT_COUNT_INC,
      .counter_h_lim = INT16_MAX,
      .counter_l_lim = INT16_MIN,
      .unit = unit_,
      .channel = PCNT_CHANNEL_0,
    };

    ESP_ERROR_CHECK(pcnt_unit_config(&dev_conf));

    // Configure channel 1
    dev_conf.pulse_gpio_num = pin_B;
    dev_conf.ctrl_gpio_num = pin_A;
    dev_conf.channel = PCNT_CHANNEL_1;
    dev_conf.pos_mode = PCNT_COUNT_INC;
    dev_conf.neg_mode = PCNT_COUNT_DEC;
    ESP_ERROR_CHECK(pcnt_unit_config(&dev_conf));
    
    pcnt_counter_pause(unit_);

    pcnt_set_filter_value(unit_, 80);

    pcnt_counter_clear(unit_);

    pcnt_counter_resume(unit_);
  };

  int get_count(){
    int16_t val = 0;
    pcnt_get_counter_value(unit_, &val);

    return (val+1)>>2;
  };

  bool get_button(){
    return !digitalRead(pin_SW_);
  }
};

#endif /* PERIPHERALS_ENCODER_H_ */
