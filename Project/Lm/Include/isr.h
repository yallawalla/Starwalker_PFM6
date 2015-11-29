#ifndef					ISR_H
#define					ISR_H
#include				"stm32f2xx.h"
#include				<stdio.h>

extern "C" {
extern					volatile int __time__;
	
void						PrintVersion(int);
void						_led(int, int),
								_lightshow(void);
void						Watchdog(void);

#define					_RED1(a)			_led(0,a)
#define					_GREEN1(a)		_led(1,a)
#define					_YELLOW1(a)		_led(2,a)
#define					_BLUE1(a)			_led(3,a)
#define					_ORANGE1(a)		_led(4,a)
#define					_RED2(a)			_led(5,a)
#define					_GREEN2(a)		_led(6,a)
#define					_YELLOW2(a)		_led(7,a)
#define					_BLUE2(a)			_led(8,a)
#define					_ORANGE2(a)		_led(9,a)

#define	 				__Esc					0x1b

#define					__CtrlA				0x01
#define					__CtrlB				0x02
#define					__CtrlC				0x03
#define					__CtrlD				0x04
#define					__CtrlE				0x05

#define					__CtrlI				0x09
#define					__CtrlO				0x0f
#define					__CtrlV				0x16
#define					__CtrlZ				0x1a
#define					__CtrlY				0x19

#define	 				__f1					0x001B4F50
#define	 				__f2					0x001B4F51
#define	 				__f3					0x001B4F52
#define	 				__f4					0x001B4F53
#define	 				__f5					0x001B4F54
#define	 				__f6					0x001B4F55
#define	 				__f7					0x001B4F56
#define	 				__f8					0x001B4F57
#define	 				__f9					0x001B4F58
#define	 				__f10					0x001B4F59
#define	 				__f11					0x001B4F5A
#define	 				__f12					0x001B4F5B

#define	 				__F1					0x5B31317E
#define	 				__F2 					0x5B31327E
#define	 				__F3					0x5B31337E
#define	 				__F4					0x5B31347E
#define	 				__F5					0x5B31357E
#define	 				__F6					0x5B31377E
#define	 				__F7					0x5B31387E
#define	 				__F8					0x5B31397E
#define	 				__F9					0x5B32307E
#define	 				__F10					0x5B32317E
#define	 				__F11					0x5B32337E
#define	 				__F12					0x5B32347E
#define	 				__Home				0x1B5B317E
#define	 				__End					0x1B5B347E
#define	 				__Insert			0x1B5B327E
#define	 				__PageUp			0x1B5B357E
#define	 				__Delete			0x1B5B337E
#define	 				__PageDown		0x1B5B367E
#define	 				__Up					0x001B5B41
#define	 				__Left				0x001B5B44
#define	 				__Down				0x001B5B42
#define	 				__Right				0x001B5B43

#define	 				__FOOT_OFF		0x0000f800
#define	 				__FOOT_IDLE		0x00003800
#define	 				__FOOT_MID		0x0000b800
#define	 				__FOOT_ON			0x0000d800
								
typedef struct _buffer
{
	char	*_buf, *_push, *_pull;
	int		(* push)(struct _buffer *, char *);
	int		(* pull)(struct _buffer *);
	int		len;
} _buffer;	

typedef struct _io
{
_buffer	*rx,
				*tx,
				*cmdline;
int			(*get)(_buffer *),
				(*put)(int, _buffer *);
int			(*parse)(char *);
} _io;

_buffer	*_buffer_init(int),
				*_buffer_close(_buffer *);
int			_buffer_len(_buffer *);
	
_io			*_io_init(int, int),
				*_io_close(_io *),
				*_stdio(_io	*);

int			_buffer_push(_buffer *, void *,int),
				_buffer_pull(_buffer *, void *,int),
				_buffer_len(_buffer *),
				_buffer_LIFO(_buffer *, void *, int);
				
void		_thread_init(void),
				_thread_loop(void),
				_thread_list(void),
				_thread_add(void *,void *,char *,int),
				_thread_remove(void *,void *),	
				*_thread_active(void *,void *);
void		_wait(int,void (*)(void));
_io			*ParseCom(_io *);
	
}


#ifndef	__max				
#define __max(a,b)  (((a) > (b)) ? (a) : (b))	
#endif
#ifndef	__min				
#define __min(a,b)  (((a) < (b)) ? (a) : (b))	
#endif
#define	__minmax(x,x1,x2,y1,y2)	__min(__max(((y2-y1)*(x-x1))/(x2-x1)+y1,y1),y2)



#endif
