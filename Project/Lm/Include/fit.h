#ifndef					FIT_H
#define					FIT_H
#include				"stm32f2xx.h"

enum _fittype		{FIT_POW, FIT_TRIG, FIT_EXP};

class _FIT {
	private:
		double	*tp,*fp,per;
		double	det(double *,int,int,int);
	public:
		int			n,typ;
		double	*rp;			
		_FIT(int = 3, _fittype=FIT_POW);
		~_FIT();
		int			Sample(double, double);
		int			Sample(double, double, double);
		double	*Compute(void);
		double	Poly(double);
	
	};
#endif
