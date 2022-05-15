#include "mpu6500.h"
#include "main.h"
#include "stm32f4xx_hal.h"

extern SPI_HandleTypeDef hspi2;

#define WHO_AM_I 0x75
#define PWR_MGMT_1 0x6B
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define GYRO_FACTOR 16.4
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

uint8_t read_byte(uint8_t reg)
{
  uint8_t rx_data[2];
  uint8_t tx_data[2];

  tx_data[0] = reg | 0x80;
  tx_data[1] = 0x00;  // dummy

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi2, tx_data, rx_data, 2, 1);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);

  return rx_data[1];
}

void write_byte(uint8_t reg, uint8_t data)
{
  uint8_t rx_data[2];
  uint8_t tx_data[2];

  tx_data[0] = reg & 0x7F;
  tx_data[1] = data;  // write data

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi2, tx_data, rx_data, 2, 1);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
}

void mpu6500_init( void )
{
  uint8_t who_am_i;

  HAL_Delay( 100 ); // wait start up
  who_am_i = read_byte( WHO_AM_I ); // 1. read who am i
  printf( "\r\n0x%x\r\n",who_am_i ); // 2. check who am i value
  HAL_Delay( 10 );
  who_am_i = read_byte( WHO_AM_I ); // 1. read who am i
  printf( "0x%x\r\n",who_am_i ); // 2. check who am i value

  // 2. error check
  if ( who_am_i != 0x70 ){
    while(1){
      printf( "gyro_error\r\n");
    }
  }

  HAL_Delay( 50 ); // wait
  write_byte( PWR_MGMT_1, 0x00 ); // 3. set pwr_might
  HAL_Delay( 50 );
  write_byte( CONFIG, 0x00 ); // 4. set config
  HAL_Delay( 50 );
  write_byte( GYRO_CONFIG, 0x18 ); // 5. set gyro config
  HAL_Delay( 50 );
}

float mpu6500_read_gyro_z( void )
{
  int16_t gyro_z;
  float omega;

  // H:8bit shift, Link h and l
  gyro_z = (int16_t)( (int16_t)(read_byte(GYRO_ZOUT_H) << 8 ) | read_byte(GYRO_ZOUT_L) );

  omega = (float)( gyro_z / GYRO_FACTOR ); // dps to deg/sec

  return omega;
}
