#include <Arduino.h>
#include <vector>
#include <memory>
#include <TFT_eSPI.h>
#include <encoder.h>
#include <tlv320aic3101.h>

#include "gain.h"
#include "Saturatio.h"
#include "meter.h"

#define LCD_BL A0

#define POT0 A6
#define POT1 A7
#define POT2 A8

#define ENC_A D4
#define ENC_B D3
#define ENC_SW D2

std::vector<GenericEffectBuffered *> effects;

TFT_eSPI tft = TFT_eSPI();
int BL_BRIGHTNESS = 100;
TFT_eSprite canvas = TFT_eSprite(&tft);
TFT_eSprite background = TFT_eSprite(&tft);


Encoder enc(ENC_A, ENC_B, ENC_SW);

tlv320aic3101 codec(CODEC_EN, &Wire);

TaskHandle_t dsp_handle;
TaskHandle_t inputs_handle;
TaskHandle_t screen_handle;

int curr_screen = 0;

float pots[3];
constexpr float POT_SCALE = 1.0f/3385.0f;
float pots_alpha = 0.8;

/**
 * @brief reads ADC, does DSP calculations and writes DAC.
*/
void dsp_task(void * params) {

  constexpr int chunksize = 128;

  // Codec init
  float * t1 = (float*) malloc(chunksize*sizeof(float));
  float * t2 = (float*) malloc(chunksize*sizeof(float));

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
    // Read into t1
    codec.read(t1, chunksize);

    //effects.at(1)->dsp_buffer(t1, t2, chunksize);

    for(int i=0; i<effects.size(); i++){
      // bypass 
      if(effects.at(i)->bypassed){
        continue;
      }
      
      // process effect
      effects.at(i)->dsp_buffer(t1, t2, chunksize);

      // swap pointers
      float * t = t1;
      t1 = t2;
      t2 = t; 
    }

    // Write output buffer
    codec.write(t1, chunksize);

  }
}


/**
 * @brief Reads potentiometer inputs and updates parameters
*/
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

    

    effects.at(0)->setParam(pots[0], 0);
    effects.at(2)->setParam(pots[1], 1);
    effects.at(2)->setParam(pots[2], 2);
    xTaskDelayUntil(&lastWake, period);
  }
}

/**
 * @brief Handles drawing to the display, as well as keeping track of encoder
*/
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

  canvas.setColorDepth(1);
  canvas.createSprite(160,100);

  background.setColorDepth(16);
  background.createSprite(160,100);
  background.fillRectHGradient(0,0,160,100,TFT_PINK, TFT_CYAN);

  while (1)
  {
    canvas.fillSprite(0);
    int c = enc.get_count();
    int es = effects.size();

    tft.setCursor(0,0);

    if (c < 0) {
      enc.set_count(-1);
      curr_screen = -1;
      
      canvas.setCursor(0,0);
      canvas.printf("POT0: %1.3f  \n", pots[0]);
      canvas.printf("POT1: %1.3f  \n", pots[1]);
      canvas.printf("POT2: %1.3f  \n", pots[2]);
      canvas.printf("ENC: %d    \n", enc.get_count());
      canvas.printf("ENC_SW: %d    \n", enc.get_button());
      
    } else {
      if(c >= es) {
        enc.set_count( es-1 );
        c = es-1;
      }

      curr_screen = c;
      
      tft.print(effects.at(curr_screen)->name_.c_str());
      tft.print("        ");

      effects.at(curr_screen)->draw_screen(&canvas);

    }

    canvas.pushSprite(0,28);

    //tft.fillRect(0,28,160,100, TFT_BLACK);
    //tft.pushMaskedImage(0,28,160,100, 
    //  (uint16_t *) background.frameBuffer(0), (uint8_t *) canvas.frameBuffer(0));

    xTaskDelayUntil(&lastWake, period);

  }
}

void setup() {
  // Initialize effects
  effects.push_back(new GainEffect());
  effects.push_back(new Meter());
  effects.push_back(new Saturation());


  // Start tasks  
  xTaskCreatePinnedToCore
    (inputs_task, "inputs", 5000, NULL, 4, &inputs_handle, 0);

  xTaskCreatePinnedToCore
    (screen_task, "screen", 5000, NULL, 2, &screen_handle, 0);

  xTaskCreatePinnedToCore(dsp_task, "DSP", 10000, NULL, 10, &dsp_handle, 1);

}

void loop() {
  delay(10000);
}
