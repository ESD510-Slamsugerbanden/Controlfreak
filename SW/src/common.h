#include <Arduino.h>
#include <HAL.h>

#define AZI_counts_pr_rev (6330*2)
#define ELE_counts_pr_rev (280*4)

void task_motor_ctrl(void *parameter);
void task_socket(void* parameters);

void task_bt_serial(void *parameter);

/// @brief Sets the target for the controller
/// @param pos Position given in raw counts
void set_azi(int32_t pos); //Sets position for azimuth

/// @brief Set position in degrees
/// @param pos 
void set_azi_deg(float pos);


/// @brief Set the target for elevation for the controller
/// @param pos Postion given in raw counts
void set_ele(int32_t pos); //Setus position for elevation


/// @brief Set position in degrees
/// @param pos 
void set_ele_deg(float pos);


/// @brief Gets the azimuth in raw counts
/// @return current encoder position
int32_t get_azi();

float get_azi_deg();

/// @brief Gets the elevation in raw counts
/// @return current encoder position
int32_t get_ele();


float get_ele_deg();
