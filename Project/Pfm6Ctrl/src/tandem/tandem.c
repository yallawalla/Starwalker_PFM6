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
typedef 	enum {__ER=1,__ND=2,__TRG=3} incrch;
//
void			CmdLine(int ch, int idx) {
int i;
					switch(ch) {
						case __ER:							
							__print("\rEr:%4du,%4dV, n=%2d,%4du",pfm->Burst->Time,pfm->Burst->Pmax,pfm->Burst->N,pfm->Burst->Length);
							break;
						case __ND:							
							__print("\rNd:%4du,%4dV, n=%2d,%4du",pfm->Burst->Time,pfm->Burst->Pmax,pfm->Burst->N,pfm->Burst->Length);
							break;
						case __TRG:							
							__print("\r  :%4d ,%4d ,  %3d,%4d ",0,0,0,0);
							break;
					}
					for(i=6*(3-idx)+1; i--; printf("\b"));
}
void			Increment(int idx, int a) {
					switch(idx) {
						case 0:
							pfm->Burst->Time = __max(0,__min(1000,pfm->Burst->Time +a));
							break;
						case 1:
							pfm->Burst->Pmax = __max(0,__min(600,pfm->Burst->Pmax +a));
							break;
						case 2:
							pfm->Burst->N = __max(0,__min(10,pfm->Burst->N +a));
							break;
						case 3:
							pfm->Burst->Length = __max(pfm->Burst->Time,__min(10000,pfm->Burst->Length +a));
							break;
					}
}
//______________________________________________________________________________________
int				DecodeIncr(void) {
incrch		ch=__ER;
int				i,idx=0;

					PFM_command(pfm,ch);
					CmdLine(ch, idx);

					while(1) {
						i=Escape();
						switch(i) {
							case EOF:
								_proc_loop();
								continue;
							case _f1: case _F1:
								if(ch != __ER)
									printf("\r\n");
								ch=__ER;
								PFM_command(pfm,ch);
								break;								
							case _f2: case _F2:
								if(ch != __ND)
									printf("\r\n");
								ch=__ND;
								PFM_command(pfm,ch);
								break;								
							case _f3: case _F3:
								if(ch != __TRG)
									printf("\r\n");
								ch=__TRG;
								PFM_command(pfm,ch);
								break;								
							case _Up:
								Increment(idx, 	1);
								break;
							case _Down:
								Increment(idx, -1);								
								break;
							case _Left:
								if(idx)	
									--idx;
								break;
							case _Right:
								if(idx < 3)	
									++idx;
								break;
							case _f11: case _F11:
								break;								
							case _f12: case _F12:
								printf("\r\n>");
								return _PARSE_OK;							
					}
					CmdLine(ch, idx);
				}
}
