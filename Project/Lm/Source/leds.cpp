/**
	******************************************************************************
	* @file		leds.cpp
	* @author	Fotona d.d.
	* @version
	* @date
	* @brief	WS2812B driver class
	*
	*/
	
/** @addtogroup
* @{
*/

#include	"leds.h"
#include	"isr.h"
#include	<leds.h>

using std::string;
using std::vector;
using std::stringstream;

ws2812 _WS2812::Led[] = {	
					{8,0,{0,0,0},NULL,noCOMM,NULL},
					{24,0,{0,0,0},NULL,noCOMM,NULL},
					{8,0,{0,0,0},NULL,noCOMM,NULL},
					{8,0,{0,0,0},NULL,noCOMM,NULL},
					{24,0,{0,0,0},NULL,noCOMM,NULL},
					{8,0,{0,0,0},NULL,noCOMM,NULL},
					{0,0,{0,0,0},NULL,noCOMM,NULL}
				};
//_________________________________________________________________________________
_WS2812::_WS2812() {
TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
TIM_OCInitTypeDef					TIM_OCInitStructure;
DMA_InitTypeDef						DMA_InitStructure;
GPIO_InitTypeDef					GPIO_InitStructure;
//
// ________________________________________________________________________________
			if(!dma_buffer) {																			// first entry
				int i,j;
				for(i=j=0; Led[i].size; ++i)												// count number of leds
					j+=Led[i].size;
				dma_buffer=new dma[j+1];														// allocate dma buffer
				dma_size=j*sizeof(dma)/sizeof(int)+1;
				for(i=j=0; Led[i].size; ++i) {
					Led[i].cbuf=new HSV_set[Led[i].size];							// alloc color buffer
					Led[i].lbuf=&dma_buffer[j];												// pointer to dma tab
					j+=Led[i].size;
				}
				_thread_add((void *)procLeds,this,(char *)"procLeds",10);
			}	
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
			DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dma_buffer;
			DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
			DMA_InitStructure.DMA_BufferSize = dma_size;
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
		}
/*******************************************************************************/
/**
	* @brief	Trigger call
	* @param	: None
	* @retval : None
	*/
/*******************************************************************************/
void		_WS2812::trigger() {
int			i,j,k;
dma			*p;
RGB_set	q;
	
				for(i=0; Led[i].size; ++i)
					for(j=0; j<Led[i].size; ++j)
						if(Led[i].cbuf) {
							HSV2RGB(Led[i].cbuf[j], &q);
							for(k=0,p=Led[i].lbuf; k<8; ++k) {
								(q.b & (0x80>>k)) ? (p[j].b[k]=53)	: (p[j].b[k]=20);
								(q.g & (0x80>>k)) ? (p[j].g[k]=53)	: (p[j].g[k]=20);
								(q.r & (0x80>>k)) ? (p[j].r[k]=53)	: (p[j].r[k]=20);
							}
						}
						else
							for(k=0,p=Led[i].lbuf; k<24; ++k)
									p[j].g[k]=20;
				
				DMA_Cmd(DMA1_Stream1, DISABLE);
				TIM_Cmd(TIM2,DISABLE);
				TIM_SetCounter(TIM2,0);
				while(DMA_GetCmdStatus(DMA1_Stream1) != DISABLE);
				DMA_SetCurrDataCounter(DMA1_Stream1,dma_size);
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
void	 *_WS2812::procLeds(_WS2812 *me) {
int				i,j,k;
	
					for(i=0; Led[i].size; ++i) {
						switch(Led[i].mode) {
							case noCOMM:
								break;

							case FILL:
								for(j=k=0; j<Led[i].size;++j) {
									Led[i].cbuf[j].h = Led[i].color.h;
									Led[i].cbuf[j].s = Led[i].color.s;
									
									if(Led[i].cbuf[j].v < Led[i].color.v)
										Led[i].cbuf[j].v += (Led[i].color.v - Led[i].cbuf[j].v)/Led[i].dt+1;
									else if(Led[i].cbuf[j].v > Led[i].color.v)
										Led[i].cbuf[j].v -= (Led[i].cbuf[j].v - Led[i].color.v)/Led[i].dt+1;
									else
										++k;
								}
								if(k==Led[i].size)
									Led[i].mode=noCOMM;
								break;

							case FILL_RIGHT:
								j=Led[i].size; 
								k=0;
								while(--j) {
									Led[i].cbuf[j].h = Led[i].cbuf[j-1].h;
									Led[i].cbuf[j].s = Led[i].cbuf[j-1].s;
									
									if(Led[i].cbuf[j].v < Led[i].cbuf[j-1].v)
										Led[i].cbuf[j].v += (Led[i].cbuf[j-1].v - Led[i].cbuf[j].v)/Led[i].dt+1;
									else if(Led[i].cbuf[j].v > Led[i].cbuf[j-1].v)
										Led[i].cbuf[j].v -= (Led[i].cbuf[j].v - Led[i].cbuf[j-1].v)/Led[i].dt+1;
									else
										++k;
								}
								Led[i].cbuf[j].h = Led[i].color.h;
								Led[i].cbuf[j].s = Led[i].color.s;
								if(Led[i].cbuf[j].v < Led[i].color.v)
									Led[i].cbuf[j].v += (Led[i].color.v - Led[i].cbuf[j].v)/Led[i].dt+1;
								else if(Led[i].cbuf[j].v > Led[i].color.v)
									Led[i].cbuf[j].v -= (Led[i].cbuf[j].v - Led[i].color.v)/Led[i].dt+1;
								else
									++k;
								if(k==Led[i].size)
									Led[i].mode=noCOMM;
								break;
								
							case FILL_LEFT:
								for(j=k=0; j<Led[i].size-1;++j) {
									Led[i].cbuf[j].h = Led[i].cbuf[j+1].h;
									Led[i].cbuf[j].s = Led[i].cbuf[j+1].s;
									
									if(Led[i].cbuf[j].v < Led[i].cbuf[j+1].v)
										Led[i].cbuf[j].v += (Led[i].cbuf[j+1].v - Led[i].cbuf[j].v)/Led[i].dt+1;
									else if(Led[i].cbuf[j].v > Led[i].cbuf[j+1].v)
										Led[i].cbuf[j].v -= (Led[i].cbuf[j].v - Led[i].cbuf[j+1].v)/Led[i].dt+1;
									else
										++k;
								}
								Led[i].cbuf[j].h = Led[i].color.h;
								Led[i].cbuf[j].s = Led[i].color.s;
								if(Led[i].cbuf[j].v < Led[i].color.v)
									Led[i].cbuf[j].v += (Led[i].color.v - Led[i].cbuf[j].v)/Led[i].dt+1;
								else if(Led[i].cbuf[j].v > Led[i].color.v)
									Led[i].cbuf[j].v -= (Led[i].cbuf[j].v - Led[i].color.v)/Led[i].dt+1;
								else
									++k;
								if(k==Led[i].size)
									Led[i].mode=noCOMM;
								break;
								
							default:
								break;
							}
					}
					
					for(i=0; Led[i].size; ++i)
						if(Led[i].mode != noCOMM) {
							me->trigger();
							break;
					}
					return NULL;
}
//______________________________________________________________________________________
int				_WS2812::SetColor(string str) {
stringstream 	stream(str.substr(0,str.find(" ")));
vector<int> 	values(10);
int						i;

					while(stream >> i)
						values.push_back(i);
					
					switch(str.at(0)) {
//__________________________________________________
						case 't':
							stream >> i;
							if(i<5 || i>100)
								return PARSE_ILLEGAL;
							if(values.size() == 1)
								_thread_find(procLeds,NULL)->dt=values.at(0);	
							break;
//__________________________________________________
						case 'd':
							stream >> i;
							if(i<5 || i>10000)
								return PARSE_ILLEGAL;
							if(values.size() == 1)
								_thread_find(procLeds,NULL)->dt=values.at(0);	
							break;
							_wait(i,_thread_loop);
							break;
//__________________________________________________
						case 'f':
							if(numscan(++c,cc,',') != 2)
								return PARSE_SYNTAX;
							Led[atoi(cc[0])].dt=atoi(cc[1]);
							Led[atoi(cc[0])].mode=FILL;
							break;
//__________________________________________________
						case 'l':
							if(numscan(++c,cc,',') != 2)
								return PARSE_SYNTAX;
							Led[atoi(cc[0])].dt=atoi(cc[1]);
							Led[atoi(cc[0])].mode=FILL_LEFT;
							break;
//__________________________________________________
						case 'r':
							if(numscan(++c,cc,',') != 2)
								return PARSE_SYNTAX;
							Led[atoi(cc[0])].dt=atoi(cc[1]);
							Led[atoi(cc[0])].mode=FILL_RIGHT;
							break;

//__________________________________________________
						case 'c':
							if(numscan(++c,cc,',')==4) {
								Led[atoi(cc[0])].color.h =atoi(cc[1]);
								Led[atoi(cc[0])].color.s =atoi(cc[2]);
								Led[atoi(cc[0])].color.v =atoi(cc[3]);
							}	else
								return PARSE_SYNTAX;
							break;
//__________________________________________________
						case 'x':
						case '>':
							__stdin.io->arg.parse=DecodeCom;
							return(DecodeCom(NULL));
					}
				}	
				return PARSE_OK;
}

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
void _WS2812::RGB2HSV(RGB_set RGB, HSV_set *HSV){
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
void _WS2812::HSV2RGB(HSV_set HSV, RGB_set *RGB){
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
/**
* @}
*/ 
