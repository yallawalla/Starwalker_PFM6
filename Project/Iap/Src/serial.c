#include	"iap.h"
//______________________________________________________________________________________
//
// zamenjava za gets, ne èaka, vraèa pointer na string brez \r(!!!) ali NULL	
// èe je mode ECHO (-1) na
//							<cr> izpiše <cr><lf>
//							<backspace> ali <del> izpiše <backspace><space><backspace>	
//
//______________________________________________________________________________________
char	*cgets(int c, int mode)
{
_buffer		*p=__stdin.io->gets;
			
			if(!p)
				p=__stdin.io->gets=_buffer_init(__stdin.io->rx->len);
			switch(c) {
				case EOF:		
					break;
				case '\r':
				case '\n':
					*p->_push = '\0';
					p->_push=p->_pull=p->_buf;
					return(p->_buf);
				case 0x08:
				case 0x7F:
					if(p->_push != p->_pull) {
						--p->_push;
						if(mode)
							printf("\b \b");
					}
					break;
				default:
					if(p->_push != &p->_buf[p->len-1])
						*p->_push++ = c;
					else  {
						*p->_push=c;
						if(mode)
							printf("\b");
					}
					if(mode) {
						if(isprint(c))
							printf("%c",c);
						else
							printf("%c%02X%c",'<',c,'>');
					}
					break;
			}
			return(NULL);
}
//______________________________________________________________________________________
void	ParseCOM(void) {
int		i,j;
char	*p;

			switch(getchar()) {			
				case _CtrlY:
					NVIC_SystemReset();
				case _CtrlZ:
					while(1);
				case '1':
#ifdef	__PVC__
					for(j=0; j<32; ++j) {
#endif
#ifdef	STM32F2XX
					for(j=0; j<5; ++j) {
#endif
						CanHexMessage(_ID_IAP_ERASE,_SIGN_PAGE+j*_PAGE_SIZE);
						for(i=0;i<1000;++i)	
							App_Loop();
					}
					break;
				case '2':
					CanHexMessage(_ID_IAP_SIGN,0);
					break;
				case '3':
					CanHexMessage(_ID_IAP_GO,0);
					break;
				case '?':
					p=(char *)_FW_START;
					for(i=0;i<8;++i)
						printf("\r\n%08X",*(int *)p++);
					break;
				case 0x1b:
					if(!(__CAN__->BTR & ((CAN_Mode_LoopBack)<<30)))
						Initialize_CAN(1);
					CanHexMessage(_ID_IAP_PING,0);
					break;
				case ':':
					do p=cgets(getchar(),0); while(!p);
					strcpy(_Iap_string,p);
					CanHexProg(NULL);
					break;
				default:
					break;
			}
}
//___________________________________________________________________________
extern	volatile int __time__;
void		Wait(int t,void (*f)(void)) {
int			to=__time__+t;
				while(to > __time__)
					if(f)
						f();
}
