#ifndef __SYSTEM_H
#define __SYSTEM_H


#include "pad.h"
#include "libtime.h"

extern u8 fileXioIrx_start[];
extern int fileXioIrx_size;
extern u8 iomanXIrx_start[];
extern int iomanXIrx_size;
extern u8 UsbdIrx_start[];
extern int UsbdIrx_size;
extern u8 usbhdfsdIrx_start[];
extern int usbhdfsdIrx_size;

typedef enum {
	BOOT_HOST,
	BOOT_MC,
	BOOT_HDD,
	BOOT_USB,
	BOOT_CD
} deviceType_t;


extern char boot_path[255];

#ifdef DEBUG
#define dbgprintf(args...) scr_printf(args)
#else
#define dbgprintf(args...) ;
#endif

int getBootDevice(void);

#endif

