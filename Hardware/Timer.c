#include "stm32f10x.h"                  // Device header

void timerInit()
{
	//该部分定时器我们选择定时器2，其是挂载在APB1总线上的，故这里选择的是APB1来开启RCC内部时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//选择定时器的触发为内部的RCC时钟触发，并将该触发供到TIM2，即定时器2上
	//此配置将打通RCC与定时器2时基单元的连接
	TIM_InternalClockConfig(TIM2);
	
/*********时基单元参数配置************/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	//选择滤波的取样点相关参数进行配置，该项参数配置对该工程影响不打，随机取TIM_CKD_DIV1
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	//选择计数器的计数模式为向上计数，即从0开始递增计数
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	//配置自动重装载寄存器的参数，该参数配为：当计数器计数10000次时进行重装
	TIM_TimeBaseInitStruct.TIM_Period = (10000 - 1);
	//配置预分频器的参数，该参数配为分频7200倍，此时接入计数器的频率变为72MHz/7200=10000Hz
	//此时配合自动重装载寄存器的参数配置，可以使得计数器1s计数10000次而满足重装载条件，并申请中断
	//即此配置下计数器1s申请一次中断
	TIM_TimeBaseInitStruct.TIM_Prescaler = (7200 - 1);
	//通用寄存器中没有重复次数计数器，该参数配0即可
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	//TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct); //此处犯错，不是使用这个函数！！！
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct); //应该用该函数！！！同时该函数执行完成后会有一个副作用：中断标志位会被置SET
	//后续如果不将该位清0，那么正式中断一开始就会导致马上执行一次中断函数，因此我们需要赶在正式中断开始前手动把中断标志位置0
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);	//此函数完成把中断标志位置0的工作
	
//至此，定时器2的时基单元参数配置完成

/*********中断输出控制参数配置************/
	//选择定时器2，并设置为“更新中断”模式（暂时不是太理解这个配置的作用）
	//此配置将打通定时器2与NVIC的连通
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

/*********NVIC输出控制参数配置************/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);//该语句适配完成后正式开启中断
	//如果此时中断标志位为SET，则不用动计数器开启，定时器2马上就会向CPU申请一次中断去执行中断函数
	//由此可体现出在TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct)完成后
	//执行TIM_ClearFlag(TIM2, TIM_FLAG_Update)的必要性
	
//运行控制，打开CNT计数器开始计数，
	TIM_Cmd(TIM2, ENABLE);
}

//void TIM2_IRQHandler()
//{
//	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//	}
//}	
