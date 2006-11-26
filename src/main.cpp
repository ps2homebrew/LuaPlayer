/*
 * Lua Player for PS2
 * ------------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE for details.
 *
 * Copyright (c) 2005 Frank Buss <fb@frank-buss.de> (aka Shine)
 *
 * Credits:
 *   many thanks to the authors of the PSPSDK from http://forums.ps2dev.org
 *   and to the hints and discussions from #pspdev on freenode.net
 *
 * $Id: main.cpp 357 2006-06-03 17:54:53Z shine $
 */


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libmc.h>
#include <libcdvd.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include <smod.h>

#include <sbv_patches.h>
#include "smem.h"

#include "graphics.h"
#include "sound.h"
#include "luaplayer.h"

/* the boot.lua */
#include "boot.cpp"


#ifdef STANDALONE
extern u8 scriptLua_start[];
extern int scriptLua_size;
#endif

char boot_path[255];


static int _bootDevice ;

void setBootDevice(const char *path)
{
	int i;

	// figure out boot mode
	i = strcspn( path, ":" );

	if( !strncmp( path, "host", i ) ) {
		_bootDevice  = BOOT_HOST;
	}
	else if( !strncmp( path, "mc", i ) ) {
		_bootDevice  = BOOT_MC;
	}
	else if( !strncmp( path, "mass", i ) ) {
		_bootDevice  = BOOT_USB;
	}
	else if( !strncmp( path, "cdrom0", i ) || !strncmp( path, "cdfs", i ) ) {
		_bootDevice  = BOOT_CD;
	}
}

int getBootDevice(void)
{
    return _bootDevice ;
}




void Init_Usb_Modules(void)
{
    int ret;
    
    SifExecModuleBuffer(&UsbdIrx_start, UsbdIrx_size, 0, NULL, &ret);
    SifExecModuleBuffer(&usbhdfsdIrx_start, usbhdfsdIrx_size, 0, NULL, &ret);

}

void initMC(void)
{
   int ret;
   // mc variables
   int mc_Type, mc_Free, mc_Format;

   
   printf("Initializing Memory Card\n");

   ret = mcInit(MC_TYPE_XMC);
   
   if( ret < 0 ) {
	printf("MC_Init : failed to initialize memcard server.\n");
	dbgprintf("MC_Init : failed to initialize memcard server.\n");
   }
   
   // Since this is the first call, -1 should be returned.
   // makes me sure that next ones will work !
   mcGetInfo(0, 0, &mc_Type, &mc_Free, &mc_Format); 
   mcSync(MC_WAIT, NULL, &ret);
   
   


}

void resetIOP(void)
{
    #ifdef RESET_IOP        
    printf("Resetting IOP.");
    cdInit(CDVD_INIT_EXIT);
    SifExitIopHeap();
    SifLoadFileExit();
    SifExitRpc();
    SifIopReset("rom0:UDNL rom0:EELOADCNF", 0);
    while (!SifIopSync())
  	;
    printf("IOP synced.");
    SifInitRpc(0);
    SifLoadFileInit();
    #endif
}



void systemInit()
{
    int ret;
    SifInitRpc(0);
    
    if (getBootDevice() != BOOT_HOST)
      resetIOP();

    
    // install sbv patch fix
    printf("Installing SBV Patch...\n");
    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check();
    
    // load pad & mc modules 
    printf("Installing Pad & MC modules...\n");
    ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule sio failed: %d\n", ret);
        SleepThread();
    }   
    ret = SifLoadModule("rom0:XMCMAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule sio failed: %d\n", ret);
        SleepThread();
    }   
    
    ret = SifLoadModule("rom0:XMCSERV", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule sio failed: %d\n", ret);
        SleepThread();
    }   
    
    ret = SifLoadModule("rom0:XPADMAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule sio failed: %d\n", ret);
        SleepThread();
    }   
    
    SifExecModuleBuffer(&iomanXIrx_start, iomanXIrx_size, 0, NULL, &ret);
    if (ret < 0) {
        printf("SifExecModuleBuffer sio failed: %d\n", ret);
        SleepThread();
    }   
    SifExecModuleBuffer(&fileXioIrx_start, fileXioIrx_size, 0, NULL, &ret); 
    if (ret < 0) {
        printf("SifExecModuleBuffer sio failed: %d\n", ret);
        SleepThread();
    }   
    
    // load USB modules    
    Init_Usb_Modules();
    
    // int MC
    initMC();
       
    // script debugging purpose
    init_scr();
   
}



int user_main(int argc, char ** argv)
{
    
    systemInit();
         
    return (0);
}


void setLuaBootPath(int argc, char ** argv, int idx)
{
    // detect host : strip elf from path
    if (argc == 0)  // argc == 0 usually means naplink..
    {
	 strcpy (boot_path,"host:");
	 
    }
    else if (argc>=(idx+1))
    {

	char *p;
	if ((p = strrchr(argv[idx], '/'))!=NULL) {
	    snprintf(boot_path, sizeof(boot_path), "%s", argv[idx]);
	    p = strrchr(boot_path, '/');
	if (p!=NULL)
	    p[1]='\0';
	} else if ((p = strrchr(argv[idx], '\\'))!=NULL) {
	   snprintf(boot_path, sizeof(boot_path), "%s", argv[idx]);
	   p = strrchr(boot_path, '\\');
	   if (p!=NULL)
	     p[1]='\0';
	} else if ((p = strchr(argv[idx], ':'))!=NULL) {
	   snprintf(boot_path, sizeof(boot_path), "%s", argv[idx]);
	   p = strchr(boot_path, ':');
	   if (p!=NULL)
	   p[1]='\0';
	}

    }
    
    //strcpy (boot_path,"mass:LUA/");
    
    // check if path needs patching
    if( !strncmp( boot_path, "mass:/", 6) && (strlen (boot_path)>6))
    {
        strcpy((char *)&boot_path[5],(const char *)&boot_path[6]);
    }
      
    
}

int lua_main(int argc, char ** argv)
{
    const char * errMsg;
    
    // init internals library
    
    // graphics (gsKit)
    initGraphics();
    
    // initializePad() can only handle 1 pad for now
    initializePad(0, 0);
    
    // Init the timer stuff
    ps2time_init();
    
    // Init Libmikmod
    initMikmod();
    
    // set base path luaplayer
    chdir(boot_path); 
    
    while (1)
    {
    
       #ifndef STANDALONE
       // if no parameters are specified, use the default boot
       if (argc < 2) 
       {
           // execute Lua script (according to boot sequence)
           char* bootStringWith0 = (char*) malloc(size_bootString + 1);
	   memcpy(bootStringWith0, bootString, size_bootString);
	   bootString[size_bootString] = 0;
           // execute the boot script
           errMsg = runScript(bootStringWith0, true);
           free (bootStringWith0);
       }       
       else
          // execute the script gived as parameter
          errMsg = runScript(argv[1], false);       
    
       // Standalone elf with an embedded script
       #else
       errMsg = runScript((char*)scriptLua_start, true);
       #endif

       gsKit_clear_screens();
       
       if ( errMsg != NULL)
       {
   	   scr_printf("Error: %s\n", errMsg );
       }
       
       scr_printf("\nPress [start] to restart\n");
       while (!isButtonPressed(PAD_START)) graphicWaitVblankStart();
    
    }
    return 0;
}


int main(int argc, char * argv[])
{
	// set the boot device
	setBootDevice (argv[0]);
	
	// PS2 specific initialization 
	user_main (argc, argv);

        #ifndef STANDALONE	
        // if no parameters are specified, use the default boot
	if (argc < 2)
	{
	   // set boot path global variable based on the elf path
	   setLuaBootPath (argc, argv, 0);  
        }
        else // set path based on the specified script
        {
           if (!strchr(argv[1], ':')) // filename doesn't contain device
              // set boot path global variable based on the elf path
	      setLuaBootPath (argc, argv, 0);  
           else
              // set path global variable based on the given script path
	      setLuaBootPath (argc, argv, 1);
	}
	#else
	// set boot path global variable based on the elf path
	setLuaBootPath (argc, argv, 0);
        #endif
	
	printf("boot path : %s\n", boot_path);
	dbgprintf("boot path : %s\n", boot_path);
	
	// Lua init
	lua_main (argc, argv);
	return 0;
}

