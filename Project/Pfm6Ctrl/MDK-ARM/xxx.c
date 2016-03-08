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
int				i,j,k;
struct 		_dma	*p;
	
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
										--rgb[i].color_buffer[j].c[3];
									else if(rgb[i].color_buffer[j].c[2] < rgb[i].color.c[2]) {
										++rgb[i].color_buffer[j].c[3];
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
