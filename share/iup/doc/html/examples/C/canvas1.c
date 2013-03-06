/*
 * IupCanvas example
 * Description : Creates a IUP canvas and uses CD to draw on it
 *      Remark : IUP must be linked to the CD library                      
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "cd.h"
#include "cdiup.h"

//#define USE_TUIO
#ifdef USE_TUIO
#include "iuptuio.h"
#endif

static cdCanvas *cdcanvas = NULL;
int draw = 0;

int button_cb(Ihandle* self, int but, int press, int x, int y)
{
  if (but == IUP_BUTTON1 && press)
  {
    cdCanvasUpdateYAxis(cdcanvas, &y);
    cdCanvasPixel(cdcanvas, x, y, CD_BLUE);
    draw = 1;
  }
  else
  {
    cdCanvasClear(cdcanvas);
    draw = 0;
  }
	
  return IUP_DEFAULT;	
}

int motion_cb(Ihandle* self, int x, int y)
{
  if (draw)
  {
    cdCanvasUpdateYAxis(cdcanvas, &y);
    cdCanvasPixel(cdcanvas, x, y, CD_BLUE);
  }

  return IUP_DEFAULT;
}

int redraw_cb( Ihandle *self, float x, float y )
{
  cdCanvasActivate(cdcanvas);
  cdCanvasClear(cdcanvas);
  cdCanvasForeground(cdcanvas, CD_BLUE);
  cdCanvasBox(cdcanvas, 0, 100, 0, 100);
  return IUP_DEFAULT;
}

#ifdef USE_TUIO

int touch_cb(Ihandle *self, int id, int x, int y, char* state)
{
  printf("touch_cb(id=%d x=%d y=%d state=%s)\n", id, x, y, state);

  if (strstr(state, "PRIMARY")!=0)
  {
    static int tap_x = 0;
    static int tap_y = 0;

    /* if self is the canvas, then must convert to screen coordinates */
    x += IupGetInt(self, "X");
    y += IupGetInt(self, "Y");

    /* TODO: Instead of using absolute coordinates, 
             use relative coordinates to simulate a touchpad like in a laptop. */

    /* simulate a cursor */
    IupSetfAttribute(NULL, "CURSORPOS", "%dx%d", x, y);

    if (state[0] == 'D') /* DOWN */
    {
      if (IupGetInt(NULL, "CONTROLKEY")) /* Ctrl pressed */
        IupSetfAttribute(NULL, "MOUSEBUTTON", "%dx%d %c %d", x, y, IUP_BUTTON1, 1);  /* mouse down */

      tap_x = x;
      tap_y = y;
    }
    else if (state[0] == 'U') /* UP */
    {
      if (IupGetInt(NULL, "CONTROLKEY")) /* Ctrl pressed */
        IupSetfAttribute(NULL, "MOUSEBUTTON", "%dx%d %c %d", x, y, IUP_BUTTON1, -1);  /* mouse up */
      else if (abs(tap_x-x)<10 && abs(tap_y-y)<10)
      {
        IupSetfAttribute(NULL, "MOUSEBUTTON", "%dx%d %c %d", x, y, IUP_BUTTON1, 1);  /* mouse down */
        IupSetfAttribute(NULL, "MOUSEBUTTON", "%dx%d %c %d", x, y, IUP_BUTTON1, -1);  /* mouse up */
      }
    }
  }
  return IUP_DEFAULT;
}

int multitouch_cb(Ihandle *self, int count, int* id, int* px, int* py, int *pstate)
{
  int i;
  printf("multitouch_cb(count=%d)\n", count);
  for (i = 0; i < count; i++)
  {
    printf("    id=%d x=%d y=%d state=%c\n", id[i], px[i], py[i], pstate[i]);

    cdCanvasPixel(cdcanvas, px[i], cdCanvasInvertYAxis(cdcanvas, py[i]), CD_RED);
  }
  return IUP_DEFAULT;
}
#else
#ifdef WIN32
int touch_cb(Ihandle *self, int id, int x, int y, char* state)
{
  printf("touch_cb(id=%d x=%d y=%d state=%s)\n", id, x, y, state);
  return IUP_DEFAULT;
}
#endif
#endif

int main(int argc, char **argv)
{
  Ihandle *dlg, *cnvs;
  IupOpen(&argc, &argv);

  cnvs = IupCanvas( NULL );
  IupSetCallback(cnvs, "ACTION",( Icallback )redraw_cb );
  IupSetAttribute(cnvs, "SIZE", "300x100");
  IupSetCallback(cnvs, "BUTTON_CB",(Icallback)button_cb);
  IupSetCallback(cnvs, "MOTION_CB",(Icallback)motion_cb);

#ifdef USE_TUIO
  {
    Ihandle *tuio;
    IupTuioOpen();
    tuio = IupTuioClient(0);
    IupSetAttribute(tuio, "CONNECT", "YES");
//    IupSetAttribute(tuio, "DEBUG", "YES");
//    IupSetCallback(tuio, "TOUCH_CB",(Icallback)touch_cb);
//    IupSetCallback(tuio, "MULTITOUCH_CB",(Icallback)multitouch_cb);
    IupSetAttributeHandle(tuio, "TARGETCANVAS", cnvs);
    IupSetCallback(cnvs, "TOUCH_CB",(Icallback)touch_cb);
    IupSetCallback(cnvs, "MULTITOUCH_CB",(Icallback)multitouch_cb);
  }
#else
#ifdef WIN32
  IupSetAttribute(cnvs, "TOUCH", "YES");
  IupSetCallback(cnvs, "TOUCH_CB",(Icallback)touch_cb);
//  IupSetCallback(cnvs, "MULTITOUCH_CB",(Icallback)multitouch_cb);
#endif
#endif


  dlg = IupDialog( IupVbox( cnvs, NULL ) );
  IupSetAttribute(dlg, "TITLE", "IupCanvas + Canvas Draw" );
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupMap( dlg );
  
  cdcanvas = cdCreateCanvas( CD_IUP, cnvs );
  cdCanvasForeground(cdcanvas, CD_BLUE);
  
  IupShowXY( dlg, IUP_CENTER, IUP_CENTER );
  IupMainLoop();
  IupClose();  
  return EXIT_SUCCESS;
}
