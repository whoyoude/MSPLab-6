//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//

#include "init.h"
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

USBH_HandleTypeDef husbh;

void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);

float cursorx = 0;
float cursory = 0;

int round(float d)
{
    int dint=(int)d;

    if(d>=dint+0.5)
    {
        return dint+1;
    }
    else
    {
        return dint;
    }
}

int main(void){
	 // System Initializations
	Sys_Init();

	// Application Initializations
	// USBH Driver Initialization
	// USB Driver Class Registrations: Add device types to handle.
	// Start USBH Driver

	/* Init Host Library */

	 USBH_Init(&husbh, USBH_UserProcess, 0);

	 /* Add Supported Class: example HID class*/
	 USBH_RegisterClass(&husbh, USBH_HID_CLASS);


	/* Start Host Process */

	 USBH_Start(&husbh);


	 printf("\033[2J\033[;H");
	 fflush(stdout);

	while(1){
		USBH_Process(&husbh);
		// Other stuff
	}
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	// ...
	 switch (id)
	 {
	 case HOST_USER_DISCONNECTION:
	 //Appli_state = APPLICATION_DISCONNECT;
	 break;

	/* when HOST_USER_CLASS_ACTIVE event is received, application can start
	 communication with device*/
	 case HOST_USER_CLASS_ACTIVE:
	 //Appli_state = APPLICATION_READY;
		 //("\033[;H");
		 printf("active");
		 fflush(stdout);
	 break;

	 case HOST_USER_CONNECTION:
	 //Appli_state = APPLICATION_START;
		 printf("connected");
		 fflush(stdout);
	 break;
	 default:
	 break;
	 }
}

// Interrupts and Callbacks...
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost){
	if (USBH_HID_GetDeviceType(phost)==HID_MOUSE){
		int8_t x,y;
		HID_MOUSE_Info_TypeDef* info = USBH_HID_GetMouseInfo(phost);
		x=info->x;
		y=info->y;
		//printf("\n\rx=%d y=%d",x,y);
		//fflush(stdout);

		cursorx+=0.1*x;
		if(cursorx<1) cursorx=1;
		if(cursorx>82) cursorx=82;
		cursory+=0.1*y;
		if(cursory<1) cursory=1;
		if(cursory>24) cursory=24;
		printf("\033[%d;%dH",round(cursory),round(cursorx));
		fflush(stdout);
		if(info->buttons[0]){
			printf("o");
			fflush(stdout);
		}
		if(info->buttons[1]){
			printf("\033[2K");
			fflush(stdout);
		}

	}

}
