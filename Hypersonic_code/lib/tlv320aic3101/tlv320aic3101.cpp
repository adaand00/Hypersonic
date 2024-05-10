#include "tlv320aic3101.h"

tlv320aic3101::tlv320aic3101
  (uint8_t reset_pin, TwoWire *i2c_bus)
  : reset_pin_(reset_pin), i2c_bus_(i2c_bus)
{
  active_page_ = -1;
}

tlv320aic3101::~tlv320aic3101(){
  i2s_driver_uninstall(i2s_num_);
}

void tlv320aic3101::begin(i2s_pin_config_t * i2s_pins){
  log_i("Codec init");

  i2c_bus_->begin();

  pinMode(reset_pin_, OUTPUT);
  digitalWrite(reset_pin_, LOW);
  delay(10);
  digitalWrite(reset_pin_, HIGH);
  delay(10);
  

  // Codec data-path, no dual rate, left DAC - left,
  write_register(7, 0, 0b00001000);

  // Audio serial data B, I2S mode, 16 bit data
  write_register(8, 0, 0b00100000);

  // CODEC filter, no high-pass, no dac effects, dac de-emph 
  write_register(12, 0, 0b00000101);

  // Left PGA, unmuted
  write_register(15, 0, 0b00000000);

  // LINE1L ctrl, fully differential, powered on, soft stepping
  write_register(19, 0, 0b10000100);

  // Unselected inputs to CM
  write_register(20, 0, 0b01111100);
  write_register(23, 0, 0b01111100);

  // DAC power, left channel power up, HPLCOM diff
  write_register(37, 0, 0b10000000);

  // HPR ctrl, short circuit protection enable
  write_register(38, 0, 0b00000110);

  // DAC switch, Left dac to line
  write_register(41, 0, 0b01000000);

  // Left DAC unmute, full volume
  write_register(43, 0, 0b00000000);

  // Left LO, unmute
  write_register(86, 0, 0b00001011);

  // clock, use CLKDIV
  write_register(101, 0, 0b00000001);


  i2s_config_t conf = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
    .sample_rate = 48000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
    .use_apll = true,
    .tx_desc_auto_clear = true,
    .mclk_multiple = I2S_MCLK_MULTIPLE_256,
  };

  i2s_driver_install(i2s_num_, &conf, 0, NULL);
  i2s_set_pin(i2s_num_, i2s_pins);

  log_i("i2s driver started");
  
}

void tlv320aic3101::write(float * buffer, size_t size){

  if(size > AIC3101_BUFF_SIZE){
    log_w("Too many samples, input %d, max %d", size, AIC3101_BUFF_SIZE);
    size = AIC3101_BUFF_SIZE;
  }

  size_t bytes_written;

  for (size_t i = 0; i < size; i++)
  {
    buff_[i] = (int16_t) (buffer[i] * INT16_MAX);
  }
  
  i2s_write(i2s_num_, buff_, size * sizeof(int16_t), &bytes_written, 1000);

  if(bytes_written != size*sizeof(int16_t)){
    log_w("I2S write mismatch");
  }
}

void tlv320aic3101::read(float * buffer, size_t size){

  if(size > AIC3101_BUFF_SIZE){
    log_w("Too many samples, input %d, max %d", size, AIC3101_BUFF_SIZE);
    size = AIC3101_BUFF_SIZE;
  }

  size_t bytes_read;

  i2s_read(i2s_num_, buff_, size * sizeof(int16_t), &bytes_read, 1000);

  if(bytes_read != size*sizeof(int16_t)){
    log_w("I2S read mismatch");
  }

  for (size_t i = 0; i < size; i++)
  {
    buffer[i] = ((float) buff_[i]) * (1.0/INT16_MAX);
  }
  
}

uint8_t tlv320aic3101::get_DAC_status(){

  return read_register(94, 0);

}


void tlv320aic3101::write_register(uint8_t reg, uint8_t page, uint8_t val){
  // Set correct page
  if(page != active_page_){
    i2c_bus_->beginTransmission(i2c_addr_);
    i2c_bus_->write(0);
    i2c_bus_->write(page);
    i2c_bus_->endTransmission();
    active_page_ = page;
  }

  // write requested register
  i2c_bus_->beginTransmission(i2c_addr_);
  i2c_bus_->write(reg);
  i2c_bus_->write(val);
  i2c_bus_->endTransmission();

}

uint8_t tlv320aic3101::read_register(uint8_t reg, uint8_t page){
  if (page != active_page_)
  {
    i2c_bus_->beginTransmission(i2c_addr_);
    i2c_bus_->write(0);
    i2c_bus_->write(page);
    i2c_bus_->endTransmission();
    active_page_ = page;
  }

  uint8_t result;
  
  i2c_bus_->beginTransmission(i2c_addr_);
  i2c_bus_->write(reg);
  i2c_bus_->endTransmission();

  i2c_bus_->requestFrom(i2c_addr_, 1);
  while (i2c_bus_->available())
  {
    result = i2c_bus_->read();
  }

  return result;
}

