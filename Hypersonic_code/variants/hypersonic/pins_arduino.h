#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define USB_VID          0x303a
#define USB_PID          0x814D  // for user apps (https://github.com/espressif/usb-pids/pull/77)
#define USB_MANUFACTURER "Adam"
#define USB_PRODUCT      "Hypersonic"
#define USB_SERIAL       ""

// ADC enabled
static const uint8_t A0 = 4;
static const uint8_t A1 = 5;
static const uint8_t A2 = 6;
static const uint8_t A3 = 7;
static const uint8_t A4 = 8;
static const uint8_t A5 = 9;
static const uint8_t A6 = 10;
static const uint8_t A7 = 1;
static const uint8_t A8 = 2;

// No ADC
static const uint8_t D0 = 14;
static const uint8_t D1 = 15;
static const uint8_t D2 = 16;
static const uint8_t D3 = 17;
static const uint8_t D4 = 18;

// SPI
static const uint8_t MISO = 13;
static const uint8_t MOSI = 11;
static const uint8_t SCK  = 12;

static const uint8_t SS   = 10;

// I2C (Pull-ups on board)
static const uint8_t SDA  = 47;
static const uint8_t SCL  = 48;

// Codec pins
static const uint8_t I2S_DI   = 38;
static const uint8_t I2S_DO   = 39;
static const uint8_t I2S_WCLK = 40;
static const uint8_t I2S_BCLK = 41;
static const uint8_t I2S_MCLK = 42;
static const uint8_t CODEC_EN = 21;

// UART
static const uint8_t TX = 43;
static const uint8_t RX = 44;

#endif /* Pins_Arduino_h */
