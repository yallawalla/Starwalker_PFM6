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

#define		setSimmer(a,b)	simm=b;	PFM_command(a,b);

static		enum 	{_ER,_ND,_STANDBY,_READY,_LASER} state=_STANDBY;
static		enum	{_OFF,_SIMM1,_SIMM2,_SIMM_ALL} simm=_OFF;
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
					switch(state) {
						case _ER:							
							__print("\rEr     : %4du,%4dV, n=%2d,%4du",p->Burst->Time,p->Burst->Pmax,p->Burst->N,p->Burst->Length);
							break;
						case _ND:							
							__print("\rNd     : %4du,%4dV, n=%2d,%4du",p->Burst->Time,p->Burst->Pmax,p->Burst->N,p->Burst->Length);
							break;
						case _STANDBY:
						case _READY:				
						case _LASER:				
							if(_MODE(p,_ALTERNATE_TRIGGER))	{
								int per=p->burst[0].Period+p->burst[1].Period;	
								__print("\rtrigger: ALTER,%4dm,%4dm",per,per-p->Burst->Period);
							} else {
								__print("\rtrigger:  BOTH,%4dm,%4du",p->Burst->Period,p->burst[1].Delay-p->burst[0].Delay);
							}
							if(state==_STANDBY)
								__print(",STNBY");
							if(state==_READY)
								__print(",READY");
							if(state==_LASER) {
								__print(",LASER\r\n");
								return;
							}
							break;
					}
					for(i=6*(3-idx)+1; i--; __print("\b"));
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
					switch(idx) {
						case 0:
							if(a > 0)
								_SET_MODE(p,_ALTERNATE_TRIGGER);
							else
								_CLEAR_MODE(p,_ALTERNATE_TRIGGER);
							break;
						case 1:
							p->burst[1].Period += 10*a;
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
								p->burst[1].Delay +=5*a;
								p->burst[0].Delay -=5*a;
								if(p->burst[0].Delay < 100 || p->burst[1].Delay < 100) {
									p->burst[0].Delay +=5*a;
									p->burst[1].Delay +=5*a;
								}
							}
							break;
						case 3:
							state = __max(_STANDBY,__min(_LASER, state + a));
							switch(state) {
								case _STANDBY:
									setSimmer(p,_OFF);	
									break;
								case _READY:
									if(p->Trigger.time)
										_SET_EVENT(pfm,_TRIGGER);		
									else {
										setSimmer(p,_SIMM1);	
										SetPwmTab(p);
										setSimmer(p,_SIMM2);	
										SetPwmTab(p);
										setSimmer(p,_SIMM_ALL);	
									}
									break;
								case _LASER:
								_SET_EVENT(pfm,_TRIGGER);	
								break;
							default:
								break;
							}
							return;
					}
					state=_STANDBY;
					setSimmer(p,_OFF);	
}
//______________________________________________________________________________________
int				Tandem(PFM *pfm) {
int				i;
					p=pfm;
					LoadSettings(p);

					__print("\r\n[F1]  - Er parameters");
					__print("\r\n[F2]  - Nd parameters");
					__print("\r\n[F3]  - trigger parameters");
					__print("\r\n[F11] - save settings");
					__print("\r\n[F12] - exit");
					__print("\r\n:");	
	
					state=_STANDBY;
					setSimmer(p,simm);
					_SET_MODE(p,_TRIGGER_PERIODIC);
	
	
					while(1) {
						i=Escape();
						switch(i) {
							case EOF:
								if(simm != p->Simmer.active) {
									__print("\r\n:simmer error...\r\n:");
									state=_STANDBY;
									setSimmer(p,_OFF);
								}
								_proc_loop();
								continue;
							case _f1: 
							case _F1:
								if(state != _ER)
									__print("\r\n");
								state=_ER;
								setSimmer(p,_SIMM1);
								break;								
							case _f2: 
							case _F2:
								if(state != _ND)
									__print("\r\n");
								state=_ND;
								setSimmer(p,_SIMM2);
								break;								
							case _f3: 
							case _F3:
								if(state != _STANDBY)
									__print("\r\n");
								state=_STANDBY;
								setSimmer(p,_OFF);
								break;								
							case _Up:
								if(state==_STANDBY || state==_READY || state==_LASER) 
									IncrementTrg(1);
								else
									IncrementPar(1);
								break;
							case _Down:
								if(state==_STANDBY || state==_READY || state==_LASER) 
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
								__print("\r\n: saved...\r\n:");
								break;								
							case _f12: 
							case _F12:
								state=_STANDBY;
								setSimmer(p,_OFF);
								_CLEAR_MODE(p,_TRIGGER_PERIODIC);
								__print("\r\n: bye...\r\n>");
								return _PARSE_OK;	
							default:
								_proc_loop();
								continue;
					}
					showCLI();
				}
}
