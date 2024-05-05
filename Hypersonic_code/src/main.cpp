#include <Arduino.h>
#include <lcdgfx.h>

#define LCD_CS D1
#define LCD_BL A0
#define LCD_RES A1
#define LCD_DC A2

DisplayST7735_128x160x16_SPI disp(LCD_RES, 
  {-1, LCD_CS, LCD_DC, 0, SCK, MOSI});

void setup() {

  // Display setup
  pinMode(LCD_BL, OUTPUT);
  analogWrite(LCD_BL, 100);

  disp.begin();
  disp.getInterface().setRotation(1);
  disp.getInterface().setOffset(2,1);
  disp.clear();
  disp.setFixedFont(ssd1306xled_font5x7);

  disp.printFixed(2,2,"Hello World!");

  // Pot setup


}

void loop() {
  // put your main code here, to run repeatedly:

}
