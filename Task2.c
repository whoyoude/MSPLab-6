//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//

#include "init.h"
#include <stdint.h>
#include<stdlib.h>
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"

USBH_HandleTypeDef husbh;
char path[4];
uint8_t connect = 0; //If device is connect it becomes 1.
uint8_t hid = 0; //If device belong to HID class, it becomes 1.
uint8_t msc = 0; //If device belong to MSC class, it becomes 1.
uint8_t hid_connect = 0;
uint8_t printplz = 0;
float cursorx =0;
float cursory = 0;
int8_t x = 0;
int8_t y = 0;


void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);
FRESULT scan_f(char* path);
int round(float d);

int main(void){
	 // System Initializations
	Sys_Init();
	USBH_Init(&husbh, USBH_UserProcess, 0);
	FATFS_LinkDriver(&USBH_Driver, path);
	USBH_RegisterClass(&husbh, USBH_MSC_CLASS);
	USBH_RegisterClass(&husbh, USBH_HID_CLASS);
	USBH_Start(&husbh);

	while(1){
		USBH_Process(&husbh);
		if(hid == 0 && msc ==1){//MSC class device is connected
			if(printplz){//Check if the MSC has been scanned
				scan_f(path);//Scan file.
				printplz = 0;//Set global variable that allow print to 0.
			}
		}
		if(hid == 1 && msc ==0){//HID class device is connected
			while(hid){
				USBH_Process(&husbh);
				if(hid_connect){
			       hid_connect = 0;
				}
			}
		}
	}
}

int round(float d){
	int dint = (int)d;
	if (d>=dint+0.5){
		return dint+1;
	}
	else{
		return dint;
	}
}

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

FRESULT scan_f(char* path){
	FRESULT res;
	FATFS fs;
	DIR dir;
	static FILINFO fno;

	f_mount(&fs,path,1);//Mount the file before scan.
	res = f_opendir(&dir, path);

	if (res== FR_OK){
		while (connect){
			res = f_readdir(&dir, &fno);
			if(res != FR_OK || fno.fname[0] == 0){//Error in opening file or directory, or reach the end of reading
				printf("Stop scan\r\n");
				break;
			}
			else{
				if(fno.fattrib & AM_DIR){//Directory detected.
					printf("%s which is a direcory\r\n", fno.fname);
				}
				else{//File detected.
					printf("%s which is a file\r\n", fno.fname);
				}
			}
		}
		f_closedir(&dir);
	}
	return res;
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	switch(id)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_CLASS_ACTIVE:
		if(phost->pActiveClass == USBH_MSC_CLASS){//Detect MSC class device
			printf("MSC device detected\r\n");
			msc = 1;
			printplz = 1;
		}
		if(phost->pActiveClass == USBH_HID_CLASS){//Detect HID class device
			printf("HID device detected\r\n");
			hid = 1;
			hid_connect = 1;
		}
		break;

	case HOST_USER_CLASS_SELECTED:
		break;

	case HOST_USER_CONNECTION:
		connect =1;
		break;

	case HOST_USER_DISCONNECTION:
		connect =0;
		hid_connect = 0;
		msc = 0;
		hid = 0;
		printplz =0;
		break;

	case HOST_USER_UNRECOVERED_ERROR:
		break;
	}
}

// Interrupts and Callbacks...

