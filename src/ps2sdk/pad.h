
#ifndef __PAD_H
#define __PAD_H


#include "libpad.h"

#ifdef __cplusplus 
extern "C" { 
#endif 

/** Returned controller data 
  * should be changed later
  */
typedef struct SceCtrlData {
	/** Bit mask containing zero or more Buttons status . */
	unsigned int 	Buttons;
	/** Analogue stick, X axis. */
	unsigned char 	Lx;
	/** Analogue stick, Y axis. */
	unsigned char 	Ly;
} SceCtrlData; // this is a hack to ease reuse

typedef SceCtrlData Controls;

int initializePad(int port, int slot);

void waitPadReady(int port, int slot);

void padProcessEvents(int port, int slot, SceCtrlData *Controls);

int isButtonPressed(u32 button);

#ifdef __cplusplus 
} 
#endif 

#endif 
