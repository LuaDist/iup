#ifdef USE_OPENGL
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <stdlib.h>
#include <stdio.h>

#include "iup.h"          
#include "iupgl.h"


static int map_cb(Ihandle *ih)
{
  printf("MAP_CB()\n");
  return IUP_DEFAULT;
}

static int resize_cb(Ihandle *ih, int w, int h)
{
  printf("RESIZE_CB(%d, %d) RASTERSIZE=%s CLIENTSIZE=%s\n", w, h, IupGetAttribute(ih, "RASTERSIZE"), IupGetAttribute(ih, "CLIENTSIZE"));
  return IUP_DEFAULT;
}

static int button_cb(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
  printf("BUTTON_CB(but=%c (pressed=%d), x=%d, y=%d [%s])\n",(char)but,pressed,x,y, status);
  return IUP_DEFAULT;
}

static int action(Ihandle *ih)
{
  IupGLMakeCurrent(ih);

  glClearColor(1.0, 0.0, 1.0, 1.f);  /* pink */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0,0.0,0.0);  /* red */
  glBegin(GL_QUADS); 
  glVertex2f(0.9f,0.9f); 
  glVertex2f(0.9f,-0.9f); 
  glVertex2f(-0.9f,-0.9f); 
  glVertex2f(-0.9f,0.9f); 
  glEnd();

  IupGLSwapBuffers(ih); 

  return IUP_DEFAULT;
}

void GLCanvasTest(void)
{
  Ihandle *dlg, *canvas, *box;

  IupGLCanvasOpen();

  box = IupVbox(NULL);
  IupSetAttribute(box, "MARGIN", "5x5");

  canvas = IupGLCanvas(NULL);
  IupSetCallback(canvas, "ACTION", action);
  IupSetAttribute(canvas, "BUFFER", "DOUBLE");
  IupSetAttribute(canvas, "BORDER", "NO");
  IupSetAttribute(canvas, "RASTERSIZE", "300x200");
  IupSetCallback(canvas, "BUTTON_CB",    (Icallback)button_cb);
  IupSetCallback(canvas, "RESIZE_CB",    (Icallback)resize_cb);
  IupSetCallback(canvas, "MAP_CB",       (Icallback)map_cb);
  IupAppend(box, canvas);

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupGLCanvas Test");

printf("IupMap\n");
  IupMap(dlg);
  IupGLMakeCurrent(canvas);
  printf("Vendor: %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version: %s\n", glGetString(GL_VERSION));
  IupMap(dlg);

printf("IupShow\n");
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  GLCanvasTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
#endif
