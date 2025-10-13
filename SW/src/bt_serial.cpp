
#include <common.h>
#ifdef BT_ESP32
#include <BluetoothSerial.h>
#include<Arduino.h>




BluetoothSerial SerialBT;




#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
The communication protocol, see https://github.com/aausat/C3-351-yagi-control/blob/hamlib/ESP32/GS_easycomm2_controller/GS_easycomm2_controller.ino

RPI <-> ESP32 uses the easycomm2 standard for communication (see https://jensd.dk/doc/yaesu/ for explanation).
The following commands are implemented:
Command           Meaning                     Parameters
-------           --------                    --------
AT                AutoTrack (AAU GS only)     satID - int of length 5 eg.32788 AKA AAUSAT II
SAT               Stop Auto Tracking (only works when AT has been called)

AZ                Azimuth                     number - 1 decimal place (XXX.X)
EL                Elevation                   number - 1 decimal place (YYY.Y)

ML                Move left                   
MR                Move right
MU                Move up
MD                Move down
SA                Stop azimuth
SE                Stop elevation

VE                Request version
AC                Automaticaly Calibrate min. and max. voltage values read from AZ and EL potentiometers on motors
PP                Print Position of antenna in AZ and EL
*/

void cmd_SE(char* arg, size_t len){
    SerialBT.print("OK\n");
    SerialBT.flush();
}


void cmd_AZ(char* arg, size_t len){
    if(len == 0){
        float angle = (get_azi()); // round to nearest degree
        SerialBT.printf("AZ%.1f", angle);     
        SerialBT.flush();
        return;
    }
    //Destroy our buffer
    arg[len] = 0;
    float angle = atof(arg);    
    set_azi(angle);
}

void cmd_EL(char* arg, size_t len){
    if(len == 0){
        float angle = (get_ele());
        SerialBT.printf("EL%.1f", angle);       
        SerialBT.flush();
        return;
    }

    //Destroy our buffer
    arg[len] = 0;
    float angle = atof(arg);

    set_ele(angle);

    
}

void cmd_ST(char* arg, size_t len){
    //LIES and DECEPTION
    Serial.print("OK");
    Serial.flush();

}






//Argument for den her struktur. Det nemt at tilføje kommandoer. og vi har massere af ram og regnekraft så vi smider bare stringen med over i vores command handlers.
typedef void (*cmd_func_t)(char* arg, size_t len);

typedef struct { 
    char cmd[3]; 
    cmd_func_t func; 
} ezcom_cmd_t;



ezcom_cmd_t cmd_table[]{
    {"AZ", cmd_AZ},
    {"EL", cmd_EL},
    {"ST", cmd_ST},
    {"SE", cmd_SE}
};





bool cmp_buffer(const char* A_arr, const char* B_arr, size_t sizeA, size_t sizeB) {
    size_t searchsize = std::min(sizeA, sizeB);
    for (size_t i = 0; i < searchsize; ++i) {
        if (A_arr[i] != B_arr[i])
            return false;
    }
    return true;
}





char buffer[128] = {0};
size_t buffer_index = 0;


void task_bt_serial(void *parameter){

    SerialBT.begin("TARM");

    //Serial.printf("[Easycomm Serial]: Number of implented commands: %d \n", ARRAY_SIZE(cmd_table));
    //Serial.println("[Easycomm Serial]: Ready");

    while(true){
        if(!SerialBT.available()){
            delay(5);   
            continue;
        }

        char key = SerialBT.read();

        if (buffer_index >= sizeof(buffer)){
            buffer_index = 0; // avoid overflow if serial garbage arrives  
        }
            
        if(key != '\n' && key != 0x20){
            buffer[buffer_index++] = key;
        }
        

        if(key != '\n' && key != 0x20 ){ //If they're not a complete command return
            continue;
        }
        if(buffer_index < 2){ //Invalid length
            continue;
        }
        //If we are here we've gotten a command.

        char cmd[]=  {buffer[0], buffer[1], 0};

        for (size_t i = 0; i < ARRAY_SIZE(cmd_table); i++) //Kørrer igennem alle vores cmds
        {
            if(cmp_buffer(cmd, cmd_table[i].cmd, sizeof(cmd), sizeof(cmd_table[i].cmd))){
                //if we get a match
                
                cmd_table[i].func(&buffer[2], buffer_index-2);

                //We've interpreted a command is it the last or are there more?
                if(SerialBT.peek() == 0x0a){
                    SerialBT.print('\n');
                }
                else{
                    SerialBT.print(' ');
                }
                buffer_index =0;
                
                break;
            }
        }
        


        buffer_index =0;

    }
}
#endif