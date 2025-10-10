#include <Motor_driver.h>

void motor::begin(int pin_CW, int pin_CCW, int pin_PWM, int ch_PWM){
    this->pin_CW = pin_CW;
    this->pin_CCW = pin_CCW;
    this->pin_PWM = pin_PWM;
    this->ch_PWM = ch_PWM;
    
    pinMode(pin_CW, OUTPUT);
    pinMode(pin_CCW, OUTPUT);
    pinMode(pin_PWM, OUTPUT);

    ledcSetup(ch_PWM, 20000, 8);


    ledcAttachPin(pin_PWM, ch_PWM);

    //ledcChangeFrequency(CH_CW, 20000, 8);
    //ledcChangeFrequency(CH_CCW, 20000, 8);
    ledcWrite(pin_PWM, 0);

    digitalWrite(pin_CW, LOW);
    digitalWrite(pin_CCW, LOW);
}


void motor::set_speed(int speed){
    if(speed < 0){

        ledcWrite(ch_PWM, abs(speed));
        digitalWrite(pin_CCW, HIGH);
        digitalWrite(pin_CW, LOW);
    }
    else if(speed > 0){
        ledcWrite(ch_PWM, abs(speed));
        digitalWrite(pin_CCW, LOW);
        digitalWrite(pin_CW, HIGH);
    }
    else{
        ledcWrite(ch_PWM, 0);
        digitalWrite(pin_CCW, LOW);
        digitalWrite(pin_CW, LOW);
    }
}