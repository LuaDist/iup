/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


 
static int Image (lua_State * L)
{
  int w, h, c, num_colors;
  unsigned char *pixels;
  Ihandle* ih;
  char str[20];

  if (lua_istable(L, 1))
  {
    int i, j;

    /* get the number of lines */
    h = iuplua_getn(L, 1);  

    /* get the number of columns of the first line */
    lua_pushinteger(L, 1);
    lua_gettable(L, 1);
    w = iuplua_getn(L, -1);  
    lua_pop(L, 1);
    
    pixels = (unsigned char *) malloc (h*w);

    for (i=1; i<=h; i++)
    {
      lua_pushinteger(L, i);
      lua_gettable(L, 1);
      for (j=1; j<=w; j++)
      {
        int idx = (i-1)*w+(j-1);
        lua_pushinteger(L, j);
        lua_gettable(L, -2);
        pixels[idx] = (unsigned char)lua_tointeger(L, -1);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    }
    
    ih = IupImage(w,h,pixels);  
    free(pixels);

    num_colors = iuplua_getn(L, 2);
    num_colors = num_colors>255? 255: num_colors;
    for(c=1; c<=num_colors; c++)
    {
      lua_rawgeti(L, 2, c);
      sprintf(str, "%d", c);
      IupStoreAttribute(ih, str, lua_tostring(L,-1));
      lua_pop(L, 1);
    }
  }
  else
  {
    w = luaL_checkint(L, 1);
    h = luaL_checkint(L, 2);
    pixels = iuplua_checkuchar_array(L, 3, w*h);
    ih = IupImage(w, h, pixels);
    free(pixels);

    num_colors = iuplua_getn(L, 4);
    num_colors = num_colors>256? 256: num_colors;
    for(c=1; c<=num_colors; c++)
    {
      lua_rawgeti(L, 4, c);
      sprintf(str, "%d", c-1);
      IupStoreAttribute(ih, str, lua_tostring(L,-1));
      lua_pop(L, 1);
    }
  }

  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
} 
 
int iupimagelua_open(lua_State * L)
{
  iuplua_register(L, Image, "Image");


#ifdef IUPLUA_USELOH
#include "image.loh"
#else
#ifdef IUPLUA_USELH
#include "image.lh"
#else
  iuplua_dofile(L, "image.lua");
#endif
#endif

  return 0;
}

