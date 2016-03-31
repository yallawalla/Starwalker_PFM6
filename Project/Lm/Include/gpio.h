#ifndef					GPIO_H
#define					GPIO_H
#include				"stm32f2xx.h"

#define	__MASK	0xf800
#define	__FOOT	0xf800
#define	__DOOR	0xf800

class	_GPIO { 
	private:
		int	key,timeout;
	public:
		_GPIO(void);
		int	Poll(void);
};
#endif
