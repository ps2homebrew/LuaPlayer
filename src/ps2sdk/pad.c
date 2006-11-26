/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id: pad.c 1152 2005-06-12 17:49:50Z oopo $
# Pad demo app
# Quick and dirty, little or no error checks etc.. 
# Distributed as is
*/

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>

#include "pad.h"


/*
 * Global var's
 */
 
// pad_dma_buf is provided by the user, one buf for each pad
// contains the pad's current state
static char padBuf[256] __attribute__((aligned(64)));

static struct padButtonStatus buttons;

static char actAlign[6];
static int actuators;
int ret;


/*
 * Local functions
 */

/*
 * waitPadReady()
 */
void waitPadReady(int port, int slot)
{
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
        }
        lastState = state;
        state=padGetState(port, slot);
    }
}


/*
 * initializePad()
 */
int _setupPad(int port, int slot)
{

    int modes;
    int i;
    
    waitPadReady(port, slot);

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);

    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);

    if (i >= modes) {
        printf("This is no Dual Shock controller\n");
        return 1;
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    if (padInfoMode(port, slot, PAD_MODECUREXID, 0) == 0) {
        printf("This is no Dual Shock controller??\n");
        return 1;
    }

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);


    if (actuators != 0) {
        actAlign[0] = 0;   // Enable small engine
        actAlign[1] = 1;   // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady(port, slot);
    }


    waitPadReady(port, slot);
    
    return 1;
}

int
initializePad(int port, int slot)
{
    int ret;	
	
    ret = padInit(0);
    
    printf("luacontrols: padInit: %d\n", ret);

    // paddle 1
    printf("Init Paddle on port %d\n",port);
    if((ret = padPortOpen(port, slot, padBuf)) != 0)
    {
        if(!_setupPad(port, slot))
    	{
        	printf("pad initalization failed!\n");
        	SleepThread();
    	}
    }
    else
    {
        printf("padOpenPort failed: %d\n", ret);
        SleepThread();
    }
    printf("...done\n");
    return 1;
}


void padProcessEvents(int port, int slot, SceCtrlData *Controls)
{
    Controls->Buttons = 0;
    
    // ------- controller 1 ---------------------------------------
    while (((ret=padGetState(port, slot)) != PAD_STATE_STABLE)&&(ret!=PAD_STATE_FINDCTP1)&&(ret != PAD_STATE_DISCONN));

    if (padRead(port, slot, &buttons) != 0)
    {
       Controls->Buttons = 0xffff ^ buttons.btns;
       Controls->Lx = buttons.ljoy_h;
       Controls->Ly = buttons.ljoy_v;
    } //endif
    	
}

// Return if the given button is pressed
int isButtonPressed(u32 button)
{
   int ret;
   u32 paddata;
   
   struct padButtonStatus padbuttons;
   
   while (((ret=padGetState(0, 0)) != PAD_STATE_STABLE)&&(ret!=PAD_STATE_FINDCTP1)&&(ret != PAD_STATE_DISCONN)); // more error check ?
   if (padRead(0, 0, &padbuttons) != 0)
   {
    	paddata = 0xffff ^ padbuttons.btns;
     	if(paddata & button)
            return 1;
   }
   return 0;

}
