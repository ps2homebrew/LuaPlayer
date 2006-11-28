#include <stdlib.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>


#include "graphics.h"
#include "lualogo.cpp"


/* 480x272 splash texture */
static GSTEXTURE gsSplashTexture;


/* 
 This function display the "Lua Powered" Logo
 The function assume that gsKit is already initialized
 */

void displaySplashScreen(GSGLOBAL *gsGlobal)
{
   //int alpha = 0;

   // if texture not initialized
   if (gsSplashTexture.Mem == NULL)
   {
   	int size;

   	unsigned char *fb, *splash;
   	gsSplashTexture.Width = SCREEN_WIDTH;
	gsSplashTexture.Height = SCREEN_HEIGHT;
	gsSplashTexture.PSM = GS_PSM_CT24;
	
	// useless but keep compiler happy :)
	size = size_rawlualogo;
	
	size = gsKit_texture_size(SCREEN_WIDTH, SCREEN_HEIGHT, GS_PSM_CT24);
	
	gsSplashTexture.Mem = (u32 *)memalign(128, size);
	
	// copy the texture into memory
	// not sure if I can directly point to my buffer (alignement?)
	fb = (unsigned char *)gsSplashTexture.Mem;
	splash = &rawlualogo[0];
	for (int i=size;i--;) *fb++ = *splash++;
	  
   }
   
   // clear the screen
   gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00));
   
   // upload new frame buffer
   gsKit_texture_upload(gsGlobal, &gsSplashTexture);
   

   //while(alpha < 0x80)
   //{
    
      gsKit_prim_sprite_striped_texture( gsGlobal,  &gsSplashTexture,
				((gsGlobal->Width - gsSplashTexture.Width) / 2),
				((gsGlobal->Height - gsSplashTexture.Height) / 2),
				0,
				0,
				gsSplashTexture.Width  + ((gsGlobal->Width - gsSplashTexture.Width) / 2),
				gsSplashTexture.Height + ((gsGlobal->Height - gsSplashTexture.Height) / 2),
				gsSplashTexture.Width,
				gsSplashTexture.Height,
				1,
				GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00)
				);

      gsKit_queue_exec(gsGlobal);
   
      gsKit_sync_flip(gsGlobal);
    
    //  alpha +=2;	

   //}
   
  
   
}




/* 
 This function display the "Lua Powered" Logo
 The function assume that gsKit is already initialized
 */
/*
void displaySplashScreen(GSGLOBAL *gsGlobal)
{
   //int alpha = 0;
   Image *image;
   unsigned char *fb, *splash;
   
   // if texture not initialized
   if (gsSplashTexture.Mem == NULL)
   {
	int size;
	
	gsSplashTexture.PSM = GS_PSM_CT32;
	gsSplashTexture.Width =  SCREEN_WIDTH;
	gsSplashTexture.Height = SCREEN_HEIGHT;
	
        size = gsKit_texture_size(gsSplashTexture.Width, gsSplashTexture.Height, gsSplashTexture.PSM );
	
	gsSplashTexture.Mem = (u32 *)memalign(128, size);
	
	image = loadImageFromMemory(jpglualogo, size_jpglualogo);	
	
        // copy the texture into memory
	// not sure if I can directly point to my buffer (alignement?)
	fb = (unsigned char *)gsSplashTexture.Mem;
	splash = (unsigned char *)&image->data[0];
	for (int i=size;i--;) *fb++ = *splash++;
	
	gsKit_texture_upload(gsGlobal, &gsSplashTexture);
	
	free(gsSplashTexture.Mem);
	free(image);
	
	
	

   }
   
   // clear the screen
   gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00));
   
   // upload new frame buffer
   gsKit_texture_upload(gsGlobal, &gsSplashTexture);
   

   gsKit_prim_sprite_striped_texture( gsGlobal,  &gsSplashTexture,
				((gsGlobal->Width - gsSplashTexture.Width) / 2),
				((gsGlobal->Height - gsSplashTexture.Height) / 2),
				0,
				0,
				gsSplashTexture.Width  + ((gsGlobal->Width - gsSplashTexture.Width) / 2),
				gsSplashTexture.Height + ((gsGlobal->Height - gsSplashTexture.Height) / 2),
				gsSplashTexture.Width,
				gsSplashTexture.Height,
				1,
				GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00)
				);

   gsKit_queue_exec(gsGlobal);
   gsKit_sync_flip(gsGlobal);
   
}

*/
