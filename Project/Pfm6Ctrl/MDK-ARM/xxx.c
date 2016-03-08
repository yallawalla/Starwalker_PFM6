#include "pfm.h"
#include <math.h>
//
//_________________________________________________________________________________
// PB10, TIM2, ch 3
//
//
void		trigger(void);
void		*procLeds(void *);

#define N 80
typedef enum {NONE, FILL, FILL_FROM_LEFT, FILL_FROM_RIGH} rgbcom;

struct _dma {
	int g[8],r[8],b[8];
} dma[N+1];

struct		rgb {
	int			size,t,dt;
	union		{
		int		i; 
		char	c[4];
	} color, *color_buffer;
	rgbcom	command;
	struct _dma *dma;
} rgb[] = {	
	{8,0,0,0,NULL,NONE,&dma[0]},
	{24,0,0,0,NULL,NONE,&dma[8]},
	{8,0,0,0,NULL,NONE,&dma[32]},
	{8,0,0,0,NULL,NONE,&dma[40]},
	{24,0,0,0,NULL,NONE,&dma[48]},
	{8,0,0,0,NULL,NONE,&dma[72]},
	{0,0,0,0,NULL,NONE,NULL}
};
//_________________________________________________________________________________
void	SetRgb(int n, int r,int g,int b) {
int i;
	for(i=0; i<8; ++i) {
		(r & (0x80>>i)) ? (dma[n].r[i]=53) : (dma[n].r[i]=20);
		(g & (0x80>>i)) ? (dma[n].g[i]=53) : (dma[n].g[i]=20);
		(b & (0x80>>i)) ? (dma[n].b[i]=53) : (dma[n].b[i]=20);
	}
}
//_________________________________________________________________________________
void 	init_TIM() {
TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
TIM_OCInitTypeDef					TIM_OCInitStructure;
DMA_InitTypeDef						DMA_InitStructure;
GPIO_InitTypeDef					GPIO_InitStructure;
//
// ________________________________________________________________________________
// TIM2

			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			
			GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
	
// DMA setup _____________________________________________________________________
			DMA_StructInit(&DMA_InitStructure);
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
			DMA_DeInit(DMA1_Stream1);
			DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dma;
			DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
			DMA_InitStructure.DMA_BufferSize = sizeof(dma)/sizeof(int)+1;
			DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
			DMA_InitStructure.DMA_Priority = DMA_Priority_High;
			DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
			DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
			DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
			DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
			DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
			DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

			DMA_InitStructure.DMA_Channel = DMA_Channel_3;
			DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM2_BASE + 0x4C;	//~~~
			DMA_Init(DMA1_Stream1, &DMA_InitStructure);
// ________________________________________________________________________________
// TIMebase setup
			TIM_DeInit(TIM2);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
			
			TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
			TIM_TimeBaseStructure.TIM_Prescaler = 0;
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
			TIM_TimeBaseStructure.TIM_Period = 74;

			TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
// ________________________________________________________________________________
// Output Compare
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
			TIM_OCInitStructure.TIM_Pulse=0;
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
			TIM_OC3Init(TIM2, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
// ________________________________________________________________________________
// Startup
			TIM_CtrlPWMOutputs(TIM2, ENABLE);
			TIM_Cmd(TIM2,ENABLE);

			TIM_DMAConfig(TIM2, TIM_DMABase_CCR3, TIM_DMABurstLength_1Transfer);
			TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
			
			if(!App_Find(procLeds,NULL)) {
				int i;
				for(i=0; rgb[i].dma; ++i)
					if(rgb[i].color_buffer==NULL)
						rgb[i].color_buffer=calloc(rgb[i].size,sizeof(int));
				App_Add(procLeds,NULL,"procLeds",30);	
			}
		}
/*******************************************************************************/
/**
	* @brief	Trigger call
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
void		trigger() {
int			i,j,k;
struct 	_dma	*p;
	
				for(i=0; rgb[i].size; ++i)
					for(j=0; j<rgb[i].size; ++j)
						if(rgb[i].color_buffer)
							for(k=0,p=rgb[i].dma; k<8; ++k) {
								(rgb[i].color_buffer[j].i & (0x000080>>k)) ? (p[j].b[k]=53)	: (p[j].b[k]=20);
								(rgb[i].color_buffer[j].i & (0x008000>>k)) ? (p[j].g[k]=53)	: (p[j].g[k]=20);
								(rgb[i].color_buffer[j].i & (0x800000>>k)) ? (p[j].r[k]=53)	: (p[j].r[k]=20);
							}
						else
							for(k=0,p=rgb[i].dma; k<24; ++k)
									p[j].g[k]=20;
				
				DMA_Cmd(DMA1_Stream1, DISABLE);
				TIM_Cmd(TIM2,DISABLE);
				TIM_SetCounter(TIM2,0);
				while(DMA_GetCmdStatus(DMA1_Stream1) != DISABLE);
				DMA_SetCurrDataCounter(DMA1_Stream1,sizeof(dma)/sizeof(int));
				DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_HTIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_DMEIF1	| DMA_FLAG_FEIF1 | DMA_FLAG_TCIF1);
				DMA_Cmd(DMA1_Stream1, ENABLE);
				TIM_Cmd(TIM2,ENABLE);
}
/*******************************************************************************/
/**
	* @brief	Trigger call
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
void		trigger_TIM() {
	
//	int			i,j,k;
//	int			r[N],g[N],b[N];

/*	
	do {
		for(i=1; i<8; ++i) {
			for(j=0; j<N; ++j) {
				(i & 1) ? (r[j]=j*5+5) : (r[j]=0);
				(i & 2) ? (g[j]=j*5+5) : (g[j]=0);
				(i & 4) ? (b[j]=j*5+5) : (b[j]=0);
				rgb(j,r[j],g[j],b[j]);
				trigger();
				Wait(5,App_Loop);
			}
			do {
				k=0;
				for(j=0; j<N; ++j) {
					(--r[j] < 0)?(r[j] = 0):(++k);
					(--g[j] < 0)?(g[j] = 0):(++k);
					(--b[j] < 0)?(b[j] = 0):(++k);
					rgb(j,r[j],g[j],b[j]);
				}
				trigger();
				Wait(10,App_Loop);
			} while(k);
		}
	} while(getchar()==EOF);
*/
}
//______________________________________________________________________________________
void			*procLeds(void *v) {
int				i,j;
	
					for(i=0; rgb[i].dma; ++i)
						switch(rgb[i].command) {
							case NONE:
								break;
							case FILL:
								if(rgb[i].t++ ==  rgb[i].dt)
									rgb[i].t=0;
								
								for(j=0; j<rgb[i].size; ++j) {
									if(rgb[i].color_buffer[j].c[0] > rgb[i].color.c[0]) 
										--rgb[i].color_buffer[j].c[0];
									else if(rgb[i].color_buffer[j].c[0] < rgb[i].color.c[0]) 
										++rgb[i].color_buffer[j].c[0];
									else if(rgb[i].color_buffer[j].c[1] > rgb[i].color.c[1]) 
										--rgb[i].color_buffer[j].c[1];
									else if(rgb[i].color_buffer[j].c[1] < rgb[i].color.c[1]) 
										++rgb[i].color_buffer[j].c[1];
									else if(rgb[i].color_buffer[j].c[2] > rgb[i].color.c[2]) 
										--rgb[i].color_buffer[j].c[2];
									else if(rgb[i].color_buffer[j].c[2] < rgb[i].color.c[2]) {
										++rgb[i].color_buffer[j].c[2];
										break;
										}
									}
								break;
							case FILL_FROM_LEFT:
								for(j=1; j<rgb[i].size; ++j)
									rgb[i].color_buffer[j].i=rgb[i].color.i;

								break;
							default:
							 break;
						}

					trigger();
					return NULL;
}
//______________________________________________________________________________________
int				SetColor(char *c) {
					char		*cc[8];
					if(!c) {
						__print("\r\ncolor>");
						__stdin.io->arg.parse=SetColor;
						init_TIM();
					} else {
						switch(*c) {
//__________________________________________________
						case 'c':
							if(numscan(++c,cc,',')==4) {
								rgb[atoi(cc[0])].color.i=atoi(cc[3]) + (atoi(cc[2])<<8) + (atoi(cc[1])<<16);
							}	else
								return _PARSE_ERR_SYNTAX;
							break;

//__________________________________________________
						case 'f':
							if(numscan(++c,cc,',')==2) {
								rgb[atoi(cc[0])].dt=atoi(cc[1]);
								rgb[atoi(cc[0])].command=FILL;
							}	else
								return _PARSE_ERR_SYNTAX;
							break;
//______________________________________________________________________________________
						case '>':
							__stdin.io->arg.parse=DecodeCom;
							return(DecodeCom(NULL));
					}
				}	
				return _PARSE_OK;
}
//______________________________________________________________________________________
struct RGB_set {
 unsigned char r;
 unsigned char g;
 unsigned char b;
} RGB_set;
 
struct HSV_set {
 signed int h;
 unsigned char s;
 unsigned char v;
} HSV_set;

/*******************************************************************************
 * Function RGB2HSV
 * Description: Converts an RGB color value into its equivalen in the HSV color space.
 * Copyright 2010 by George Ruinelli
 * The code I used as a source is from http://www.cs.rit.edu/~ncs/color/t_convert.html
 * Parameters:
 *   1. struct with RGB color (source)
 *   2. pointer to struct HSV color (target)
 * Notes:
 *   - r, g, b values are from 0..255
 *   - h = [0,360], s = [0,255], v = [0,255]
 *   - NB: if s == 0, then h = 0 (undefined)
 ******************************************************************************/
void RGB2HSV(struct RGB_set RGB, struct HSV_set *HSV){
 unsigned char min, max, delta;
 
 if(RGB.r<RGB.g)min=RGB.r; else min=RGB.g;
 if(RGB.b<min)min=RGB.b;
 
 if(RGB.r>RGB.g)max=RGB.r; else max=RGB.g;
 if(RGB.b>max)max=RGB.b;
 
 HSV->v = max;                // v, 0..255
 
 delta = max - min;                      // 0..255, < v
 
 if( max != 0 )
 HSV->s = (int)(delta)*255 / max;        // s, 0..255
 else {
 // r = g = b = 0        // s = 0, v is undefined
 HSV->s = 0;
 HSV->h = 0;
 return;
 }
 
 if( RGB.r == max )
 HSV->h = (RGB.g - RGB.b)*60/delta;        // between yellow & magenta
 else if( RGB.g == max )
 HSV->h = 120 + (RGB.b - RGB.r)*60/delta;    // between cyan & yellow
 else
 HSV->h = 240 + (RGB.r - RGB.g)*60/delta;    // between magenta & cyan
 
 if( HSV->h < 0 )
 HSV->h += 360;
}
/*******************************************************************************
 * Function HSV2RGB
 * Description: Converts an HSV color value into its equivalen in the RGB color space.
 * Copyright 2010 by George Ruinelli
 * The code I used as a source is from http://www.cs.rit.edu/~ncs/color/t_convert.html
 * Parameters:
 *   1. struct with HSV color (source)
 *   2. pointer to struct RGB color (target)
 * Notes:
 *   - r, g, b values are from 0..255
 *   - h = [0,360], s = [0,255], v = [0,255]
 *   - NB: if s == 0, then h = 0 (undefined)
 ******************************************************************************/
void HSV2RGB(struct HSV_set HSV, struct RGB_set *RGB){
 int i;
 float f, p, q, t, h, s, v;
 
 h=(float)HSV.h;
 s=(float)HSV.s;
 v=(float)HSV.v;
 
 s /=255;
 
 if( s == 0 ) { // achromatic (grey)
 RGB->r = RGB->g = RGB->b = v;
 return;
 }
 
 h /= 60;            // sector 0 to 5
 i = floor( h );
 f = h - i;            // factorial part of h
 p = (unsigned char)(v * ( 1 - s ));
 q = (unsigned char)(v * ( 1 - s * f ));
 t = (unsigned char)(v * ( 1 - s * ( 1 - f ) ));
 
 switch( i ) {
 case 0:
 RGB->r = v;
 RGB->g = t;
 RGB->b = p;
 break;
 case 1:
 RGB->r = q;
 RGB->g = v;
 RGB->b = p;
 break;
 case 2:
 RGB->r = p;
 RGB->g = v;
 RGB->b = t;
 break;
 case 3:
 RGB->r = p;
 RGB->g = q;
 RGB->b = v;
 break;
 case 4:
 RGB->r = t;
 RGB->g = p;
 RGB->b = v;
 break;
 default:        // case 5:
 RGB->r = v;
 RGB->g = p;
 RGB->b = q;
 break;
 }
}
