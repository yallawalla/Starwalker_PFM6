#include	"pfm.h"
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
app list[20] = {
	{(func *)ParseCom,					(arg *)&__com0,0,0,0,"ParseCOM"},
	{(func *)ParseCom,					(arg *)&__com1,0,0,0,"ParseUSB"},
	{(func *)ParseCanTx,				(arg *)&pfm,0,0,0,"txCAN"},
	{(func *)ParseCanRx,				(arg *)&pfm,0,0,0,"rxCAN"},
	{(func *)ProcessingEvents,	(arg *)&pfm,0,0,0,"events"},
	{(func *)ProcessingStatus,	(arg *)&pfm,0,1,0,"status"},
	{(func *)ProcessingCharger,	(arg *)&pfm,0,1,0,"charger6"},
	{(func *)USBHost,						(arg *)NULL,0,0,0,"host USB"},
	{(func *)Watchdog,					(arg *)NULL,0,0,0,"Watchdog"},
	{(func *)Lightshow,					(arg *)NULL,0,0,0,"Leds"}
};
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void	App_Loop(void) {
static 
	int	current_job=-1;
int		i=current_job;
			if(++current_job >= sizeof(list)/sizeof(app))
				current_job=0;				
			if(list[i].f && list[i].dt >= 0 && __time__ >= list[i].t) {
				int		dt=list[i].dt;
				list[i].dt = -1;
				list[i].to=__time__;
				list[i].f(*list[i].arg);
				list[i].to=__time__-list[i].to;
				list[i].dt=dt;
				list[i].t = __time__ + dt;
			}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
app		*App_Find(func *f,arg *a) {
int		i;
			for(i=0; i<sizeof(list)/sizeof(app); ++i)
				if(list[i].f == f && list[i].arg == a) 
					return &list[i];
			return NULL;
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void	App_Add(func *f,arg *a,char *name, int dt) {
int		i;
			for(i=0; i<sizeof(list)/sizeof(app); ++i)
				if(list[i].f == NULL) {
						list[i].to=0;
						list[i].t=__time__+dt;
						list[i].f=f;
						list[i].arg=a;
						list[i].name=name;
						list[i].dt=dt;
						return;
				}
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void	App_Remove(func *f,arg *a) {
int		i;
			for(i=0; i<sizeof(list)/sizeof(app); ++i)
				if(list[i].f == f && list[i].arg == a)
					list[i].f=NULL;
}
/*******************************************************************************
* Function Name	: 
* Description		: 
* Output				:
* Return				:
*******************************************************************************/
void	App_List(void) {
int		i;
			for(i=0; i<sizeof(list)/sizeof(app); ++i) {
				if(list[i].f == 0)
					continue;
				if(list[i].dt >= 0)
					__print("\r\n%08X,%08X,%s,%d",(int)list[i].f,(int)list[i].arg,list[i].name,list[i].to);
				else
					__print("\r\n%08X,%08X,%s,---",(int)list[i].f,(int)list[i].arg,list[i].name);
			}
}
