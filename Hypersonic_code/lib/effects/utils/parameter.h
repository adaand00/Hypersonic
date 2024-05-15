#ifndef PARAMETER_H_
#define PARAMETER_H_

#include "Arduino.h"

typedef enum {
  PARAM_LINEAR,
  PARAM_LOG,
  PARAM_EXP,
} param_mode_t;

class Parameter
{
private:
  float value_;
  float lower_;
  float upper_;
  param_mode_t mode_;

public:
  std::string name;

  Parameter(){
    lower_ = 0;
    upper_ = 1;
    mode_ = PARAM_LINEAR;
    name.assign("Unknown");
  };

  ~Parameter(){};
  
  /**
   * 
   * @brief Configure this parameter. Sets the limits that the min/max 
   * potentiomer positions map to, as well as the mapping.
   * 
   * @param[in] lower lower limit for parameter.
   * 
   * @param[in] upper upper limit for parameter.
   * 
   * @param[in] init initial value for parameter
   * 
   * @param[in] mode mapping mode of parameter. One of:
   *    - `PARAM_LINEAR`: Simple linear mapping
   *    - `PARAM_EXP`: Exponential mapping, useful for dB type inputs
   *    - `PARAM_LOG`: Logarithmic mapping.
   * 
   * @param[in] name Name of parameter
   * 
   * @note If `PARAM_EXP` mode is used, `min` and `max` need to be strictly
   * larger than 0. 
  */
  void config
    (float lower, float upper, float init, param_mode_t mode, std::string name){
    
    if(mode==PARAM_EXP){
      // PARAM_EXP mode, limits much be strictly larger than 0. 
      if(!(upper>0.0f)){
        log_e("Invalid parameter limits for %s, upper= %1.2f", name, upper);
        upper = 0.001;
      }
      
      if(!(lower>0.0f)){
        log_e("Invalid parameter limits for %s, lower= %1.2f", name, lower);
        lower = 0.001;
      }
    }    

    lower_ = lower;
    upper_ = upper;
    mode_ = mode;
    this->name.assign(name); 

    value_ = init;

  };

  /**
   * @brief Set parameter value from measured potentiometer. 
   * 
   * @param[in] pot_value Measured potentiometer value, should be in range 
   * 0 -> 1
  */
  void set(float val){

    if(val < 0.0f || val > 1.0f){
      log_e("Invalid potentiometer input: %d", val);
      return;
    }

    switch (mode_)
    {
    case PARAM_LINEAR:
      value_ = val*(upper_-lower_) + lower_;
      break;
    case PARAM_EXP:
      value_ = pow10f(val*(log10f(upper_)-log10f(lower_))+log10f(lower_));
      break;
    case PARAM_LOG:
      value_ = log10f(val*(pow10f(upper_)-pow10f(lower_))+pow10f(lower_));
      break;
    default:
    break;
    }
  };
  
  /**
   * @brief Get parameter value.
   * 
   * @return value, in 
  */
  float get(){
    return value_;
  };
  
};

#endif