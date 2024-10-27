#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "../ecu_layer/ultrasonic.h"

float dis_1,dis_2,dis_3,dis_4;
UltrasonicSensor sensor_1={
		.Channel=channel_1,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim2,
		.distance=&dis_1

};
UltrasonicSensor sensor_2={
		.Channel=channel_3,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim3,
		.distance=&dis_2

};
UltrasonicSensor sensor_3={
		.Channel=channel_3,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim4,
		.distance=&dis_3

};
UltrasonicSensor sensor_4={
		.Channel=channel_4,
		.TRIG_PORT=GPIOA,
		.TRIG_PIN=GPIO_PIN_7,
		.htim=&htim5,
		.distance=&dis_4

};
void SystemClock_Config(void);
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM4_Init();

  Ultrasonic_Init(4,&sensor_1,&sensor_2,&sensor_3,&sensor_4);
  while (1)
  {
	  HAL_Delay(500);
	  Ultrasonic_ReadDistance(4,&sensor_1,&sensor_2,&sensor_3,&sensor_4);
  }

}
