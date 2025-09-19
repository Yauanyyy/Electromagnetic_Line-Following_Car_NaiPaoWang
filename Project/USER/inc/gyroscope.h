#ifndef __GYROSCOPE_H
#define __GYROSCOPE_H

#include "headfile.h"

void IMU_offset();
void gyro_low_pass_filter();
void acc_low_pass_filter();

#endif
