extern "C" {
#include <pspkernel.h>
#include <psptypes.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspge.h>
#include "mikmod.h"
}

#include <time.h>
#include <string.h>
#include "../framebuffer.h"
#include "../graphics.h"
#include "../sio.h"
#include "md5.h"

/* includes from host system, not PSPSDK */
#include </usr/include/unistd.h>
#include </usr/include/sys/types.h>
#include </usr/include/sys/stat.h>
#include </usr/include/fcntl.h>   /* File control definitions */
#include </usr/include/dirent.h>

#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

extern int currentControls;
extern Color* fb;
Color* currentTexture;
int currentTextureWidth, currentTextureHeight;
extern void emuRefreshDisplay();

extern "C" {

void sceGuClearDepth(unsigned int depth)
{
  //printf("NOT IMPLEMENTD: sceGuClearDepth\n");
}

void sceGuBlendFunc(int op, int src, int dest, unsigned int srcfix, unsigned int destfix)
{
  //printf("NOT IMPLEMENTD: sceGuBlendFunc\n");
}

unsigned int clear_color = 0;
void sceGuClearColor(unsigned int color)
{
  clear_color = color;
}

void sceGumMatrixMode(int mode)
{
  //printf("NOT IMPLEMENTD: sceGumMatrixMode\n");
}

/**
  * Load identity matrix
  *
  * [1 0 0 0]
  * [0 1 0 0]
  * [0 0 1 0]
  * [0 0 0 1]
**/
void sceGumLoadIdentity(void)
{
  //printf("NOT IMPLEMENTD: sceGumLoadIdentity\n");
}

void sceGumPerspective(float fovy, float aspect, float near, float far)
{
  //printf("NOT IMPLEMENTD: sceGumPerspective\n");
}

void sceGumRotateXYZ(const ScePspFVector3* v)
{
  //printf("NOT IMPLEMENTD: sceGumRotateXYZ\n");
}

void sceGumTranslate(const ScePspFVector3* v)
{
  //printf("NOT IMPLEMENTD: sceGumTranslate\n");
}

void sceGumDrawArray(int prim, int vtype, int count, const void* indices, const void* vertices)
{
  //printf("NOT IMPLEMENTD: sceGumDrawArray\n");
}

/**
  * Specify the texture environment color
  *
  * This is used in the texture function when a constant color is needed.
  *
  * See sceGuTexFunc() for more information.
  *
  * @param color - Constant color (0x00BBGGRR)
**/
void sceGuTexEnvColor(unsigned int color)
{
  //printf("NOT IMPLEMENTD: sceGuTexEnvColor\n");
}

void sceGuTexOffset(float u, float v)
{
  //printf("NOT IMPLEMENTD: sceGuTexOffset\n");
}

/**
  * Disable GE state
  *
  * Look at sceGuEnable() for a list of states
  *
  * @param state - Which state to disable
**/
void sceGuDisable(int state)
{
  //printf("NOT IMPLEMENTD: sceGuDisable\n");
}

/**
  * Set light parameters
  *
  * Available light types are:
  *   - GU_DIRECTIONAL - Directional light
  *   - GU_POINTLIGHT - Single point of light
  *   - GU_SPOTLIGHT - Point-light with a cone
  *
  * Available light components are:
  *   - GU_AMBIENT_AND_DIFFUSE
  *   - GU_DIFFUSE_AND_SPECULAR
  *   - GU_UNKNOWN_LIGHT_COMPONENT
  *
  * @param light - Light index
  * @param type - Light type
  * @param components - Light components
  * @param position - Light position
**/
void sceGuLight(int light, int type, int components, const ScePspFVector3* position)
{
  //printf("NOT IMPLEMENTD: sceGuLight\n");
}

/**
  * Set light attenuation
  *
  * @param light - Light index
  * @param atten0 - Constant attenuation factor
  * @param atten1 - Linear attenuation factor
  * @param atten2 - Quadratic attenuation factor
**/
void sceGuLightAtt(int light, float atten0, float atten1, float atten2)
{
  //printf("NOT IMPLEMENTD: sceGuLightAtt\n");
}

/**
  * Set light color
  *
  * Available light components are:
  *   - GU_AMBIENT
  *   - GU_DIFFUSE
  *   - GU_SPECULAR
  *   - GU_AMBIENT_AND_DIFFUSE
  *   - GU_DIFFUSE_AND_SPECULAR
  *
  * @param light - Light index
  * @param component - Which component to set
  * @param color - Which color to use
**/

void sceGuLightColor(int light, int component, unsigned int color)
{
  //printf("NOT IMPLEMENTD: sceGuLightColor\n");
}

void sceGuLightMode(int mode)
{
  //printf("NOT IMPLEMENTD: sceGuLightMode\n");
}

void sceGuLightSpot(int index, const ScePspFVector3* direction, float f12, float f13)
{
  //printf("NOT IMPLEMENTD: sceGuLightSpot\n");
}

void sceGuAmbient(int color)
{
  //printf("NOT IMPLEMENTD: sceGuAmbient\n");
}

/**
 * Save the GE's current state.
 *
 * @param context - Pointer to a ::PspGeContext.
 *
 * @returns ???
 */
int sceGeSaveContext(PspGeContext *context)
{
  //printf("NOT IMPLEMENTD: sceGeSaveContext\n");
  return (0);
}

/**
 * Restore a previously saved GE context.
 *
 * @param context - Pointer to a ::PspGeContext.
 *
 * @returns ???
 */
int sceGeRestoreContext(const PspGeContext *context)
{
  //printf("NOT IMPLEMENTD: sceGeRestoreContext\n");
  return (0);
}



int sceIoIoctl(SceUID fd, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	if (cmd == SIO_IOCTL_SET_BAUD_RATE)
	{
		int baudrate = 0;
		int pspbaudrate = 0;
		struct termios options;		pspbaudrate = *((int*)indata);
		switch (pspbaudrate)
		{
      case 38400:
        baudrate = B38400;
      break;
      case 19200:
        baudrate  = B19200;
      break;
      case 9600:
          baudrate  = B9600;
      break;
      case 4800:
        baudrate  = B4800;
      break;
      case 2400:
        baudrate  = B2400;
      break;
      case 1800:
        baudrate  = B1800;
      break;
      case 1200:
        baudrate  = B1200;
      break;
      case 600:
        baudrate  = B600;
      break;
      case 300:
        baudrate  = B300;
      break;
      case 200:
        baudrate  = B200;
      break;
      case 150:
        baudrate  = B150;
      break;
      case 134:
        baudrate  = B134;
      break;
      case 110:
        baudrate  = B110;
      break;
      case 75:
        baudrate  = B75;
      break;
      case 50:
        baudrate  = B50;
      break;
     
    }
		
    /*
     * Get the current options for the port...
    */
    tcgetattr(fd, &options);
    
    /*
    * Set the baud rates
    */
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    
    /*
    * Enable the receiver and set local mode...
    */
    options.c_cflag |= (CLOCAL | CREAD);
    
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    /*
    * Set the new options for the port...
    */
    
    tcsetattr(fd, TCSANOW, &options);
  }
  return (0);
}

SceUID sceIoOpen(const char *file, int flags, SceMode mode)
{
  int fd = -1; /* File descriptor for the port */
  if (strncmp(file, "sio:", 4) == 0)
  {
    fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd != -1)
    {
      fcntl(fd, F_SETFL, 0);
			fcntl(fd, F_SETFL, FNDELAY);
    }
  }
	else
	{
	  fd = open(file, O_RDWR);
	}
  return (fd);
}

int sceIoRead(SceUID fd, void *data, SceSize size)
{
  int readChars = 0;
  return (read(fd, data, size));
}

int sceIoWrite(SceUID fd, const void *data, SceSize size)
{
  return (write(fd, data, size));
}

void sceKernelDelayThread(SceUInt delay)
{
  sleep(delay);
}

void sceKernelDcacheWritebackInvalidateAll()
{
}

void sceGuStart(int cid, void* list)
{
}

void sceGuCopyImage(int psm, int sx, int sy, int width, int height, int srcw, void* src, int dx, int dy, int destw, void* dest)
{
	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			((Color*)dest)[x + dx + (y + dy) * destw] = ((Color*)src)[x + sx + (y + sy) * srcw];
		}
	}
}

int sceGuFinish(void)
{
}

int sceGuSync(int mode, int a1)
{
}

void sceGuTexImage(int mipmap, int width, int height, int tbw, const void* tbp)
{
	currentTexture = (Color*) tbp;
	currentTextureWidth = width;
	currentTextureHeight = height;
}

void sceGuTexScale(float u, float v)
{
}

void* sceGuGetMemory(int size)
{
	static char memory[1024];
	return memory;
}


typedef struct 
{
	unsigned short u, v;
	short x, y, z;
} Vertex;

void sceGuDrawArray(int prim, int vtype, int count, const void* indices, const void* vertices)
{
	Vertex* v = (Vertex*) vertices;
	int sx = v[0].u;
	int sy = v[0].v;
	int dx = v[0].x;
	int dy = v[0].y;
	int width = v[1].x - v[0].x;
	int height = v[1].y - v[0].y;
	int x, y;
	Color* dest = getVramDrawBuffer();
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			Color color = ((Color*)currentTexture)[x + sx + (y + sy) * currentTextureWidth];
			if (color & 0xFF000000) dest[x + dx + (y + dy) * 512] = color;
		}
	}
}

u8 msx[]=
"\x00\x00\x00\x00\x00\x00\x00\x00\x3c\x42\xa5\x81\xa5\x99\x42\x3c"
"\x3c\x7e\xdb\xff\xff\xdb\x66\x3c\x6c\xfe\xfe\xfe\x7c\x38\x10\x00"
"\x10\x38\x7c\xfe\x7c\x38\x10\x00\x10\x38\x54\xfe\x54\x10\x38\x00"
"\x10\x38\x7c\xfe\xfe\x10\x38\x00\x00\x00\x00\x30\x30\x00\x00\x00"
"\xff\xff\xff\xe7\xe7\xff\xff\xff\x38\x44\x82\x82\x82\x44\x38\x00"
"\xc7\xbb\x7d\x7d\x7d\xbb\xc7\xff\x0f\x03\x05\x79\x88\x88\x88\x70"
"\x38\x44\x44\x44\x38\x10\x7c\x10\x30\x28\x24\x24\x28\x20\xe0\xc0"
"\x3c\x24\x3c\x24\x24\xe4\xdc\x18\x10\x54\x38\xee\x38\x54\x10\x00"
"\x10\x10\x10\x7c\x10\x10\x10\x10\x10\x10\x10\xff\x00\x00\x00\x00"
"\x00\x00\x00\xff\x10\x10\x10\x10\x10\x10\x10\xf0\x10\x10\x10\x10"
"\x10\x10\x10\x1f\x10\x10\x10\x10\x10\x10\x10\xff\x10\x10\x10\x10"
"\x10\x10\x10\x10\x10\x10\x10\x10\x00\x00\x00\xff\x00\x00\x00\x00"
"\x00\x00\x00\x1f\x10\x10\x10\x10\x00\x00\x00\xf0\x10\x10\x10\x10"
"\x10\x10\x10\x1f\x00\x00\x00\x00\x10\x10\x10\xf0\x00\x00\x00\x00"
"\x81\x42\x24\x18\x18\x24\x42\x81\x01\x02\x04\x08\x10\x20\x40\x80"
"\x80\x40\x20\x10\x08\x04\x02\x01\x00\x10\x10\xff\x10\x10\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x20\x20\x20\x20\x00\x00\x20\x00"
"\x50\x50\x50\x00\x00\x00\x00\x00\x50\x50\xf8\x50\xf8\x50\x50\x00"
"\x20\x78\xa0\x70\x28\xf0\x20\x00\xc0\xc8\x10\x20\x40\x98\x18\x00"
"\x40\xa0\x40\xa8\x90\x98\x60\x00\x10\x20\x40\x00\x00\x00\x00\x00"
"\x10\x20\x40\x40\x40\x20\x10\x00\x40\x20\x10\x10\x10\x20\x40\x00"
"\x20\xa8\x70\x20\x70\xa8\x20\x00\x00\x20\x20\xf8\x20\x20\x00\x00"
"\x00\x00\x00\x00\x00\x20\x20\x40\x00\x00\x00\x78\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x60\x60\x00\x00\x00\x08\x10\x20\x40\x80\x00"
"\x70\x88\x98\xa8\xc8\x88\x70\x00\x20\x60\xa0\x20\x20\x20\xf8\x00"
"\x70\x88\x08\x10\x60\x80\xf8\x00\x70\x88\x08\x30\x08\x88\x70\x00"
"\x10\x30\x50\x90\xf8\x10\x10\x00\xf8\x80\xe0\x10\x08\x10\xe0\x00"
"\x30\x40\x80\xf0\x88\x88\x70\x00\xf8\x88\x10\x20\x20\x20\x20\x00"
"\x70\x88\x88\x70\x88\x88\x70\x00\x70\x88\x88\x78\x08\x10\x60\x00"
"\x00\x00\x20\x00\x00\x20\x00\x00\x00\x00\x20\x00\x00\x20\x20\x40"
"\x18\x30\x60\xc0\x60\x30\x18\x00\x00\x00\xf8\x00\xf8\x00\x00\x00"
"\xc0\x60\x30\x18\x30\x60\xc0\x00\x70\x88\x08\x10\x20\x00\x20\x00"
"\x70\x88\x08\x68\xa8\xa8\x70\x00\x20\x50\x88\x88\xf8\x88\x88\x00"
"\xf0\x48\x48\x70\x48\x48\xf0\x00\x30\x48\x80\x80\x80\x48\x30\x00"
"\xe0\x50\x48\x48\x48\x50\xe0\x00\xf8\x80\x80\xf0\x80\x80\xf8\x00"
"\xf8\x80\x80\xf0\x80\x80\x80\x00\x70\x88\x80\xb8\x88\x88\x70\x00"
"\x88\x88\x88\xf8\x88\x88\x88\x00\x70\x20\x20\x20\x20\x20\x70\x00"
"\x38\x10\x10\x10\x90\x90\x60\x00\x88\x90\xa0\xc0\xa0\x90\x88\x00"
"\x80\x80\x80\x80\x80\x80\xf8\x00\x88\xd8\xa8\xa8\x88\x88\x88\x00"
"\x88\xc8\xc8\xa8\x98\x98\x88\x00\x70\x88\x88\x88\x88\x88\x70\x00"
"\xf0\x88\x88\xf0\x80\x80\x80\x00\x70\x88\x88\x88\xa8\x90\x68\x00"
"\xf0\x88\x88\xf0\xa0\x90\x88\x00\x70\x88\x80\x70\x08\x88\x70\x00"
"\xf8\x20\x20\x20\x20\x20\x20\x00\x88\x88\x88\x88\x88\x88\x70\x00"
"\x88\x88\x88\x88\x50\x50\x20\x00\x88\x88\x88\xa8\xa8\xd8\x88\x00"
"\x88\x88\x50\x20\x50\x88\x88\x00\x88\x88\x88\x70\x20\x20\x20\x00"
"\xf8\x08\x10\x20\x40\x80\xf8\x00\x70\x40\x40\x40\x40\x40\x70\x00"
"\x00\x00\x80\x40\x20\x10\x08\x00\x70\x10\x10\x10\x10\x10\x70\x00"
"\x20\x50\x88\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xf8\x00"
"\x40\x20\x10\x00\x00\x00\x00\x00\x00\x00\x70\x08\x78\x88\x78\x00"
"\x80\x80\xb0\xc8\x88\xc8\xb0\x00\x00\x00\x70\x88\x80\x88\x70\x00"
"\x08\x08\x68\x98\x88\x98\x68\x00\x00\x00\x70\x88\xf8\x80\x70\x00"
"\x10\x28\x20\xf8\x20\x20\x20\x00\x00\x00\x68\x98\x98\x68\x08\x70"
"\x80\x80\xf0\x88\x88\x88\x88\x00\x20\x00\x60\x20\x20\x20\x70\x00"
"\x10\x00\x30\x10\x10\x10\x90\x60\x40\x40\x48\x50\x60\x50\x48\x00"
"\x60\x20\x20\x20\x20\x20\x70\x00\x00\x00\xd0\xa8\xa8\xa8\xa8\x00"
"\x00\x00\xb0\xc8\x88\x88\x88\x00\x00\x00\x70\x88\x88\x88\x70\x00"
"\x00\x00\xb0\xc8\xc8\xb0\x80\x80\x00\x00\x68\x98\x98\x68\x08\x08"
"\x00\x00\xb0\xc8\x80\x80\x80\x00\x00\x00\x78\x80\xf0\x08\xf0\x00"
"\x40\x40\xf0\x40\x40\x48\x30\x00\x00\x00\x90\x90\x90\x90\x68\x00"
"\x00\x00\x88\x88\x88\x50\x20\x00\x00\x00\x88\xa8\xa8\xa8\x50\x00"
"\x00\x00\x88\x50\x20\x50\x88\x00\x00\x00\x88\x88\x98\x68\x08\x70"
"\x00\x00\xf8\x10\x20\x40\xf8\x00\x18\x20\x20\x40\x20\x20\x18\x00"
"\x20\x20\x20\x00\x20\x20\x20\x00\xc0\x20\x20\x10\x20\x20\xc0\x00"
"\x40\xa8\x10\x00\x00\x00\x00\x00\x00\x00\x20\x50\xf8\x00\x00\x00"
"\x70\x88\x80\x80\x88\x70\x20\x60\x90\x00\x00\x90\x90\x90\x68\x00"
"\x10\x20\x70\x88\xf8\x80\x70\x00\x20\x50\x70\x08\x78\x88\x78\x00"
"\x48\x00\x70\x08\x78\x88\x78\x00\x20\x10\x70\x08\x78\x88\x78\x00"
"\x20\x00\x70\x08\x78\x88\x78\x00\x00\x70\x80\x80\x80\x70\x10\x60"
"\x20\x50\x70\x88\xf8\x80\x70\x00\x50\x00\x70\x88\xf8\x80\x70\x00"
"\x20\x10\x70\x88\xf8\x80\x70\x00\x50\x00\x00\x60\x20\x20\x70\x00"
"\x20\x50\x00\x60\x20\x20\x70\x00\x40\x20\x00\x60\x20\x20\x70\x00"
"\x50\x00\x20\x50\x88\xf8\x88\x00\x20\x00\x20\x50\x88\xf8\x88\x00"
"\x10\x20\xf8\x80\xf0\x80\xf8\x00\x00\x00\x6c\x12\x7e\x90\x6e\x00"
"\x3e\x50\x90\x9c\xf0\x90\x9e\x00\x60\x90\x00\x60\x90\x90\x60\x00"
"\x90\x00\x00\x60\x90\x90\x60\x00\x40\x20\x00\x60\x90\x90\x60\x00"
"\x40\xa0\x00\xa0\xa0\xa0\x50\x00\x40\x20\x00\xa0\xa0\xa0\x50\x00"
"\x90\x00\x90\x90\xb0\x50\x10\xe0\x50\x00\x70\x88\x88\x88\x70\x00"
"\x50\x00\x88\x88\x88\x88\x70\x00\x20\x20\x78\x80\x80\x78\x20\x20"
"\x18\x24\x20\xf8\x20\xe2\x5c\x00\x88\x50\x20\xf8\x20\xf8\x20\x00"
"\xc0\xa0\xa0\xc8\x9c\x88\x88\x8c\x18\x20\x20\xf8\x20\x20\x20\x40"
"\x10\x20\x70\x08\x78\x88\x78\x00\x10\x20\x00\x60\x20\x20\x70\x00"
"\x20\x40\x00\x60\x90\x90\x60\x00\x20\x40\x00\x90\x90\x90\x68\x00"
"\x50\xa0\x00\xa0\xd0\x90\x90\x00\x28\x50\x00\xc8\xa8\x98\x88\x00"
"\x00\x70\x08\x78\x88\x78\x00\xf8\x00\x60\x90\x90\x90\x60\x00\xf0"
"\x20\x00\x20\x40\x80\x88\x70\x00\x00\x00\x00\xf8\x80\x80\x00\x00"
"\x00\x00\x00\xf8\x08\x08\x00\x00\x84\x88\x90\xa8\x54\x84\x08\x1c"
"\x84\x88\x90\xa8\x58\xa8\x3c\x08\x20\x00\x00\x20\x20\x20\x20\x00"
"\x00\x00\x24\x48\x90\x48\x24\x00\x00\x00\x90\x48\x24\x48\x90\x00"
"\x28\x50\x20\x50\x88\xf8\x88\x00\x28\x50\x70\x08\x78\x88\x78\x00"
"\x28\x50\x00\x70\x20\x20\x70\x00\x28\x50\x00\x20\x20\x20\x70\x00"
"\x28\x50\x00\x70\x88\x88\x70\x00\x50\xa0\x00\x60\x90\x90\x60\x00"
"\x28\x50\x00\x88\x88\x88\x70\x00\x50\xa0\x00\xa0\xa0\xa0\x50\x00"
"\xfc\x48\x48\x48\xe8\x08\x50\x20\x00\x50\x00\x50\x50\x50\x10\x20"
"\xc0\x44\xc8\x54\xec\x54\x9e\x04\x10\xa8\x40\x00\x00\x00\x00\x00"
"\x00\x20\x50\x88\x50\x20\x00\x00\x88\x10\x20\x40\x80\x28\x00\x00"
"\x7c\xa8\xa8\x68\x28\x28\x28\x00\x38\x40\x30\x48\x48\x30\x08\x70"
"\x00\x00\x00\x00\x00\x00\xff\xff\xf0\xf0\xf0\xf0\x0f\x0f\x0f\x0f"
"\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x3c\x3c\x00\x00\x00\xff\xff\xff\xff\xff\xff\x00\x00"
"\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\x0f\x0f\x0f\x0f\xf0\xf0\xf0\xf0"
"\xfc\xfc\xfc\xfc\xfc\xfc\xfc\xfc\x03\x03\x03\x03\x03\x03\x03\x03"
"\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x11\x22\x44\x88\x11\x22\x44\x88"
"\x88\x44\x22\x11\x88\x44\x22\x11\xfe\x7c\x38\x10\x00\x00\x00\x00"
"\x00\x00\x00\x00\x10\x38\x7c\xfe\x80\xc0\xe0\xf0\xe0\xc0\x80\x00"
"\x01\x03\x07\x0f\x07\x03\x01\x00\xff\x7e\x3c\x18\x18\x3c\x7e\xff"
"\x81\xc3\xe7\xff\xff\xe7\xc3\x81\xf0\xf0\xf0\xf0\x00\x00\x00\x00"
"\x00\x00\x00\x00\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x00\x00\x00\x00"
"\x00\x00\x00\x00\xf0\xf0\xf0\xf0\x33\x33\xcc\xcc\x33\x33\xcc\xcc"
"\x00\x20\x20\x50\x50\x88\xf8\x00\x20\x20\x70\x20\x70\x20\x20\x00"
"\x00\x00\x00\x50\x88\xa8\x50\x00\xff\xff\xff\xff\xff\xff\xff\xff"
"\x00\x00\x00\x00\xff\xff\xff\xff\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0"
"\x0f\x0f\x0f\x0f\x0f\x0f\x0f\x0f\xff\xff\xff\xff\x00\x00\x00\x00"
"\x00\x00\x68\x90\x90\x90\x68\x00\x30\x48\x48\x70\x48\x48\x70\xc0"
"\xf8\x88\x80\x80\x80\x80\x80\x00\xf8\x50\x50\x50\x50\x50\x98\x00"
"\xf8\x88\x40\x20\x40\x88\xf8\x00\x00\x00\x78\x90\x90\x90\x60\x00"
"\x00\x50\x50\x50\x50\x68\x80\x80\x00\x50\xa0\x20\x20\x20\x20\x00"
"\xf8\x20\x70\xa8\xa8\x70\x20\xf8\x20\x50\x88\xf8\x88\x50\x20\x00"
"\x70\x88\x88\x88\x50\x50\xd8\x00\x30\x40\x40\x20\x50\x50\x50\x20"
"\x00\x00\x00\x50\xa8\xa8\x50\x00\x08\x70\xa8\xa8\xa8\x70\x80\x00"
"\x38\x40\x80\xf8\x80\x40\x38\x00\x70\x88\x88\x88\x88\x88\x88\x00"
"\x00\xf8\x00\xf8\x00\xf8\x00\x00\x20\x20\xf8\x20\x20\x00\xf8\x00"
"\xc0\x30\x08\x30\xc0\x00\xf8\x00\x18\x60\x80\x60\x18\x00\xf8\x00"
"\x10\x28\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xa0\x40"
"\x00\x20\x00\xf8\x00\x20\x00\x00\x00\x50\xa0\x00\x50\xa0\x00\x00"
"\x00\x18\x24\x24\x18\x00\x00\x00\x00\x30\x78\x78\x30\x00\x00\x00"
"\x00\x00\x00\x00\x30\x00\x00\x00\x3e\x20\x20\x20\xa0\x60\x20\x00"
"\xa0\x50\x50\x50\x00\x00\x00\x00\x40\xa0\x20\x40\xe0\x00\x00\x00"
"\x00\x38\x38\x38\x38\x38\x38\x00\x00\x00\x00\x00\x00\x00\x00";

void* sceGuSwapBuffers(void)
{
  emuRefreshDisplay();
}

void sceDisplaySetFrameBuf(void *topaddr, int bufferwidth, int pixelformat, int sync)
{
}

int sceDisplaySetMode(int mode, int width, int height)
{
}

int sceDisplayWaitVblankStart(void)
{
  emuRefreshDisplay();
	static float lastTime = 0;
	float t = ((float) clock()) / ((float) CLOCKS_PER_SEC);
	if (t - lastTime < 1.0/60.0) {
		struct timespec w;
		w.tv_sec  = 0;
		w.tv_nsec = (int) (1000000000.0 * (1.0/60.0 - (t - lastTime)));
		nanosleep(&w, NULL);
	}
	lastTime = ((float) clock()) / ((float) CLOCKS_PER_SEC);
}

void sceGuDrawBuffer(int psm, void* fbp, int fbw)
{
}

void sceGuDispBuffer(int width, int height, void* dispbp, int dispbw)
{
}

void sceGuClear(int flags)
{
	Color* dest = getVramDrawBuffer();
	for (int n = 0; n  < PSP_LINE_SIZE*SCREEN_HEIGHT; n++)
  {
    dest[n] = clear_color;
	}
}

void sceGuDepthBuffer(void* zbp, int zbw)
{
}

void sceGuOffset(unsigned int x, unsigned int y)
{
}

void sceGuViewport(int cx, int cy, int width, int height)
{
}

void sceGuDepthRange(int near, int far)
{
}

void sceGuScissor(int x, int y, int w, int h)
{
}

void sceGuEnable(int state)
{
}

void sceGuAlphaFunc(int func, int value, int mask)
{
}

void sceGuDepthFunc(int function)
{
}

void sceGuFrontFace(int order)
{
}

void sceGuShadeModel(int mode)
{
}

void sceGuTexMode(int tpsm, int maxmips, int a2, int swizzle)
{
}

void sceGuTexFunc(int tfx, int tcc)
{
}

void sceGuTexFilter(int min, int mag)
{
}

void sceGuAmbientColor(unsigned int color)
{
}

int sceCtrlReadBufferPositive(SceCtrlData *pad_data, int count)
{
	pad_data->Buttons = currentControls;
  pad_data->Lx = 128;
  pad_data->Ly = 128;
  
	return 0;
}


int sceCtrlSetSamplingMode(int mode)
{
}


SceUID sceIoDopen(const char *dirname)
{
  DIR* dir = NULL;
  if (dirname && (strlen(dirname) == 0))
  {
    dir = opendir(".");
  }
  else
  {
    dir = opendir(dirname);
  }
  if (NULL != dir)
  {
    return ((SceUID) dir);
  }
  return (-1);
}


int sceIoDread(SceUID fd, SceIoDirent *dir)
{
  struct stat statbuf;
  dirent* ent = readdir((DIR*)fd);
  if (NULL != ent)
  {
    strcpy(dir->d_name, ent->d_name);
    stat(dir->d_name, &statbuf);
    dir->d_stat.st_size = statbuf.st_size;
    dir->d_stat.st_attr = 0;
    if (S_ISDIR(statbuf.st_mode))
    {
      dir->d_stat.st_attr |= FIO_SO_IFDIR;
    }
    if (S_ISLNK(statbuf.st_mode))
    {
      dir->d_stat.st_attr |= FIO_SO_IFLNK;
    }
    if (S_ISREG(statbuf.st_mode))
    {
      dir->d_stat.st_attr |= FIO_SO_IFREG;
    }
    
    // TODO: Read, Write, Execute flags mapping
    
    return (1);
  }
  return (-1);
}


int sceIoDclose(SceUID fd)
{
  if (0 == closedir((DIR*)fd))
    return (0);
  return (-1);
}


SceUID sceKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
}


int sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
}


int sceUsbStart(const char* driverName, int unknown1, int unknown2)
{
}

int sceKernelSleepThread(void)
{
}


int sceUsbstorBootSetCapacity(u32 size)
{
}

int sceUsbActivate(u32 flag)
{
}

int sceUsbDeactivate()
{
}

void sceGuInit(void)
{
}

int sceGuDisplay(int state)
{
}

int scePowerIsPowerOnline()
{
	return 1;
}

int scePowerIsBatteryExist()
{
	return 1;
}

int scePowerIsBatteryCharging()
{
	return 0;
}

int scePowerGetBatteryChargingStatus()
{
	return 0;
}

int scePowerIsLowBattery()
{
	return 0;
}

int scePowerGetBatteryLifePercent()
{
	return 0;
}

int scePowerGetBatteryLifeTime()
{
	return 0;
}

int scePowerGetBatteryTemp()
{
	return 0;
}

int scePowerGetBatteryVolt()
{
	return 0;
}

int sceKernelUtilsMd5Digest(u8 *data, u32 size, u8 *digest)
{
  MD5_CTX ctx;
  MD5Init( &ctx );
  MD5Update( &ctx, data, size );
  MD5Final( digest, &ctx );
}

}
