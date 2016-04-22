/**
  ******************************************************************************
  * @file    states.cpp
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief
  *
  */ 
/** @addtogroup
* @{
*/

#include	"state.h"
/*******************************************************************************/
_ERROR			_Error;
_ACTIVE			_Active;
_READY			_Ready;
_STANDBY		_Standby;
/*******************************************************************************/
_STATE *_STATE::active=NULL;
/*******************************************************************************/
void _STANDBY::OnEvent(Event e) {
		switch(e) {
			case evt_Error:
				stateChange(&_Error);
				break;
			case req_Ready:
				stateChange(&_Ready);
				break;
			default:
				break;
		}
}
/*******************************************************************************/
void _READY::OnEvent(Event e) {
		switch(e) {
			case evt_Timeout:
			case req_Standby:
				stateChange(&_Standby);
				break;
			case req_Active:
				stateChange(&_Active);
				break;
			case evt_Error:
				stateChange(&_Error);
				break;
			default:
				break;
		}
}
/*******************************************************************************/
void _ACTIVE::OnEvent(Event e) {
		switch(e) {
			case evt_Error:
				stateChange(&_Error);
				break;
			case req_Ready:
				stateChange(&_Ready);
				break;
			default:
				break;
		}
}
/*******************************************************************************/
void _ACTIVE::OnIdle() {
	if(!(__time__ % 1000))
		printf(".");
}
/*******************************************************************************/
void _ERROR::OnEvent(Event e) {
		switch(e) {
			case evt_Timeout:
			case req_Standby:
				stateChange(&_Standby);
				break;
			default:
				break;
		}
}












/**
* @}
*/ 

