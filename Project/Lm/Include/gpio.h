#ifndef					GPIO_H
#define					GPIO_H
#include				"stm32f2xx.h"

class	_GPIO { 
	private:
		int	key,timeout;
	public:
		_GPIO(void);
		int	Poll(void);
};

/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
class _TERM {
	private:
		int			seq, timeout;
		_GPIO		gp;
	public:
		_TERM() {
			seq=timeout=0;
			clp=cl;
		};
		char cl[128],*clp;
		void Repeat(int);
		int	 Escape(void);
};

#endif
