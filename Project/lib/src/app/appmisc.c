#include	"app.h"
/**
  ******************************************************************************
  * @file    appmisc.c
  * @author  Fotona d.d.
  * @version V1
  * @date    30-Sept-2013
  * @brief	 Application support 
  *
  */
/** @addtogroup PFM6_Application
* @{
*/
#define 	_THREAD_BUFFER_SIZE 128

_buffer 	*_thread_buf=NULL;
 typedef	void *func(void *);
 
typedef	struct {
func			*f;
void			*arg;
char			*name;
int				t,dt,to;
} _thread;
//______________________________________________________________________________
void			_thread_add(void *f,void *arg,char *name, int dt) {
_thread		*p=malloc(sizeof(_thread));
					if(p != NULL) {
						p->f=(func *)f;
						p->arg=arg;
						p->name=name;
						p->t=__time__;
						p->dt=dt;
						if(!_thread_buf)
							_thread_buf=_buffer_init(_THREAD_BUFFER_SIZE*sizeof(_thread *));
						_buffer_push(_thread_buf,&p,sizeof(_thread *));
					}
}
//______________________________________________________________________________
void			_thread_loop(void) {
_thread		*p;
					if(_thread_buf)
						if(_buffer_pull(_thread_buf,&p,sizeof(_thread *))) {
							if(__time__ >= p->t) {
								p->to = __time__ - p->t;
								p->f(p->arg);
								p->t = __time__ + p->dt;
							}
							_buffer_push(_thread_buf,&p,sizeof(_thread *));
						}
}
//______________________________________________________________________________
void			_thread_remove(void  *f,void *arg) {
int				i=_buffer_len(_thread_buf)/sizeof(_thread *);
_thread		*p;
					while(i--) {
						_buffer_pull(_thread_buf,&p,sizeof(_thread *));
						if(f == p->f && arg == p->arg)
							free(p);
						else
							_buffer_push(_thread_buf,&p,sizeof(_thread *));
					}
}
//______________________________________________________________________________
_thread		*_thread_active(void  *f,void *arg) {
int				i=_buffer_len(_thread_buf)/sizeof(_thread *);
_thread		*p,*q=NULL;
					while(i--) {
						_buffer_pull(_thread_buf,&p,sizeof(_thread *));
						if(f == p->f && arg == p->arg)
							q=p;
						_buffer_push(_thread_buf,&p,sizeof(_thread *));
					}
					return q;
}
//______________________________________________________________________________
void			_thread_list(void) {
int i			=_buffer_len(_thread_buf)/sizeof(_thread *);
_thread		*p;	
					printf("...\r\n");
					while(i--) {
						_buffer_pull(_thread_buf,&p,sizeof(_thread *));
						printf("%08X,%08X,%s,%d\r\n",(int)p->f,(int)p->arg,p->name,p->to);
						_buffer_push(_thread_buf,&p,sizeof(_thread *));
					}
}
//___________________________________________________________________________
void			batch(char *filename) {
FIL				f;
					if(f_open(&f,filename,FA_READ)==FR_OK) {
						__stdin.FIL=&f;
						do
							_thread_loop();
							while(!f_eof(&f));
						__stdin.FIL=NULL;
						f_close(&f);
					}
}
//___________________________________________________________________________
void			_wait(int t,void (*f)(void)) {
int				to=__time__+t;
					while(to > __time__) {
						if(f)
							f();
					}
}
//___________________________________________________________________________
void			PrintVersion(int v) {
					RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
					CRC_ResetDR();
					printf(" %d.%02d %s, <%08X>",
						v/100,v%100,
						__DATE__,
							CRC_CalcBlockCRC(__Vectors, (FATFS_ADDRESS-(int)__Vectors)/sizeof(int)));
}
/**
* @}
*/
			

