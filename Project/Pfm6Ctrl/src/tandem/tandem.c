/**
  ******************************************************************************
  * @file    com.c
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief	 COM port parsing related functionality
  *
  */

/** @addtogroup PFM6_Application
* @{
*/

#include	"pfm.h"
#include	<ctype.h>
#include	<math.h>
#include	<string.h>
#include	<stdio.h>
#include	<ff.h>
#include	"limits.h"
//
//
typedef 	enum {__ERR, __ER,__ND,__TRG} ich;
//______________________________________________________________________________________
void			LoadSettings(PFM *p) {
FIL				f;
FATFS			fs;
UINT			bw;
					if(f_mount(&fs,"0:",1) == FR_OK && 
						f_open(&f,"0:/lm.ini",FA_READ) == FR_OK) {
						f_read(&f,p->burst,2*sizeof(burst),&bw);
						f_close(&f);
					}
}
//______________________________________________________________________________________
void			SaveSettings(PFM *p) {
FIL				f;
FATFS			fs;
UINT			bw;
					if(f_mount(&fs,"0:",1) == FR_OK && 
						f_open(&f,"0:/lm.ini",FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
						f_write(&f,p->burst,2*sizeof(burst),&bw);
						f_sync(&f);
						f_close(&f);
					}
}
//______________________________________________________________________________________
void			showCLI(PFM *p, int idx, int ch) {
int i;
					switch(ch) {
						case __ERR:
							__print("\r\n:simmer error...\r\n:");
							return;
						case __ER:							
							__print("\rEr:%4du,%4dV, n=%2d,%4du",p->Burst->Time,p->Burst->Pmax,p->Burst->N,p->Burst->Length);
							break;
						case __ND:							
							__print("\rNd:%4du,%4dV, n=%2d,%4du",p->Burst->Time,p->Burst->Pmax,p->Burst->N,p->Burst->Length);
							break;
						case __TRG:
							if(_MODE(p,_ALTERNATE_TRIGGER))							
								__print("\r  :ALTER,%4dm,%4du,%4du",p->Burst->Period,p->burst[0].Delay,p->burst[1].Delay);
							else {
								i=p->burst[1].Delay-p->burst[0].Delay;
								__print("\r  : BOTH,%4dm,%4du, --- ",p->Burst->Period,i);
							}
							break;
					}
					for(i=6*(3-idx)+1; i--; printf("\b"));
}
//______________________________________________________________________________________
void			Increment(PFM *p, int idx, int a) {
					switch(idx) {
						case 0:
							p->Burst->Time = __max(0,__min(1000,p->Burst->Time +a));
							break;
						case 1:
							p->Burst->Pmax = __max(0,__min(600,p->Burst->Pmax +a));
							break;
						case 2:
							p->Burst->N = __max(0,__min(10,p->Burst->N +a));
							break;
						case 3:
							p->Burst->Length = __max(p->Burst->Time,__min(10000,p->Burst->Length +a));
							break;
					}
}
//______________________________________________________________________________________
void			IncrementTrigger(PFM *p, int idx, int a) {
int				i;
					switch(idx) {
						case 0:
							if(a>0)
								_SET_MODE(p,_ALTERNATE_TRIGGER);
							else
								_CLEAR_MODE(p,_ALTERNATE_TRIGGER);
							break;
						case 1:
							p->burst[0].Period = p->burst[1].Period = __max(50,__min(1000,p->burst[0].Period +a));
							break;
						case 2:
							if(_MODE(p,_ALTERNATE_TRIGGER))	{
								i=p->burst[1].Period+p->burst[0].Period;
								i = __max(-500,__min(500,i  +a));
								if(i > 0) {
									p->burst[0].Period = 5;
									p->burst[1].Period = 5+i;
								} else {
									p->burst[1].Period = 5;
									p->burst[0].Period = 5-i;
								}
							} else {
								i=p->burst[1].Delay-p->burst[0].Delay;
								i = __max(-5000,__min(5000,i  +a));
								if(i > 0) {
									p->burst[0].Delay = 100;
									p->burst[1].Delay = 100+i;
								} else {
									p->burst[1].Delay = 100;
									p->burst[0].Delay = 100-i;
								}
							}
							break;
						case 3:
							break;

					}
}
//______________________________________________________________________________________
int				Tandem(PFM *p) {
ich				ch=__ERR;
int				i,idx=0;
					LoadSettings(p);

					printf("\r\n[F1]  - Er parameters");
					printf("\r\n[F2]  - Nd parameters");
					printf("\r\n[F3]  - trigger parameters");
					printf("\r\n[F11] - save settings");
					printf("\r\n[F12] - exit");
					printf("\r\n:");	

					while(1) {
						i=Escape();
						switch(i) {
							case EOF:
								if(ch != p->Simmer.active) {
									ch=__ERR;
									PFM_command(p,ch);
									showCLI(p, idx, ch);
								}
								_proc_loop();
								continue;
							case _f1: 
							case _F1:
								if(ch != __ER)
									printf("\r\n");
								ch=__ER;
								PFM_command(p,ch);
								break;								
							case _f2: 
							case _F2:
								if(ch != __ND)
									printf("\r\n");
								ch=__ND;
								PFM_command(p,ch);
								break;								
							case _f3: 
							case _F3:
								if(ch != __TRG)
									printf("\r\n");
								ch=__TRG;
								PFM_command(p,ch);
								break;								
							case _Up:
								if(ch==__TRG)
									IncrementTrigger(p, idx, 1);
								else
									Increment(p, idx, 1);
								break;
							case _Down:
								if(ch==__TRG)
									IncrementTrigger(p, idx, -1);
								else
									Increment(p, idx, -1);
								break;
							case _Left:
								if(idx)	
									--idx;
								break;
							case _Right:
								if(idx < 3)	
									++idx;
								break;
							case _f11: 
							case _F11:
								SaveSettings(p);
								printf("\r\n: saved...\r\n:");
								break;								
							case _f12: 
							case _F12:
								ch=__ERR;
								PFM_command(p,ch);
								printf("\r\n: bye...\r\n>");
								return _PARSE_OK;							
					}
					showCLI(p, idx, ch);
				}
}
