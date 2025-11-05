#include <common.h>









// Declare task handle
TaskHandle_t Ctrl_taskhandle;

// Declare task handle
TaskHandle_t Socket_taskhandle;

// Declare task handle
TaskHandle_t BT_taskhandle;

// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  xTaskCreatePinnedToCore(task_motor_ctrl, "ctrl", 10000, NULL, 5, &Ctrl_taskhandle, 1);
  //xTaskCreatePinnedToCore(task_socket, "Socket", 10000, NULL, 2, &Socket_taskhandle, 1);
  xTaskCreatePinnedToCore(task_bt_serial, "BT", 10000, NULL, 4, &BT_taskhandle, 1);
}


void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}