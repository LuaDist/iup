#ifdef USE_OPENGL
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "iup.h"          
#include "iupgl.h"


typedef struct _color3D
{
  double r;
  double g;
  double b;
} color3D;

static void add_color_vertex(const color3D* c)
{
  glColor3d(c->r, c->g, c->b);
  glVertex3d(c->r, c->g, c->b);
}

static void draw_cube_face(const color3D* c1, const color3D* c2, const color3D* c3, const color3D* c4)
{
  color3D cm;
  cm.r = (c1->r + c2->r + c3->r + c4->r)/4;
  cm.g = (c1->g + c2->g + c3->g + c4->g)/4;
  cm.b = (c1->b + c2->b + c3->b + c4->b)/4;

  add_color_vertex(&cm);
  add_color_vertex(c1);
  add_color_vertex(c2);

  add_color_vertex(&cm);
  add_color_vertex(c2);
  add_color_vertex(c3);

  add_color_vertex(&cm);
  add_color_vertex(c3);
  add_color_vertex(c4);

  add_color_vertex(&cm);
  add_color_vertex(c4);
  add_color_vertex(c1);
}

const color3D c3D_black = {0.0, 0.0, 0.0};
const color3D c3D_red = {1.0, 0.0, 0.0};
const color3D c3D_green = {0.0, 1.0, 0.0};
const color3D c3D_blue = {0.0, 0.0, 1.0};
const color3D c3D_cian = {0.0, 1.0, 1.0};
const color3D c3D_magenta = {1.0, 0.0, 1.0};
const color3D c3D_yellow = {1.0, 1.0, 0.0};
const color3D c3D_white = {1.0, 1.0, 1.0};

static void draw_cube(void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBegin(GL_TRIANGLES);

  draw_cube_face(&c3D_black, &c3D_blue, &c3D_magenta, &c3D_red);
  draw_cube_face(&c3D_black, &c3D_green, &c3D_cian, &c3D_blue);
  draw_cube_face(&c3D_black, &c3D_red, &c3D_yellow, &c3D_green);
  draw_cube_face(&c3D_white, &c3D_cian, &c3D_blue, &c3D_magenta);
  draw_cube_face(&c3D_white, &c3D_cian, &c3D_green, &c3D_yellow);
  draw_cube_face(&c3D_white, &c3D_magenta, &c3D_red, &c3D_yellow);

  glEnd();
}

static void init(void)
{
  glClearColor(0.5,0.5,0.5,0.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, 1, 1, 10);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt (0.5, 0.5, 3.0,  /* posicao do observador */
            0.5, 0.5, 0.0,  /* ponto de referencia   */
            0.0, 1.0, 0.0); /* vup                   */
  glEnable(GL_DEPTH_TEST);
  glClearDepth (1.0);
}

static void unproject (double x2, double y2, double *x3, double *y3, double *z3)
{
  double mv[16];
  double pm[16];
  int    vp[4];

  glGetDoublev (GL_MODELVIEW_MATRIX,  mv);
  glGetDoublev (GL_PROJECTION_MATRIX, pm);
  glGetIntegerv (GL_VIEWPORT, vp);
  gluUnProject (x2, y2, 0.0,
                mv, pm, vp,
                x3, y3, z3);
}

static int pos_x, pos_y;
static int move = 0;

static int button_cb(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
  (void)ih;
  (void)status;

  if (but==IUP_BUTTON1)
  {
    if (pressed)
    {
      pos_x = x;
      pos_y = y;
      move = 1;
    }
    else
    {
      move = 0;
    }
  }
  return IUP_DEFAULT;
}

#define INVERT_Y(_y) (height-y)

static int motion_cb(Ihandle *ih,int x,int y,char* status)
{
  (void)status;

  if (move)
  {
    double dif_x, dif_y;
    double dx, dy, dz;
    double x1, y1, z1;
    double x2, y2, z2;
    double angle, norma;
    int height = IupGetInt2(ih, "RASTERSIZE");

    IupGLMakeCurrent(ih);

    dif_x = x - pos_x;
    dif_y = y - pos_y;

    pos_x = x;
    pos_y = y;

    angle = sqrt(dif_x*dif_x + dif_y*dif_y);

    unproject (pos_x, INVERT_Y(pos_y), &x1, &y1, &z1);
    unproject ((double)(dif_y+pos_x), (double)(dif_x+INVERT_Y(pos_y)), &x2, &y2, &z2);
    dx = x2-x1; dy = y2-y1; dz = z2-z1;
    norma = sqrt(dx*dx + dy*dy + dz*dz);
    dx /= norma; dy /= norma; dz /= norma;

    glTranslated(0.5, 0.5, 0.5);
    glRotated (angle, dx, dy, dz);
    glTranslated(-0.5, -0.5, -0.5);

    draw_cube();
  
    IupGLSwapBuffers(ih); 
  }
  return IUP_DEFAULT;
}

static int action(Ihandle *ih)
{
  IupGLMakeCurrent(ih);

  glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  draw_cube();

  IupGLSwapBuffers(ih); 

  return IUP_DEFAULT;
}

void GLCanvasCubeTest(void)
{
  Ihandle *dlg, *canvas, *box;

  IupGLCanvasOpen();

  box = IupVbox(NULL);
  IupSetAttribute(box, "MARGIN", "5x5");

  canvas = IupGLCanvas(NULL);
  IupSetCallback(canvas, "ACTION", action);
  IupSetCallback(canvas, "BUTTON_CB", (Icallback)button_cb);
  IupSetCallback(canvas, "MOTION_CB", (Icallback)motion_cb);
  IupSetAttribute(canvas, "BUFFER", "DOUBLE");
  IupSetAttribute(canvas, "RASTERSIZE", "300x300");
  IupAppend(box, canvas);

  dlg = IupDialog(IupSetAttributes(IupFrame(box), "TITLE=Teste"));
  IupSetAttribute(dlg, "TITLE", "IupGLCanvas Test");
//  IupSetAttribute(dlg, "COMPOSITED", "YES");

  IupMap(dlg);

  IupGLMakeCurrent(canvas);
  init();
  printf("Vendor: %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version: %s\n", glGetString(GL_VERSION));
  IupSetAttribute(canvas, "RASTERSIZE", NULL);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  GLCanvasCubeTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
#endif
