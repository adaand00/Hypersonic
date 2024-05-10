#ifndef TLV320AIC3101_TLV320AIC3101_H_
#define TLV320AIC3101_TLV320AIC3101_H_

#include <Arduino.h>
#include <Wire.h>

#include <driver/i2s.h>

#define AIC3101_BUFF_SIZE 512


class tlv320aic3101
{
private:
  // reset pin
  uint8_t reset_pin_;

  // I2C config bus
  TwoWire * i2c_bus_;
  uint8_t i2c_addr_ = 0x18;

  // I2S communication
  i2s_port_t i2s_num_ = I2S_NUM_0;

  // Current active page (register 0)
  uint8_t active_page_;

  // temp buffer
  int16_t buff_[AIC3101_BUFF_SIZE];

  // I2C read/write configuration registers
  void write_register(uint8_t reg, uint8_t page, uint8_t val);
  uint8_t read_register(uint8_t reg, uint8_t page);

public:
  tlv320aic3101(uint8_t reset_pin, TwoWire *i2c_bus);
  ~tlv320aic3101();

  void begin(i2s_pin_config_t * i2s_p);

  void write(float * buffer, size_t size);

  void read(float * buffer, size_t size);

  void playback_stop();

  void playback_start();

  void set_sample_rate();

  void set_bit_depth();

  void set_input_gain(uint8_t gain);

  void set_output_volume(uint8_t vol);

  void bypass_enable();

  void bypass_disable();

  uint8_t get_DAC_status();

};



#endif /* TLV320AIC3101_TLV320AIC3101_H_ */
