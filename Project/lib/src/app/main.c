/**
  ******************************************************************************
  * @file			main.c
  * @author		Fotona d.d.
  * @version	V1
  * @date			30-Sept-2013
  * @brief   	Initialization and main app. loop wrapper file
  *	
  */
/** @addtogroup PFM6_Application
* @brief PFM6 application group
* @{
*/
#include	"app.h"
#include	"math.h"
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
int				main(void) { 
					_thread_init();
					while(1)
						_thread_loop();
} 




#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>


#ifdef __cplusplus    // If used by C++ code,

extern "C" {          // we need to export the C interface
#endif
               #ifdef UNIX
                       #define EXPORT extern
               #elif (defined (_WINDOWS))
                       #define EXPORT extern __declspec( dllexport )
               #endif

// gcc -c -Wall -Werror -fpic ./canwr.c -DUNIX
// gcc -shared -o libcanwr.so canwr.o

EXPORT int mainnn(void)
{
        int s;
        int nbytes;
        struct sockaddr_can addr;
        struct can_frame frame;
        struct ifreq ifr;

        char *ifname = "can0";

        if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
                perror("Error while opening socket");
                return -1;
        }

        strcpy(ifr.ifr_name, ifname);
        ioctl(s, SIOCGIFINDEX, &ifr);
        addr.can_family  = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

        if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("Error in socket bind");
                return -2;
        }

        frame.can_id  = 0x123;
        frame.can_dlc = 2;
        frame.data[0] = 0x11;
        frame.data[1] = 0x22;


/** 
* @}
*/
