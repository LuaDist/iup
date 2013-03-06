/* Alternative initialization when statically linking IUP and Lua.
   Usually Iup initialization functions are not necessary because 
   they are called from inside the IupLua initialization functions.  */

#include <stdlib.h>
#include <stdio.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <iup.h>
//#include <iupcontrols.h>

#include <iuplua.h>
//#include <iupluacontrols.h>


int main(int argc, char **argv)
{
  lua_State *L;

  IupOpen(&argc, &argv);
//  IupControlsOpen();

  /* Lua 5 initialization */
  L = lua_open();   
  luaL_openlibs(L);

  iuplua_open(L);      /* Initialize Binding Lua */
//  iupcontrolslua_open(L); /* Initialize the additional controls binding Lua */

  /* do other things, like running a lua script */
  luaL_dofile(L, "lua_init.lua");

//  IupMainLoop(); /* could be here or inside "myfile.lua" */

  lua_close(L);

  IupClose();

  return EXIT_SUCCESS;

}
