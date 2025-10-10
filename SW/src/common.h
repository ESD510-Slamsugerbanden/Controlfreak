#include <Arduino.h>
#include <HAL.h>



void task_motor_ctrl(void *parameter);
void task_socket(void* parameters);


/// @brief Sets the target for the controller
/// @param pos Position given in raw counts
void set_azi(int32_t pos); //Sets position for azimuth

/// @brief Set the target for elevation for the controller
/// @param pos Postion given in raw counts
void set_ele(int32_t pos); //Setus position for elevation

/// @brief Gets the azimuth in raw counts
/// @return current encoder position
int32_t get_azi();

/// @brief Gets the elevation in raw counts
/// @return current encoder position
int32_t get_ele();