/********************************************************************
 Example IupGLCanvas in C 
 Creates a OpenGL canvas and draws a line in it.
 *** by  Mark Stroetzel Glasberg, March 2001 ***
********************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "iup.h"          
#include "iupgl.h"
#include "iupkey.h"

#ifdef WIN32
  #include <windows.h>
#endif
#include "GL/gl.h" 

int k_any(Ihandle *self, int c)
{
  if(c == K_q)
    return IUP_CLOSE;
  else
    return IUP_DEFAULT;
}

int redraw(Ihandle *self, float x, float y)
{
  int w, h;
  char *size = IupGetAttribute(self, "RASTERSIZE");
  sscanf(size, "%dx%d", &w, &h);

  IupGLMakeCurrent(self);
  
  glViewport(0, 0, w, h);
  glClearColor(1.0, 1.0, 1.0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0,0.0,0.0);
  glBegin(GL_QUADS); 
    glVertex2f( 0.9f,  0.9f); 
    glVertex2f( 0.9f, -0.9f); 
    glVertex2f(-0.9f, -0.9f); 
    glVertex2f(-0.9f,  0.9f); 
  glEnd();

  IupGLSwapBuffers(self); 

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *canvas, *finale, *dg;

  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  canvas = IupGLCanvas(NULL);
  IupSetCallback(canvas, "ACTION", (Icallback) redraw);
  IupSetAttribute(canvas, IUP_BUFFER, IUP_DOUBLE);
  IupSetAttribute(canvas, "RASTERSIZE", "123x200");

  finale = IupHbox(IupFill(), 
                   canvas, 
                   IupFill(), 
                   NULL);

  dg = IupDialog(finale);
  IupSetAttribute(dg, "TITLE", "IupGLCanvas");

  IupShow(dg);
  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;
}