#ifndef __MPU6500_H
#define __MPU6500_H

#include <stdint.h>
#include <stdio.h>

uint8_t read_byte( uint8_t reg );

void write_byte( uint8_t reg, uint8_t val );

void mpu6500_init( void );

float mpu6500_read_gyro_z( void );

#endif /* __MPU6500_H */
