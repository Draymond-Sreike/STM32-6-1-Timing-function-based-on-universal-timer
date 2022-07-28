#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"

uint16_t time;

int main()
{
	OLED_Init();
	timerInit();
	OLED_ShowString(1, 1, "Time:");
	OLED_ShowString(2, 1, "Counter:");
	while(1)
	{
		OLED_ShowNum(1, 6, time,5);					//显示计数器的中断更新值，1s申请一次中断，1s加1，即该项显示“秒”
		OLED_ShowNum(2, 9, TIM_GetCounter(TIM2),5);	//显示计数器从0到999的装载过程值
	}
}

void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		time++;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}	
