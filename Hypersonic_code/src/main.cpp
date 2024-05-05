#include <Arduino.h>
#include <lcdgfx.h>

#define GPD1 15
#define GPA0 4
#define GPA1 5
#define GPA2 6

#define SPI_MISO 13
#define SPI_MOSI 11
#define SPI_CLK 12

#define LCD_CS GPD1
#define LCD_BL GPA0
#define LCD_RES GPA1
#define LCD_DC GPA2

DisplayST7735_128x160x16_SPI disp(LCD_RES, 
  {-1, LCD_CS, LCD_DC, 0, SPI_CLK, SPI_MOSI});

void setup() {
  // put your setup code here, to run once:
  disp.begin();
  disp.getInterface().setRotation(1);
  disp.getInterface().setOffset(2,1);
  disp.clear();
  disp.setFixedFont(ssd1306xled_font6x8);
  disp.printFixed(2,2,"Hello World!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
