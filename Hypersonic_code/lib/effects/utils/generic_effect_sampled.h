#ifndef GENERIC_EFFECT_SAMPLED_H_
#define GENERIC_EFFECT_SAMPLED_H_

#include "generic_effect_buffered.h"

class GenericEffectSampled: public GenericEffectBuffered 
{
public:

    void dsp_buffer(float * in, float * out, int num_samples) override {
        for (int i = 0; i < num_samples; i++)
        {
            out[i] = dsp(in[i]);
        }
    };

    virtual float dsp(float in) = 0;

};

#endif