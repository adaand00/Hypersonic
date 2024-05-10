#ifndef ST7735_SETUP_H_
#define ST7735_SETUP_H_


#define ST7735_DRIVER


#define TFT_WIDTH  128
#define TFT_HEIGHT 160

#define ST7735_GREENTAB2

#define USE_HSPI_PORT

#define TFT_RGB_ORDER TFT_RGB  // Colour order Blue-Green-Red

#define TFT_MOSI MOSI
#define TFT_SCLK SCK
#define TFT_MISO MISO

#define TFT_CS D1
#define TFT_DC A2
#define TFT_RST A1

//#define TFT_BL A0
//#define TFT_BACKLIGHT_ON HIGH

#define LOAD_GLCD
#define LOAD_FONT2

#define SPI_FREQUENCY 27000000

#endif /* ST7735_SETUP_H_ */
