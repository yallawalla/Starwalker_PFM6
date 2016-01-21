/**
  ******************************************************************************
  * @file    pilot.cpp
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief
  *
  */ 
/** @addtogroup
* @{
*/

#include	"pilot.h"
/*******************************************************************************/
/**
	* @brief
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
_PILOT::_PILOT() {
			value=count=0;
}
/*******************************************************************************/
/**
	* @brief
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
_PILOT::~_PILOT() {
}
/*******************************************************************************/
/**
	* @brief
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
void	_PILOT::Poll(void) {
			if(count < value)
				GPIO_ResetBits(GPIOD,GPIO_Pin_13);
			else
				GPIO_SetBits(GPIOD,GPIO_Pin_13);
			count = (count + 5) % 100;
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void	_PILOT::LoadSettings(FILE *f) {
char	c[128];
			fgets(c,sizeof(c),f);
			sscanf(c,"%d",&value);
}
/*******************************************************************************/
/**
	* @brief	TIM3 IC2 ISR
	* @param	: None
	* @retval : None
	*/
void	_PILOT::SaveSettings(FILE *f) {
			fprintf(f,"%5d                   /.. pilot\r\n",value);
}
/**
* @}
*/ 

