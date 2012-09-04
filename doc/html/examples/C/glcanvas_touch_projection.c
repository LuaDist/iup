/*
* IupGLCanvas PROJECTION Sample
* Description : Creates a IUP GL canvas and uses OpenGL to draw on it
*               IUP TUIO multi-touch sample (PROJECTION = Zoom, Pan and Rotate)
*               The texture is loaded using Drag/Drop operation and IUP IM function
*      Remark : IUP must be linked to the IM library
*  TODO: both TUIO+OpenGL examples need some work so they can be used in real applications.
*        Depending on the interaction device the movements, rotations and scales can be very noisy.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "iup.h"          
#include "iupgl.h"
#include "iupim.h"

#ifdef WIN32
#include <windows.h>
#endif
#include "GL/gl.h" 
#include "GL/glu.h"

#define USE_TUIO
#ifdef USE_TUIO
#include "iuptuio.h"
#endif

double window = 50.0;
double left, right, bottom, top;

double moveX = 0, moveY = 0;
double rotationAngle = 0, rotateVal = 0;
double zoomFactor = 1, scaleVal = 1;
double translateX = 0, translateY = 0;

GLuint textureID;
Ihandle* image;
unsigned char* data;

void LoadTexture()
{
  int width  = IupGetInt(image, "WIDTH");
  int height = IupGetInt(image, "HEIGHT");

  data = (unsigned char*)IupGetAttribute(image, "WID");

  // allocate a texture name
  glGenTextures(1, &textureID );

  // select our current texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  // select modulate to mix texture with color for shading
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // select filter for the small areas
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  // select filter for the large areas
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

  glEnable(GL_TEXTURE_2D);
}

void FreeTexture()
{
  glDisable(GL_TEXTURE_2D);
  glDeleteTextures(1, &textureID);
}

void reshape(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  left = bottom = -window;
  right  =  top =  window;

  if (w > h)
  {
    left  = -window*w/h;
    right =  window*w/h;
  }
  else
  {
    bottom = -window*h/w;
    top    =  window*h/w;
  }

  gluOrtho2D(left, right, bottom, top);

  glTranslated(moveX+translateX, moveY+translateY, 0.0);
  glRotated(rotateVal+rotationAngle, 0.0, 0.0, 1.0);
  glScaled(scaleVal*zoomFactor, scaleVal*zoomFactor, 1.0);

  glMatrixMode(GL_MODELVIEW);
}

int drop_image(Ihandle *self, const char* filename, int num, int x, int y)
{
  image = IupLoadImage(filename);
  if (!image)
  {
    char* err_msg = IupGetGlobal("IUPIM_LASTERROR");
    if (err_msg)
      IupMessage("Error", err_msg);

    IupRedraw(self, 0);
    return IUP_CLOSE;
  }

  IupRedraw(self, 0);

  (void)x;
  (void)y;
  (void)num;

  return IUP_DEFAULT;
}

int k_any(Ihandle *self, int key)
{
  if(key == K_ESC)
    return IUP_CLOSE;

  switch(key)
  {
  case K_UP:     // moving the window to up
    moveY += 1.0;
    break;
  case K_DOWN:   // moving the window to down
    moveY -= 1.0;
    break;
  case K_LEFT:   // moving the window to the left
    moveX -= 1.0;
    break;
  case K_RIGHT:  // moving the window to the right
    moveX += 1.0;
    break;
  case K_PGUP:
    zoomFactor += 0.1; // zoom in
    break;
  case K_PGDN:
    zoomFactor -= 0.1; // zoom out
    if(zoomFactor < 0)
      zoomFactor = 0;
    break;
  case K_HOME:
    rotationAngle -= 1.0; // rotate clockwise
    break;
  case K_END:
    rotationAngle += 1.0; // rotate counter-clockwise
    break;
  }

  IupRedraw(self, 0);
  return IUP_DEFAULT;
}

int redraw(Ihandle *self, float x, float y)
{
  int w, h;
  char *size = IupGetAttribute(self, "RASTERSIZE");
  sscanf(size, "%dx%d", &w, &h);

  IupGLMakeCurrent(self);

  glViewport(0, 0, w, h);
  reshape(w, h);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  LoadTexture();

  glPushMatrix();
  glBegin( GL_QUADS );
  glTexCoord2d(0.0, 0.0); glVertex2d(-10.0,  10.0);
  glTexCoord2d(1.0, 0.0); glVertex2d( 10.0,  10.0);
  glTexCoord2d(1.0, 1.0); glVertex2d( 10.0, -10.0);
  glTexCoord2d(0.0, 1.0); glVertex2d(-10.0, -10.0);
  glEnd();
  glPopMatrix();

  FreeTexture();

  IupGLSwapBuffers(self); 

  (void)x;
  (void)y;

  return IUP_DEFAULT;
}

///////////////////////////////////////////////////////////////////////////////
#ifdef USE_TUIO

#define GRAD_PI               57.2957787  /* 180.0f / 3.1415927f */
#define MAX_ANGLE_VARIANCE    10.0
#define INVERT_Y_AXIS(_y, _h) (_h - _y - 1)

int oldX = 0, oldY = 0;
double initDist = 0, initAngle = 0;

// Calculates the angle of this vector in grad in the trigonometric sense.
// Source: touchlib (http://www.nuigroup.com/touchlib/)
double getAngleTrig(double X, double Y)
{
  if(X == 0.0)
    return Y < 0.0 ? 270.0 : 90.0;
  else
    if(Y == 0)
      return X < 0.0 ? 180.0 : 0.0;

  if(Y > 0.0)
    if(X > 0.0)
      return atan(Y/X) * GRAD_PI;
    else
      return 180.0-atan(Y/-X) * GRAD_PI;
  else
    if(X > 0.0)
      return 360.0-atan(-Y/X) * GRAD_PI;
    else
      return 180.0+atan(-Y/-X) * GRAD_PI;
}

/*
Zoom, Move and Rotate gestures were mapped using this link as reference:
http://wiki.nuigroup.com/Gesture_recognition
*/
int multitouch_cb(Ihandle *self, int count, int* id, int* px, int* py, int *pstate)
{
  int w, h, fullw, fullh;
  char *size = IupGetAttribute(self, "RASTERSIZE");
  char *full = IupGetGlobal("FULLSIZE");
  sscanf(size, "%dx%d", &w, &h);
  sscanf(full, "%dx%d", &fullw, &fullh);

  /* if self is the canvas, then must convert to screen coordinates */
  px[0] += IupGetInt(self, "X");
  py[0] += IupGetInt(self, "Y");

  /* Now, convert to the GL window coordinates */
  px[0] = (int)(px[0]*(right-left)/fullw - right);
  py[0] = (int)(py[0]*(top-bottom)/fullh - top);

  // DOWN - first blob on surface
  if(pstate[0] == 'D')
  {
    oldX = px[0];
    oldY = INVERT_Y_AXIS(py[0], h);
  }

  // DOWN - second blob on surface
  if(count == 2)
  {
    /* if self is the canvas, then must convert to screen coordinates */
    px[1] += IupGetInt(self, "X");
    py[1] += IupGetInt(self, "Y");

    /* Now, convert to the GL window coordinates */
    px[1] = (int)(px[1]*(right-left)/fullw - right);
    py[1] = (int)(py[1]*(top-bottom)/fullh - top);

    if(pstate[1] == 'D')
    {
      int initDiffX;
      int initDiffY;

      initDiffX = px[1] - oldX;
      initDiffY = INVERT_Y_AXIS(py[1], h) - oldY;
      initAngle = getAngleTrig(initDiffX, initDiffY);
      initDist  = sqrt(pow((double)initDiffX, 2) + pow((double)initDiffY, 2));
    }
  }

  // MOVE - there is one blob on surface, and it is moving
  if(count == 1 && pstate[0] == 'M')
  {
    translateX += (double)(px[0] - oldX);                    //  PAN GESTURE
    translateY += (double)(INVERT_Y_AXIS(py[0], h) - oldY);  //  (translate XY)

    oldX = px[0];
    oldY = INVERT_Y_AXIS(py[0], h);
  }
  // MOVE - there are two blobs on surface, and they are moving
  else if(count == 2)
  {
    if(pstate[0] == 'M' && pstate[1] == 'M')
    {
      int curDiffX = px[1]-px[0];
      int curDiffY = INVERT_Y_AXIS(py[1], h) - INVERT_Y_AXIS(py[0], h);
      double curAngle = getAngleTrig((double)curDiffX, (double)curDiffY);
      double curDist = sqrt(pow((double)curDiffX, 2) + pow((double)curDiffY, 2));

      if(abs(curAngle-initAngle) > MAX_ANGLE_VARIANCE)
      {
        // ROTATE GESTURE (rotate angle)
        rotationAngle = curAngle-initAngle;
      }
      else
      {
        // ZOOM GESTURE (scale factor)
        if(initDist > 0) // Prevent a divide by zero
          zoomFactor = curDist/initDist;
      }
    }
  }

  // UP - the first blob is out of surface... Now, the second blob becomes the first!
  // So, we must reset the initial values
  if(pstate[0] == 'U')
  {
    if(count == 2)
    {
      oldX = px[1];
      oldY = INVERT_Y_AXIS(py[1], h);
    }

    moveX += translateX;
    moveY += translateY;
    translateX = 0;
    translateY = 0;

    initDist = 0;
    initAngle = 0;
  }

  // UP - the second blob is out of surface...
  // So, we must reset the initial values
  if(count == 2)
  {
    if(pstate[1] == 'U')
    {
      initDist = 0;
      initAngle = 0;

      rotateVal += rotationAngle;
      if(abs(rotateVal) > 360)
        rotateVal = (int)rotateVal % 360;
      rotationAngle = 0;

      scaleVal *= zoomFactor;
      if(scaleVal < 0)
        scaleVal = 1;
      zoomFactor = 1;
    }
  }

  // Updating the display
  IupRedraw(self, 0);

  (void)id;
  return IUP_DEFAULT;
}

#endif
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  Ihandle *canvas, *finale, *dlg;

  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  canvas = IupGLCanvas(NULL);
  IupSetCallback(canvas, "ACTION", (Icallback) redraw);
  IupSetCallback(canvas, "K_ANY", (Icallback) k_any);
  IupSetCallback(canvas, "DROPFILES_CB", (Icallback) drop_image);

  IupSetAttribute(canvas, IUP_BUFFER, IUP_DOUBLE);
  IupSetAttribute(canvas, "RASTERSIZE", "450x450");

#ifdef USE_TUIO
  {
    Ihandle *tuio;
    IupTuioOpen();
    tuio = IupTuioClient(0);
    IupSetAttribute(tuio, "CONNECT", "YES");
    IupSetAttributeHandle(tuio, "TARGETCANVAS", canvas);
    IupSetCallback(canvas, "MULTITOUCH_CB",(Icallback)multitouch_cb);
  }
#endif

  finale = IupHbox(IupFill(), 
    canvas, 
    IupFill(), 
    NULL);

  dlg = IupDialog(finale);
  IupSetAttribute(dlg, "TITLE", "IupGLCanvas Multi-touch PROJECTION");
  IupMap(dlg);
  IupGLMakeCurrent(canvas);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;
}
