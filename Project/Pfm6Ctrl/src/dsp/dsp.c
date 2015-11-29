#include "stm32f2xx.h"      
#include <stdio.h>      

#include "stm32f4_discovery_lcd.h"

extern volatile int __time__;
void	DrawGrid() {
int		i;
			LCD_Clear(LCD_COLOR_BLACK);
			LCD_SetTextColor(0x1ce7);
			for(i=0; i<LCD_PIXEL_WIDTH; i+=50)
				LCD_DrawLine(i,0,LCD_PIXEL_WIDTH,LCD_DIR_VERTICAL);
			for(i=0; i<LCD_PIXEL_HEIGHT/2; i+=50) {
				LCD_DrawLine(0,LCD_PIXEL_HEIGHT/2+i,LCD_PIXEL_WIDTH,LCD_DIR_HORIZONTAL);
				LCD_DrawLine(0,LCD_PIXEL_HEIGHT/2-i,LCD_PIXEL_WIDTH,LCD_DIR_HORIZONTAL);
			}
}
/**
* @}
*/ 
void	LcdInit() {
			STM32f4_Discovery_LCD_Init();
			LCD_SetBackColor(LCD_COLOR_BLACK);
			LCD_SetTextColor(LCD_COLOR_YELLOW);
			LCD_SetFont(&Font16x24);
			DrawGrid();
}
//*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/

/* ----------------------------------------------------------------------
* Copyright (C) 2010-2012 ARM Limited. All rights reserved.
*
* $Date:         17. January 2013
* $Revision:     V1.4.0
*
* Project:       CMSIS DSP Library
* Title:         arm_graphic_equalizer_example_q31.c
*
* Description:   Example showing an audio graphic equalizer constructed
*                out of Biquad filters.
*
* Target Processor: Cortex-M4/Cortex-M3
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of ARM LIMITED nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------- */
#include "arm_math.h"
/* Length of the overall data in the test */
#define TESTLENGTH 320
/* Block size for the underlying processing */
#define BLOCKSIZE 32
/* Total number of blocks to run */
#define NUMBLOCKS (TESTLENGTH/BLOCKSIZE)
/* Number of 2nd order Biquad stages per filter */
#define NUMSTAGES 2
#define SNR_THRESHOLD_F32  98
/* -------------------------------------------------------------------
 * External Declarations for Input and Output buffers
 * ------------------------------------------------------------------- */
extern void			(*App_Loop)(void),__App_Loop(void);
void						Wait(int,void (*)(void));

/* ----------------------------------------------------------------------
 * Graphic equalizer Example
 * ------------------------------------------------------------------- */
void dsp(void)
{
static float32_t coeff[]={
0.002295154427120992,0.004590308854241984,0.002295154427120992,1.8973814990203015,-0.9065621167287853
};

static float32_t  pstate[8*10];
				
  volatile int32_t iCount; 
  float32_t  in[BLOCKSIZE], out[BLOCKSIZE];
  arm_biquad_casd_df1_inst_f32 S;
  int		i,j,k,l;
	int		a,b;
	float	c,d;
	char	s[64];
	LcdInit();
  arm_biquad_cascade_df1_init_f32 (&S, 1, coeff, pstate);

	j=l=0;
	while(1) {
		k=0;
		do {
			i=getchar();
			switch(i) {
				case 0x0d:	s[k]=0;
				case -1:		break;
				case 0x1b:	return;
				default:		s[k++]=i;
			}
		} while(i!=0x0d);
		
		if(sscanf(s,"%d,%d,%f,%f",&a,&b,&c,&d)==4) {
			in[j++]=b - 64000;
			if(j==BLOCKSIZE) {
				j=0;
				arm_biquad_cascade_df1_f32(&S, &in[0], &out[0], BLOCKSIZE);

				for(k=0; k < BLOCKSIZE; k++,l++) {
					if(l > LCD_PIXEL_WIDTH) {
						l=0;
						DrawGrid();
					}
					LCD_SetTextColor(LCD_COLOR_YELLOW);
					LCD_DrawCircle(l,LCD_PIXEL_HEIGHT/2+100-out[k]/10,1);
					LCD_SetTextColor(LCD_COLOR_GREEN);
					LCD_DrawCircle(l,LCD_PIXEL_HEIGHT/2+100-in[k]/10,1);
				}
			}
		}
	}
}



