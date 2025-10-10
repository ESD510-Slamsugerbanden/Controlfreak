#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h" // For direct GPIO register access
#include <Arduino.h>


class motor{
    int pin_CCW;
    int pin_CW;
    int pin_PWM;
    int ch_PWM;
    public:
    /// @brief Sets up the pin and pwm configuration for a given motor
    /// @param pin_CW pin used for clockwise
    /// @param pin_CCW pin used for counter clockwise
    /// @param pin_PWM pin used for enable
    /// @param ch_PWM pwm channel used for counter clockwise
    void begin(int pin_CW, int pin_CCW, int pin_PWM, int ch_PWM);

    /// @brief Sets the speed for the given motor
    /// @param speed 
    void set_speed(int speed);
};
