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


static		enum 	{__OFF,__ER,__ND,__TRG} ich=__OFF;
static		int		idx;
static		PFM		*p=NULL;
//______________________________________________________________________________________
static
	void		LoadSettings(PFM *p) {
FIL				f;
FATFS			fs;
UINT			bw;
					if(f_mount(&fs,"0:",1) == FR_OK && 
						f_open(&f,"0:/tandem.ini",FA_READ) == FR_OK) {
						f_read(&f,p->burst,2*sizeof(burst),&bw);
						f_close(&f);
					}
}
//______________________________________________________________________________________
static
	void		SaveSettings() {
FIL				f;
FATFS			fs;
UINT			bw;
					if(f_mount(&fs,"0:",1) == FR_OK && 
						f_open(&f,"0:/tandem.ini",FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
						f_write(&f,p->burst,2*sizeof(burst),&bw);
						f_sync(&f);
						f_close(&f);
					}
}
//______________________________________________________________________________________
static 
	void		showCLI() {
int				i;
					switch(ich) {
						case __ER:							
							__print("\rEr     : %4du,%4dV, n=%2d,%4du",p->Burst->Time,p->Burst->Pmax,p->Burst->N,p->Burst->Length);
							break;
						case __ND:							
							__print("\rNd     : %4du,%4dV, n=%2d,%4du",p->Burst->Time,p->Burst->Pmax,p->Burst->N,p->Burst->Length);
							break;
						case __OFF:
						case __TRG:				
							if(_MODE(p,_ALTERNATE_TRIGGER))	{
								int per=p->burst[0].Period+p->burst[1].Period;	
								__print("\rtrigger: ALTER,%4dm,%4dm",per,per-p->Burst->Period);
							} else {
								__print("\rtrigger:  BOTH,%4dm,%4du",p->Burst->Period,p->burst[1].Delay-p->burst[0].Delay);
							}
							
							if(p->Simmer.active)
								__print(", _-_ ");
							else
								__print(", ___ ");
							break;
					}
					for(i=6*(3-idx)+1; i--; printf("\b"));
}
//______________________________________________________________________________________
static 
	void		IncrementPar(int a) {
					switch(idx) {
						case 0:
							p->Burst->Time = __max(0,__min(1000,p->Burst->Time +10*a));
							break;
						case 1:
							p->Burst->Pmax = __max(0,__min(600,p->Burst->Pmax +a));
							break;
						case 2:
							p->Burst->N = __max(0,__min(10,p->Burst->N +a));
							break;
						case 3:
							p->Burst->Length = __max(p->Burst->Time,__min(10000,p->Burst->Length +100*a));
							break;
					}
}
//______________________________________________________________________________________
static 
	void		IncrementTrg(int a) {
					a*=10;
					switch(idx) {
						case 0:
							if(a > 0)
								_SET_MODE(p,_ALTERNATE_TRIGGER);
							else
								_CLEAR_MODE(p,_ALTERNATE_TRIGGER);
							break;
						case 1:
							p->burst[1].Period += a;
							if(!_MODE(p,_ALTERNATE_TRIGGER))
								p->burst[0].Period=p->burst[1].Period;
							break;
						case 2:
							if(_MODE(p,_ALTERNATE_TRIGGER)) {
								p->burst[0].Period +=a;
								p->burst[1].Period -=a;
								if(p->burst[0].Period < 5 || p->burst[1].Period < 5) {
									p->burst[0].Period +=a;
									p->burst[1].Period +=a;
								}
							} else {
								p->burst[1].Delay +=a;
								p->burst[0].Delay -=a;
								if(p->burst[0].Delay < 100 || p->burst[1].Delay < 100) {
									p->burst[0].Delay +=a;
									p->burst[1].Delay +=a;
								}
							}
							break;
						case 3:
							if(ich==__OFF && a > 0) {
								ich=__ER;
								PFM_command(p,ich);	
								SetPwmTab(p);
								
								ich=__ND;
								PFM_command(p,ich);	
								SetPwmTab(p);
								
								ich=__TRG;
								PFM_command(p,ich);	
								return;
							} 
							break;
					}
					ich=__OFF;
					PFM_command(p,ich);	
}
//______________________________________________________________________________________
int				Tandem(PFM *pfm) {
int				i;
					p=pfm;
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
								if(ich != p->Simmer.active) {
									__print("\r\n:simmer error...\r\n:");
									ich=__OFF;
									PFM_command(p,ich);
								}
								_proc_loop();
								continue;
							case _Esc:		
								if(ich==__TRG)
									_SET_EVENT(pfm,_TRIGGER);	
								break;
							case _f1: 
							case _F1:
								if(ich != __ER)
									printf("\r\n");
								ich=__ER;
								PFM_command(p,ich);
								break;								
							case _f2: 
							case _F2:
								if(ich != __ND)
									printf("\r\n");
								ich=__ND;
								PFM_command(p,ich);
								break;								
							case _f3: 
							case _F3:
								if(ich != __OFF)
									printf("\r\n");
								ich=__OFF;
								PFM_command(p,ich);
								break;								
							case _Up:
								if(ich==__OFF || ich==__TRG)
									IncrementTrg(1);
								else
									IncrementPar(1);
								break;
							case _Down:
								if(ich==__OFF || ich==__TRG)
									IncrementTrg(-1);
								else
									IncrementPar(-1);
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
								SaveSettings();
								printf("\r\n: saved...\r\n:");
								break;								
							case _f12: 
							case _F12:
								ich=__OFF;
								PFM_command(p,ich);
								printf("\r\n: bye...\r\n>");
								return _PARSE_OK;	
							default:
								_proc_loop();
								continue;
					}
					showCLI();
				}
}
