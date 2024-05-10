#include <Arduino.h>
#include <TFT_eSPI.h>
#include <encoder.h>
#include <tlv320aic3101.h>

#define LCD_BL A0

#define POT0 A6
#define POT1 A7
#define POT2 A8

#define ENC_A D4
#define ENC_B D3
#define ENC_SW D2


TFT_eSPI tft = TFT_eSPI();
int BL_BRIGHTNESS = 50; 

Encoder enc(ENC_A, ENC_B, ENC_SW);

tlv320aic3101 codec(CODEC_EN, &Wire);

TaskHandle_t dsp_handle;
TaskHandle_t inputs_handle;
TaskHandle_t screen_handle;

float pots[3];
constexpr float POT_SCALE = 1.0f/3380.0f;
float pots_alpha = 0.8;

void dsp_task(void * params) {

  // Codec init
  float t[512];

  Wire.setPins(SDA, SCL);

  float T = 0;

  i2s_pin_config_t i2s_pins = {
    .mck_io_num = I2S_MCLK,
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_WCLK,
    .data_out_num = I2S_DO,
    .data_in_num = I2S_DI,
  };

  codec.begin(&i2s_pins);
  delay(1000);

  while (1)
  {
    codec.read(t, 256);

    /*
    for(size_t i=0; i<256; i++){
      t[i] = 0.5f * sinf(T);

      T += (float) (2.0f*PI*440.0f/48000.0f);
      
    }
    */

    codec.write(t, 256);

  }
}

void inputs_task(void * params) {
  // Timer
  TickType_t lastWake = xTaskGetTickCount();
  const TickType_t period = pdMS_TO_TICKS(10);

  // Pot setup
  pinMode(POT0, INPUT);
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);

  while (1)
  {
    pots[0] = pots[0]*pots_alpha + (1-pots_alpha)*analogRead(POT0)*POT_SCALE;
    pots[1] = pots[1]*pots_alpha + (1-pots_alpha)*analogRead(POT1)*POT_SCALE;
    pots[2] = pots[2]*pots_alpha + (1-pots_alpha)*analogRead(POT2)*POT_SCALE;
    xTaskDelayUntil(&lastWake, period);
  }
}

void screen_task(void * params) {
  // timer
  TickType_t lastWake = xTaskGetTickCount();
  const TickType_t period = pdMS_TO_TICKS(33);

  // Display setup
  tft.init();
  pinMode(LCD_BL, OUTPUT);
  analogWrite(LCD_BL, BL_BRIGHTNESS);
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(1);
  tft.println("Hello World!");

  while (1)
  {
    tft.setCursor(0,0);
    tft.printf("POT0: %1.3f  \n", pots[0]);
    tft.printf("POT1: %1.3f  \n", pots[1]);
    tft.printf("POT2: %1.3f  \n", pots[2]);
    tft.printf("ENC: %d    \n", enc.get_count());
    tft.printf("ENC_SW: %d    \n", enc.get_button());
    xTaskDelayUntil(&lastWake, period);

  }
}

void setup() {
  // Start tasks  

  xTaskCreatePinnedToCore
    (inputs_task, "inputs", 5000, NULL, 4, &inputs_handle, 0);

  xTaskCreatePinnedToCore
    (screen_task, "screen", 5000, NULL, 2, &screen_handle, 0);

  xTaskCreatePinnedToCore(dsp_task, "DSP", 5000, NULL, 10, &dsp_handle, 1);

}

void loop() {
  // put your main code here, to run repeatedly:¨
  //tft.fillScreen(TFT_BLACK);
  
  //tft.printf("DAC status: %d \n", codec.get_DAC_status());
  delay(10000);
}
