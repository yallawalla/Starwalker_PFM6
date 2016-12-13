
/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2012 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <rt_misc.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f2xx.h"
#pragma		import(__use_no_semihosting_swi)

#include "ff.h"
#include "io.h"
//_________________________________________________________________________________
FILE 		__stdout;
FILE 		__stdin;
FILE 		__stderr;
//_________________________________________________________________________________
_io			*_stdio(_io	*p) {
_io			*io=__stdin.handle.io;
				__stdin.handle.io=__stdout.handle.io=p;
				return(io);
}
//_________________________________________________________________________________
extern	_io	*__com0,*__com1,*__can;
//_________________________________________________________________________________
int 		fputc(int c, FILE *f) {
				int n;
				if(f==&__stdout) {
_io				*io=__stdout.handle.io;
					if(io == __com0 || io == __com1 || io == __can) {
						while(io->put(io->tx,c) == EOF)
							Watchdog();	
						return	c;
					} else 
						return fputc(c,f->handle.file);
				}
				if(!f || f_write((FIL *)f, &c, 1, (UINT *)&n)!=FR_OK || n!=1)
					return(EOF);
				else
					return(c);
}
//_________________________________________________________________________________
int 		fgetc(FILE *f) {
				int n,c=0;
				if(f==&__stdin) {
_io				*io=__stdin.handle.io;
					if(io == __com0 || io == __com1 || io == __can)
						return io->get(io->rx);
					else
						return fgetc(f->handle.file);
				}
				if(f_read((FIL *)f, &c, 1, (UINT *)&n)==FR_OK && n==1)
					return(c);
				else
					return(EOF);
}
//_________________________________________________________________________________
int 		fclose(FILE* f) 
{	
				return((int)f_close(f->handle.fil));
}
//_________________________________________________________________________________
FILE 		*fopen(const char *filename, const char *att) {
				FIL *f=malloc(sizeof(FIL));
				if(!f)
					return(NULL);
				if(strchr(att,'+')) {
					if(strchr(att,'r'))	
						if(f_open(f,filename,FA_READ | FA_WRITE)==FR_OK)
							return((FILE *)f);				
					if(strchr(att,'w'))	
						if(f_open(f,filename,FA_READ | FA_WRITE | FA_CREATE_ALWAYS)==FR_OK)
							return((FILE *)f);										
				} else {	
					if(strchr(att,'r'))
						if(f_open(f,filename,FA_READ)==FR_OK)
							return((FILE *)f);
					if(strchr(att,'w'))
						if(f_open(f,filename,FA_WRITE | FA_CREATE_ALWAYS)==FR_OK)
							return((FILE *)f);
				}
				free(f);
				return(NULL);
}
//_________________________________________________________________________________
int 		fseek (FILE *f, long nPos, int nMode)  {
				switch(nMode) {
					case SEEK_SET:
						return(f_lseek(f->handle.fil,nPos));
					case SEEK_CUR:
						return(f_lseek(f->handle.fil, f_tell(f->handle.fil)+nPos));
					case SEEK_END:
						return(f_lseek(f->handle.fil, f_size(f->handle.fil)-nPos));
					default:
						return EOF;
				}
}
//_________________________________________________________________________________
int 		fflush (FILE *f)  {
				return	f_sync(f->handle.fil);
}
//_________________________________________________________________________________
int 		ferror(FILE *f) {
				return	f_error(f->handle.fil);
}
//_________________________________________________________________________________
void 		_ttywrch(int c) {
				fputc(c,&__stdout);
}
//_________________________________________________________________________________
void		_sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
