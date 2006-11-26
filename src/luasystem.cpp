#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libmc.h>
#include <fileXio_rpc.h>
#include <sys/stat.h>
#include "luaplayer.h"
#include "ps2sdk/md5.h"

#define MAX_DIR_FILES 512




/* Normalize a pathname by removing
  . and .. components, duplicated /, etc. */
char* __ps2_normalize_path(char *path_name)
{
	int i, j;
	int first, next;
	static char out[255];
	
	/* First copy the path into our temp buffer */
	strcpy(out, path_name);
        /* Then append "/" to make the rest easier */
	strcat(out,"/");

	/* Convert "//" to "/" */
	for(i=0; out[i+1]; i++) {
		if(out[i]=='/' && out[i+1]=='/') {
			for(j=i+1; out[j]; j++)
				out[j] = out[j+1];
			i--;
		}
	}

	/* Convert "/./" to "/" */
	for(i=0; out[i] && out[i+1] && out[i+2]; i++) {
		if(out[i]=='/' && out[i+1]=='.' && out[i+2]=='/') {
			for(j=i+1; out[j]; j++)
				out[j] = out[j+2];
			i--;
		}
	}

	/* Convert "/path/../" to "/" until we can't anymore.  Also
	 * convert leading "/../" to "/" */
	first = next = 0;
	while(1) {
		/* If a "../" follows, remove it and the parent */
		if(out[next+1] && out[next+1]=='.' && 
		   out[next+2] && out[next+2]=='.' &&
		   out[next+3] && out[next+3]=='/') {
			for(j=0; out[first+j+1]; j++)
				out[first+j+1] = out[next+j+4];
			first = next = 0;
			continue;
		}

		/* Find next slash */
		first = next;
		for(next=first+1; out[next] && out[next] != '/'; next++)
			continue;
		if(!out[next]) break;
	}

	/* Remove trailing "/" */
	for(i=1; out[i]; i++)
		continue;
	if(i >= 1 && out[i-1] == '/') 
		out[i-1] = 0;

	return (char*)out;
}



static int lua_getCurrentDirectory(lua_State *L)
{
	char path[256];
	getcwd(path, 256);
	lua_pushstring(L, path);
	
	return 1;
}

static int lua_setCurrentDirectory(lua_State *L)
{
        static char temp_path[256];
	const char *path = luaL_checkstring(L, 1);
	if(!path) return luaL_error(L, "Argument error: System.currentDirectory(file) takes a filename as string as argument.");

	lua_getCurrentDirectory(L);
	
	// let's do what the ps2sdk should do, 
	// some normalization... :)
	// if absolute path (contains [drive]:path/)
	if (strchr(path, ':'))
	{
	      strcpy(temp_path,path);
	}
	else // relative path
	{
	   // remove last directory ?
	   if(!strncmp(path, "..", 2))
	   {
	        getcwd(temp_path, 256);
	        if ((temp_path[strlen(temp_path)-1] != ':'))
	        {
	           int idx = strlen(temp_path)-1;
	           do
	           {
	                idx--;
	           } while (temp_path[idx] != '/');
	           temp_path[idx] = '\0';
	        }
	        
           }
           // add given directory to the existing path
           else
           {
	      getcwd(temp_path, 256);
	      strcat(temp_path,"/");
	      strcat(temp_path,path);
	   }
        }
        
        printf("changing directory to %s\n",__ps2_normalize_path(temp_path));
        chdir(__ps2_normalize_path(temp_path));
       
	return 1;
}

static int lua_curdir(lua_State *L) {
	int argc = lua_gettop(L);
	if(argc == 0) return lua_getCurrentDirectory(L);
	if(argc == 1) return lua_setCurrentDirectory(L);
	return luaL_error(L, "Argument error: System.currentDirectory([file]) takes zero or one argument.");
}


static int lua_dir(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc != 0 && argc != 1) return luaL_error(L, "Argument error: System.listDirectory([path]) takes zero or one argument.");
	
        const char *temp_path = "";
	char path[255];
	
	getcwd((char *)path, 256);
	printf("current dir %s\n",(char *)path);
	
	if (argc != 0) 
	{
		temp_path = luaL_checkstring(L, 1);
		// append the given path to the boot_path
	        
	        strcpy ((char *)path, boot_path);
	        
	        if (strchr(temp_path, ':'))
	           // workaround in case of temp_path is containing 
	           // a device name again
	           strcpy ((char *)path, temp_path);
	        else
	           strcat ((char *)path, temp_path);
	}
	
	strcpy(path,__ps2_normalize_path(path));
	printf("\nchecking path : %s\n",path);
		

        
        //-----------------------------------------------------------------------------------------
	
	// read from MC ?
        
        if( !strncmp( path, "mc0:", strlen("mc0:") ) || !strncmp( path, "mc1:", strlen("mc1:") ) )
        {       
                int	nPort;
                int	numRead;
                char    mcPath[256];
		mcTable mcEntries[MAX_DIR_FILES] __attribute__((aligned(64)));
		
		if( !strncmp( path, "mc0:", strlen("mc0:") ) )
			nPort = 0;
		else
			nPort = 1;
		
		
		// copy only the path without the device (ie : mc0:/xxx/xxx -> /xxx/xxx)
		strcpy(mcPath,(char *)&path[4]);
				
		// it temp_path is empty put a "/" inside
                if (strlen(mcPath)==0)
                   strcpy((char *)mcPath,(char *)"/");
		

		if (mcPath[strlen(mcPath)-1] != '/')
		  strcat( mcPath, "/*" );
		else
		  strcat( mcPath, "*" );
	
		mcGetDir( nPort, 0, mcPath, 0, MAX_DIR_FILES, mcEntries );
   		while (!mcSync( MC_WAIT, NULL, &numRead ));
   		                	    
	        int cpt = 1;
	        lua_newtable(L);

		for( int i = 0; i < numRead; i++ )
		{
                        lua_pushnumber(L, cpt++);  // push key for file entry

	                lua_newtable(L);
          	                lua_pushstring(L, "name");
        	                lua_pushstring(L, (const char *)mcEntries[i].name);
         	                lua_settable(L, -3);
        		
        	                lua_pushstring(L, "size");
        	                lua_pushnumber(L, mcEntries[i].fileSizeByte);
        	                lua_settable(L, -3);
        	
                        	lua_pushstring(L, "directory");
        	                lua_pushboolean(L, ( mcEntries[i].attrFile & MC_ATTR_SUBDIR ));
        	                lua_settable(L, -3);
	                lua_settable(L, -3);

		}
		return 1;  /* table is already on top */
        }
        //-----------------------------------------------------------------------------------------
        
        // else regular one using Dopen/Dread
        int fd = fileXioDopen((char *)path);

	if (fd < 0) {
		lua_pushnil(L);  // return nil
		return 1;
	}
	
	lua_newtable(L);
	
	int i = 1;
	iox_dirent_t dircontent;
		
	// other cases
	while (fileXioDread(fd, &dircontent) > 0)
	{
	        lua_pushnumber(L, i++);  // push key for file entry

		lua_newtable(L);
		   	lua_pushstring(L, "name");
        		lua_pushstring(L, dircontent.name);
        	  	lua_settable(L, -3);
        		
        		lua_pushstring(L, "size");
        		lua_pushnumber(L, dircontent.stat.size);
        		lua_settable(L, -3);
        	        
        		lua_pushstring(L, "directory");
        		lua_pushboolean(L,  FIO_S_ISDIR(dircontent.stat.mode));
        		lua_settable(L, -3);
		lua_settable(L, -3);
	}

	fileXioDclose(fd);

	return 1;  /* table is already on top */
}

static int lua_createDir(lua_State *L)
{
	const char *path = luaL_checkstring(L, 1);
	if(!path) return luaL_error(L, "Argument error: System.createDirectory(directory) takes a directory name as string as argument.");
        
	mkdir(path, 0777);
	
	return 0;
}

static int lua_removeDir(lua_State *L)
{
	const char *path = luaL_checkstring(L, 1);
	if(!path) return luaL_error(L, "Argument error: System.removeDirectory(directory) takes a directory name as string as argument.");
        

	rmdir(path);

	
	return 0;
}

static int lua_removeFile(lua_State *L)
{
	const char *path = luaL_checkstring(L, 1);
	if(!path) return luaL_error(L, "Argument error: System.removeFile(filename) takes a filename as string as argument.");


	remove(path);

	
	return 0;
}

static int lua_rename(lua_State *L)
{
	const char *oldName = luaL_checkstring(L, 1);
	const char *newName = luaL_checkstring(L, 2);
	if(!oldName || !newName)
		return luaL_error(L, "Argument error: System.rename(source, destination) takes two filenames as strings as arguments.");

	printf("LuaPlayer : remove_dir not implemented!\n");
	
	return 0;
}



static char modulePath[256];

static void setModulePath()
{
	getcwd( modulePath, 256 );
}

static int lua_loadModule(lua_State *L)
{
	
	printf("LuaPlayer : loadModule not implemented!\n");
	return 0;
}

static int lua_usbActivate(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
		
	// Usb is always activated
	return 0;
}

static int lua_usbDeactivate(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
        
        // ---
	return 0;
}

// battery functions
 
static int lua_powerIsPowerOnline(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	lua_pushboolean(L, 1);
	return 1;
}

static int lua_powerIsBatteryExist(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	// no battery on the PS2
	lua_pushboolean(L, 0);
	return 1;
}

static int lua_powerIsBatteryCharging(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	lua_pushboolean(L, 0);
	return 1;
}

static int lua_powerGetBatteryChargingStatus(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	lua_pushnumber(L, 0);
	return 1;
}

static int lua_powerIsLowBattery(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	lua_pushboolean(L, 0);
	return 1;
}

static int lua_powerGetBatteryLifePercent(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	lua_pushnumber(L, 0);
	return 1;
}

static int lua_powerGetBatteryLifeTime(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	lua_pushnumber(L, 0);
	return 1;
}

static int lua_powerGetBatteryTemp(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	lua_pushnumber(L, 0);
	return 1;
}

static int lua_powerGetBatteryVolt(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	lua_pushnumber(L, 0);
	return 1;
}

static int lua_powerTick(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "wrong number of arguments");
	// do nothing :)
	return 0;
}

static int lua_md5sum(lua_State *L)
{
	size_t size;
	const char *string = luaL_checklstring(L, 1, &size);
	if (!string) return luaL_error(L, "Argument error: System.md5sum(string) takes a string as argument.");

	int i;
	char result[33];        
	u8 digest[16];

	MD5_CTX ctx;
        MD5Init( &ctx );
        MD5Update( &ctx, (u8 *)string, size );
        MD5Final( digest, &ctx );

	for (i = 0; i < 16; i++) sprintf(result + 2 * i, "%02x", digest[i]);
	lua_pushstring(L, result);
	
	return 1;
}



static int lua_sleep(lua_State *L)
{
	if (lua_gettop(L) != 1) return luaL_error(L, "milliseconds expected.");
	//int milliseconds = luaL_checkint(L, 1);
	//sceKernelDelayThread(milliseconds * 1000);
	printf("LuaPlayer : sleep not implemented!\n");
	return 0;
}

static int lua_getFreeMemory(lua_State *L)
{
	if (lua_gettop(L) != 0) return luaL_error(L, "no arguments expected.");
	
	// for a realistic number, try to allocate 1 mb blocks until out of memory
	void* buf[64];
	int i = 0;
	for (i = 0; i < 64; i++) {
		buf[i] = malloc(1024 * 1024);
		if (!buf[i]) break;
	}
	int result = i;
	for (; i >= 0; i--) {
		free(buf[i]);
	}
	lua_pushnumber(L, result * 1024 * 1024);
	return 1;
}

static const luaL_reg System_functions[] = {
  {"currentDirectory",              lua_curdir},
  {"listDirectory",           	    lua_dir},
  {"createDirectory",               lua_createDir},
  {"removeDirectory",               lua_removeDir},
  {"removeFile",                    lua_removeFile},
  {"rename",                        lua_rename},
  {"usbDiskModeActivate",           lua_usbActivate},
  {"usbDiskModeDeactivate",    	    lua_usbDeactivate},
  {"powerIsPowerOnline",            lua_powerIsPowerOnline},
  {"powerIsBatteryExist",           lua_powerIsBatteryExist},
  {"powerIsBatteryCharging",        lua_powerIsBatteryCharging},
  {"powerGetBatteryChargingStatus", lua_powerGetBatteryChargingStatus},
  {"powerIsLowBattery",             lua_powerIsLowBattery},
  {"powerGetBatteryLifePercent",    lua_powerGetBatteryLifePercent},
  {"powerGetBatteryLifeTime",       lua_powerGetBatteryLifeTime},
  {"powerGetBatteryTemp",           lua_powerGetBatteryTemp},
  {"powerGetBatteryVolt",           lua_powerGetBatteryVolt},
  {"powerTick",                     lua_powerTick},
  {"md5sum",                        lua_md5sum},
  {"sleep",                         lua_sleep},
  {"getFreeMemory",                 lua_getFreeMemory},
  {0, 0}
};
void luaSystem_init(lua_State *L) {
	setModulePath();
	luaL_openlib(L, "System", System_functions, 0);
	lua_register(L, "loadlib", lua_loadModule);
}

