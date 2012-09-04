/** \file
 * \brief Creates a Lua executable linked to all standard IUP libraries
 * If user does not provide arguments environment variable 
 * "console3.lua" will be searched. And if fails
 * internal Lua code will be executed to provide a basic interface
 * for the user.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"   
#include "luadebug.h"

#include "iup.h"
#include "iupkey.h"
#include "iuplua.h"

#ifndef IUPLUA_NO_GL
#include "iupgl.h"
#include "iupluagl.h"
#endif

#ifndef IUPLUA_NO_CD
#include "iupcontrols.h"
#include "iupluacontrols.h"
#include "iup_pplot.h"
#include "iuplua_pplot.h"
#include <cd.h>
#include <cdlua.h>
#include <cdluaiup.h>
#endif

#ifndef IUPLUA_NO_IM
#include "iupluaim.h"
#include <imlua.h>
#endif

int main ( int argc, char **argv )
{
  IupOpen(&argc, &argv);
#ifndef IUPLUA_NO_GL
  IupGLCanvasOpen(); 
#endif
#ifndef IUPLUA_NO_CD
  IupControlsOpen();
  IupPPlotOpen();
#endif

  lua_open();

  lua_setdebug(1);

  lua_iolibopen( );
  lua_strlibopen( );
  lua_mathlibopen( );  
  
  iuplua_open( );
  iupkey_open( );
#ifndef IUPLUA_NO_GL
  iupgllua_open();
#endif
#ifndef IUPLUA_NO_CD
  iupcontrolslua_open();
  iup_pplotlua_open();
  cdlua_open();
  cdluaiup_open();
#endif
#ifndef IUPLUA_NO_IM
  iupimlua_open();
  imlua_open();
#endif

  if (argc <= 1) 
  {
    if(!iuplua_dofile("console3.lua"))
    {
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh3/console3_be64.loh"
#else
#include "loh3/console3_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh3/console3_le64w.loh"
#else
#include "loh3/console3_le64.loh"
#endif  
#else
#include "loh3/console3.loh"
#endif  
#endif  
    }
  }
  else
  {
    int ok = 1,
        i  = 1;

    /* Running all .lua given as arguments */
    while(ok & (i < argc))
    {
      ok = iuplua_dofile(argv[i]);
      i++;
    }

    if(!ok)
    {
      return EXIT_FAILURE;
    }
  }

#ifndef IUPLUA_NO_CD
  cdlua_close();
  IupControlsClose();
#endif
  IupClose();
  
  lua_close();
  
  return EXIT_SUCCESS;
}
