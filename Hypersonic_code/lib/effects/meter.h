#ifndef EFFECTS_METER_H_
#define EFFECTS_METER_H_

#include "utils/generic_effect_sampled.h"

#include "esp_dsp.h"

class Meter: public GenericEffectSampled {
private:
    float level = 0;
    bool clip = false;

    // circular coordinates center
    int cx = 80;
    int cy = 240;

    // Draw a line from the circular coordinates center, 
    // between r1 and r2 along angle
    void drawRadiusLine(TFT_eSprite * disp, int angle, int r1, int r2){

        float sin_ang = sinf(-angle*M_PI/180.0); 
        float cos_ang = cosf(-angle*M_PI/180.0); 

        disp->drawLine(
            cx+ r1*sin_ang, cy+ r1*cos_ang,
            cx+ r2*sin_ang, cy+ r2*cos_ang,
            1);
    }

public:

    Meter() {

        name_.assign("Meter");
    };

    float dsp(float in){

        float s = abs(in);

        if(s > level){
            level += (s-level)*0.002;
        }else{
            level -= level*0.00008;
        }

        if(s>1){
            clip = true;
        }else if(level<0.7){
            clip = false;
        }

        return in;
    }

    void draw_screen(TFT_eSprite * display){
        
        // get screen data
        int w = display->width();
        int h = display->height(); 

        // Draw decorations
        display->drawArc(cx, cy, cy-10, cy-11, 165,195, 1, 0, false);
        display->drawArc(cx, cy, cy-70, cy-71, 165,195, 1, 0, false);

        drawRadiusLine(display, 165, cy-11, cy-70);
        drawRadiusLine(display, 195, cy-11, cy-70);
        
        // Draw needle
        float dbLevel = 20*log10(level);
        drawRadiusLine(display, 195+dbLevel, cy-20, 0);

        if(clip){
            display->drawString("clip", 120, 0);
        }

    };

};

#endif