#ifndef EFFECTS_GAIN_H_
#define EFFECTS_GAIN_H_

#include "utils/generic_effect_buffered.h"

#include "esp_dsp.h"

class GainEffect: public GenericEffectBuffered {
public:

    GainEffect() {
        params_[0].config(0.01f, 10.0f, 1.0f, PARAM_EXP, "Gain");

        name_.assign("Gain");
    };

    void dsp_buffer(float * in, float * out, int num_samples) override {

        float C = getParam(0);

        dsps_mulc_f32_ae32(in, out, num_samples, C, 1, 1);

    };

    void draw_screen(TFT_eSprite * display){
        float g = 20*log10f(getParam(0)); 

        display->drawFastHLine(10, 30, g+40, 1);
    };

};


#endif /* EFFECTS_GAIN_H_ */
