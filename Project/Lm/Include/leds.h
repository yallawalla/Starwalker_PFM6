#ifndef					WS2812_H
#define					WS2812_H

#include				"stm32f2xx.h"
#include 				<string>
#include				<stdlib.h>
#include 				<sstream>
#include 				<vector>

typedef struct	{unsigned char r; unsigned char g; unsigned char b; }	RGB_set;
typedef struct	{signed short h; unsigned char s; unsigned char v; 	}	HSV_set;
typedef enum		{noCOMM, FILL, FILL_LEFT, FILL_RIGHT}	command;
typedef struct	{
									int g[8];
									int r[8];
									int b[8];
								} dma;

typedef	struct	{
									int				size,
														dt;
									HSV_set		color, 
														*cbuf;
									command		mode;
									dma 			*lbuf;
								} ws2812;

class	_WS2812 {
	private:
		void 		RGB2HSV( RGB_set, HSV_set *);
		void		HSV2RGB( HSV_set, RGB_set *);
		void		trigger(void);
		static 	ws2812 	Led[];
		dma			*dma_buffer;
		int			dma_size;
//______________________________________________________________________________________
	public:
		_WS2812(void);
		~_WS2812(void);
		int		SetColor(std::string);
		static void	*procLeds(_WS2812 *);
};

#endif
