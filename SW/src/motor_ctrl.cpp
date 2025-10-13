#include<common.h>
#include<Motor_driver.h>
#include <stuPID.h>
#include <atomic>



//Atoms for thread safety
std::atomic<int32_t> ELE_counts(0);
std::atomic<int32_t> AZI_counts(0);


// When A pin is asserted this will run
void IRAM_ATTR elevation_ISR() {
    int state = (GPIO.in1.data >> (ELE_B_PIN - 32)) & 0x1; //Flækker portreisgre direkte

    if (state)
        ELE_counts++;
    else
        ELE_counts--;
}



void IRAM_ATTR azimuth_ISR() {
    int state = (GPIO.in1.data >> (AZI_B_PIN - 32)) & 0x1; //Flækker portreisgre direkte
    if (state)
        AZI_counts--;
    else
        AZI_counts++;
}



static motor AZI_motor;
static motor ELE_motor;
QueueHandle_t azi_pos = NULL;
QueueHandle_t ele_pos = NULL;


void set_azi_deg(float pos){

    int temp = pos/360.0*AZI_counts_pr_rev;
    xQueueSend(azi_pos, &temp, 0);
}

void set_ele_deg(float pos){
    int temp = pos/360.0*ELE_counts_pr_rev;

    xQueueSend(ele_pos, &temp, 0);
}


void set_azi(int32_t pos){
    xQueueSend(azi_pos, &pos, 0);
}

void set_ele(int32_t pos){
    xQueueSend(ele_pos, &pos, 0);
}

int32_t get_ele(){
    return ELE_counts;
}
int32_t get_azi(){
    return AZI_counts;
}

float get_ele_deg(){
    return (float)ELE_counts * 360.0/(float)ELE_counts_pr_rev;
}

float get_azi_deg(){
    return (float)AZI_counts * 360.0/(float)AZI_counts_pr_rev;
}




void task_motor_ctrl(void *paramters){


    azi_pos = xQueueCreate(2, sizeof(int32_t));
    ele_pos = xQueueCreate(2, sizeof(int32_t));


    pinMode(AZI_home_PIN, INPUT);
    pinMode(ELE_home_PIN, INPUT);
    pinMode(AZI_B_PIN, INPUT);
    pinMode(ELE_B_PIN, INPUT);
    pinMode(AZI_A_PIN, INPUT_PULLDOWN);
    pinMode(ELE_A_PIN, INPUT_PULLDOWN);
    pinMode(status_LED1_PIN, OUTPUT);
    attachInterrupt(ELE_A_PIN, elevation_ISR, RISING);
    attachInterrupt(AZI_A_PIN, azimuth_ISR, RISING);

    int T_s = 10; //Sampling period for our controller set to 100Hz
    TickType_t lastWake = xTaskGetTickCount();

    PID_ctrl AZI_ctrl(-200, 200, T_s, 0.6, 1, 0.1);
    PID_ctrl ELE_ctrl(-255, 255, T_s, 10, 3, 0.6);

    pinMode(ELE_CW_PIN, OUTPUT);
    digitalWrite(ELE_CW_PIN,LOW);
    pinMode(ELE_CCW_PIN, OUTPUT);
    digitalWrite(ELE_CCW_PIN, LOW);
    AZI_motor.begin(AZI_CW_PIN, AZI_CCW_PIN, AZI_PWM_PIN, 0);
    ELE_motor.begin(ELE_CW_PIN, ELE_CCW_PIN, ELE_PWM_PIN, 1);

    int i = 0;
    bool mode = false;
    while(true){
        int azi_pos_buffer = 0;
        int ele_pos_buffer = 0;
        if(xQueueReceive(azi_pos, &azi_pos_buffer,0) == pdTRUE){
            AZI_ctrl.set_point(azi_pos_buffer);
        }
        if(xQueueReceive(ele_pos, &ele_pos_buffer, 0) == pdTRUE){
            ELE_ctrl.set_point(ele_pos_buffer);
        }

        //Serial.print("RALLAHRALLAH");
        int AZI_counts_local = AZI_counts;
        int ELE_counts_local = ELE_counts;
        int PWM_azi =AZI_ctrl.compute(AZI_counts_local);
        int PWM_ele = ELE_ctrl.compute(ELE_counts_local);
        AZI_motor.set_speed(PWM_azi);
        ELE_motor.set_speed(PWM_ele);

        
        //Serial.printf("AZI: %d", AZI_counts_local);


        //digitalWrite(status_LED1_PIN, !digitalRead(status_LED1_PIN));
        xTaskDelayUntil(&lastWake, pdMS_TO_TICKS(T_s));
    }
    
}