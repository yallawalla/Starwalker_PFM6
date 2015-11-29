#include	"pfm.h"
/*******************************************************************************
* Function Name	: Timer_Init
* Description		: Configure timer pins as output open drain
* Output				 : TIM4
* Return				 : None
*******************************************************************************/
void 	Init_Pyro() {
TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
TIM_OCInitTypeDef					TIM_OCInitStructure;
GPIO_InitTypeDef					GPIO_InitStructure;
EXTI_InitTypeDef   				EXTI_InitStructure;

		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		TIM_OCStructInit(&TIM_OCInitStructure);

		TIM_TimeBaseStructure.TIM_Prescaler = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
		TIM_TimeBaseStructure.TIM_RepetitionCounter=1;

// TIM4

		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

		GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

 		TIM_DeInit(TIM4);
		TIM_TimeBaseStructure.TIM_Period = 420;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);

		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_Pulse=380;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);

		TIM_CtrlPWMOutputs(TIM4, ENABLE);
		TIM_ITConfig(TIM4, TIM_IT_Update,ENABLE);
}

/*******************************************************************************/
/**
	* @brief	TIM4 IC2 ISR
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
void 	TIM4_IRQHandler(void){
static 
int		n=0,data=0;
			if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
			TIM_ClearFlag(TIM4, TIM_FLAG_Update);
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
						
			if(!(TIM4->CR1 & 0x10)) {
				if(GPIO_ReadOutputDataBit(GPIOD,GPIO_Pin_15)==SET)	
					data |= 1;
					data <<= 1;
				} else {		
					if(!(++n % 42)) {
						TIM_Cmd(TIM4,DISABLE);
						data = 0;
					}	
				}
			}
}
