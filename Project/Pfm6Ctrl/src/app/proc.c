#include "proc.h"
_buffer 	*_thread_buf=NULL;
//______________________________________________________________________________
_proc		*_proc_add(void *f,void *arg,char *name, int dt) {
_proc		*p=malloc(sizeof(_proc));
					if(p != NULL) {
						p->f=(func *)f;
						p->arg=arg;
						p->name=name;
						p->dt=dt;
						p->t=__time__+dt;
						if(!_thread_buf)
							_thread_buf=_buffer_init(_THREAD_BUFFER_SIZE*sizeof(_proc *));
						_buffer_push(_thread_buf,&p,sizeof(_proc *));
					}
					return p;
}
//______________________________________________________________________________
void			_proc_loop(void) {
_proc			*p;
					if(_thread_buf)
						if(_buffer_pull(_thread_buf,&p,sizeof(_proc *))) {
							if(__time__ >= p->t) {
								p->to = __time__ - p->t;
								p->f(p->arg);
								p->t = __time__ + p->dt;
							}
							_buffer_push(_thread_buf,&p,sizeof(_proc *));
						}
}
//______________________________________________________________________________
void			_proc_remove(void  *f,void *arg) {
int				i=_buffer_count(_thread_buf)/sizeof(_proc *);
_proc		*p;
					while(i--) {
						_buffer_pull(_thread_buf,&p,sizeof(_proc *));
						if(f == p->f && arg == p->arg)
							free(p);
						else
							_buffer_push(_thread_buf,&p,sizeof(_proc *));
					}
}
//______________________________________________________________________________
_proc		*_proc_find(void  *f,void *arg) {
int				i=_buffer_count(_thread_buf)/sizeof(_proc *);
_proc		*p,*q=NULL;
					while(i--) {
						_buffer_pull(_thread_buf,&p,sizeof(_proc *));
						if(f == p->f && (arg == p->arg || !arg))
							q=p;
						_buffer_push(_thread_buf,&p,sizeof(_proc *));
					}
					return q;
}
//______________________________________________________________________________
void			_proc_list(void) {
int i			=_buffer_count(_thread_buf)/sizeof(_proc *);
_proc		*p;	
					printf("...\r\n");
					while(i--) {
						_buffer_pull(_thread_buf,&p,sizeof(_proc *));
						printf("%08X,%08X,%s,%d\r\n",(int)p->f,(int)p->arg,p->name,p->to);
						_buffer_push(_thread_buf,&p,sizeof(_proc *));
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
