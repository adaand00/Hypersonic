#ifndef EFFECTS_UTILS_GENERIC_EFFECT_BUFFERED_H_
#define EFFECTS_UTILS_GENERIC_EFFECT_BUFFERED_H_

#include "parameter.h"
#include <tft_eSPI.h>

constexpr int NUM_PARAMS = 3;

class GenericEffectBuffered
{
protected:
    Parameter params_[NUM_PARAMS];
    
public:
    std::string name_ = "unnamed";
    bool bypassed=false;

    GenericEffectBuffered(){};

    ~GenericEffectBuffered(){};

    virtual void dsp_buffer(float * in, float * out, int num_samples) = 0;

    virtual void draw_screen(TFT_eSprite * display) = 0;

    void setParam(float value, uint8_t param_num){
        if(param_num > NUM_PARAMS) {
            return;
        }

        params_[param_num].set(value);
    };

    inline float getParam(uint8_t param_num){
        if(param_num > NUM_PARAMS) {
            return NAN;
        }

        return params_[param_num].get();
    }
};


#endif /* EFFECTS_UTILS_GENERIC_EFFECT_BUFFERED_H_ */
