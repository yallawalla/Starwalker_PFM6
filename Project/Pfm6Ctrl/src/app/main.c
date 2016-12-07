/**
  ******************************************************************************
  * @file			main.c
  * @author		Fotona d.d.
  * @version	V1
  * @date			30-Sept-2013
  * @brief   	Initialization and main app. loop wrapper file
  *
  */

/** @addtogroup PFM6_Application
* @brief PFM6 application group
* @{
*/
#ifdef __DISC7__
void	F7_init(void);
#endif

#include	"pfm.h"
int				main(void) {
#ifdef __DISC7__
					F7_init();
#endif
					App_Init();
					while(1)
						_proc_loop();
}
/**
* @}
*/ 
