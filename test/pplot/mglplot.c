/*
 * IupMglPlot Test
 * Description : Create all built-in plots.
 *               It is organized as two side-by-side panels:
 *                 - left panel for current plot control
 *                 - right panel containing tabbed plots
 *      Remark : depend on libs IUP, CD, IUP_MGLPLOT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#define USE_IM 1
#ifdef USE_IM
#include "im.h"
#include "im_image.h"
#include "im_convert.h"
#include "im_process.h"
#endif

#include "iup.h"
#ifdef USE_IM
#include "iupim.h"
#endif
#include "iupcontrols.h"
#include "iup_mglplot.h"


#ifdef MGLPLOT_TEST

#define MAXPLOT 6  /* room for examples */


static Ihandle *plot[MAXPLOT] = {NULL}; /* PPlot controls */
static Ihandle *dial1, *dial2,          /* dials for zooming */
        *tgg1, *tgg2,            /* auto scale on|off toggles */
        *tgg3, *tgg4,            /* grid show|hide toggles */
        *tgg5,                   /* legend show|hide toggle */
        *tgg6,                   /* box show|hide toggle */
        *tabs;                   /* tabbed control */

static int delete_cb(Ihandle* ih, int index, int sample_index, float x, float y)
{
  printf("DELETE_CB(%d, %d, %g, %g)\n", index, sample_index, x, y);
  return IUP_DEFAULT;
}

static int select_cb(Ihandle* ih, int index, int sample_index, float x, float y, int select)
{
  printf("SELECT_CB(%d, %d, %g, %g, %d)\n", index, sample_index, x, y, select);
  return IUP_DEFAULT;
}

static int edit_cb(Ihandle* ih, int index, int sample_index, float x, float y, float *new_x, float *new_y)
{
  printf("EDIT_CB(%d, %d, %g, %g, %g, %g)\n", index, sample_index, x, y, *new_x, *new_y);
  return IUP_DEFAULT;
}

static int postdraw_cb(Ihandle* ih)
{
  IupMglPlotDrawText(ih, "My Inline Legend", 0.003f, 0.02f, 0);
  printf("POSTDRAW_CB()\n");
  return IUP_DEFAULT;
}

static int predraw_cb(Ihandle* ih)
{
  printf("PREDRAW_CB()\n");
  return IUP_DEFAULT;
}

static void InitPlots(void)
{
  int theI, ds_index, count;
  float x, y, theFac;
  float px[210], py[210];

  /* PLOT 0 - MakeExamplePlot1 */
  IupSetAttribute(plot[0], "TITLE", "AutoScale");
//  IupSetAttribute(plot[0], "TITLEFONTSIZE", "16");
  IupSetAttribute(plot[0], "LEGEND", "YES");
//  IupSetAttribute(plot[0], "ALPHA", "NO");
//  IupSetAttribute(plot[0], "ANTIALIAS", "NO");
//  IupSetAttribute(plot[0], "FONT", "Helvetica, 14");
//  IupSetAttribute(plot[0], "FONT", "Courier, 11");
//  IupSetAttribute(plot[0], "FONT", "Times, 8");
//  IupSetAttribute(plot[0], "FONT", "courbd, 8");
  IupSetAttribute(plot[0], "FONT", "texgyrecursor-bold, 8");
//  IupSetAttribute(plot[0], "FONT", "cursor, Bold 8");

  IupSetAttribute(plot[0], "AXS_XLABEL", "gnu (Foo)");
  IupSetAttribute(plot[0], "AXS_YLABEL", "Space (m^3)");
//   IupSetAttribute(plot[0], "AXS_XTICKFONTSIZE", "8");
//   IupSetAttribute(plot[0], "AXS_YTICKFONTSIZE", "8");
//  IupSetAttribute(plot[0], "AXS_XFONTSIZE", "10");
//  IupSetAttribute(plot[0], "AXS_YFONTSIZE", "10");
  IupSetAttribute(plot[0], "AXS_XLABELCENTERED", "NO");
  IupSetAttribute(plot[0], "AXS_YLABELCENTERED", "NO");
  IupSetAttribute(plot[0], "AXS_YLABELROTATION", "NO");
  IupSetAttribute(plot[0], "AXS_YTICKVALUESROTATION", "NO");
//  IupSetAttribute(plot[0], "AXS_XTICKVALUESROTATION", "NO");
  IupSetAttribute(plot[0], "AXS_XCROSSORIGIN", "Yes");
  IupSetAttribute(plot[0], "AXS_YCROSSORIGIN", "Yes");

  theFac = (float)1.0/(100*100*100);
  IupMglPlotBegin(plot[0], 2);
  for (theI=-100; theI<=100; theI++) 
  {
    x = (float)(theI+50);
    y = theFac*theI*theI*theI;
    IupMglPlotAdd2D(plot[0], x, y);
  }
  IupMglPlotEnd(plot[0]);
  IupSetAttribute(plot[0], "DS_LINEWIDTH", "3");
  IupSetAttribute(plot[0], "DS_LEGEND", "Line");
  IupSetAttribute(plot[0], "FGCOLOR", "0 128 255");

  theFac = (float)2.0/100;
  ds_index = IupMglPlotNewDataSet(plot[0], 2);
  count = 0;
  for (theI=-100; theI<=100; theI++) 
  {
    x = (float)theI;
    y = -theFac*theI;
    px[theI+100] = x;
    py[theI+100] = y;
    count++;
  }
  IupMglPlotSet2D(plot[0], ds_index, px, py, count);
  IupSetAttribute(plot[0], "DS_LEGEND", "Curve 1");

  IupMglPlotBegin(plot[0], 2);
  for (theI=-100; theI<=100; theI++) 
  {
    x = (float)(0.01*theI*theI-30);
    y = (float)0.01*theI;
    IupMglPlotAdd2D(plot[0], x, y);
  }
  IupMglPlotEnd(plot[0]);
  IupSetAttribute(plot[0], "DS_LEGEND", "Curve 2");


  /* PLOT 1 - MakeExamplePlot2 */
//  IupSetAttribute(plot[1], "TITLE", "No Autoscale+No CrossOrigin");
  IupSetAttribute(plot[1], "BGCOLOR", "0 192 192");
  IupSetAttribute(plot[1], "AXS_XLABEL", "Tg (X)");
  IupSetAttribute(plot[1], "AXS_YLABEL", "Tg (Y)");
  IupSetAttribute(plot[1], "AXS_XAUTOMIN", "NO");
  IupSetAttribute(plot[1], "AXS_XAUTOMAX", "NO");
  IupSetAttribute(plot[1], "AXS_YAUTOMIN", "NO");
  IupSetAttribute(plot[1], "AXS_YAUTOMAX", "NO");
  IupSetAttribute(plot[1], "AXS_XMIN", "10");
  IupSetAttribute(plot[1], "AXS_XMAX", "60");
  IupSetAttribute(plot[1], "AXS_YMIN", "-0.5");
  IupSetAttribute(plot[1], "AXS_YMAX", "0.5");
  IupSetAttribute(plot[1], "AXS_XFONTSTYLE", "ITALIC");
  IupSetAttribute(plot[1], "AXS_YFONTSTYLE", "BOLD");
  IupSetAttribute(plot[1], "AXS_XREVERSE", "YES");
  IupSetAttribute(plot[1], "GRIDCOLOR", "128 255 128");
  IupSetAttribute(plot[1], "GRIDLINESTYLE", "DOTTED");
  IupSetAttribute(plot[1], "GRID", "YES");
  IupSetAttribute(plot[1], "BOX", "YES");
  IupSetAttribute(plot[1], "LEGEND", "YES");
  IupSetAttribute(plot[1], "AXS_YTICKVALUESROTATION", "NO");

  theFac = (float)1.0/(100*100*100);
  IupMglPlotBegin(plot[1], 2);
  for (theI=0; theI<=100; theI++) 
  {
    x = (float)(theI);
    y = theFac*theI*theI*theI;
    IupMglPlotAdd2D(plot[1], x, y);
  }
  IupMglPlotEnd(plot[1]);

  theFac = (float)2.0/100;
  IupMglPlotBegin(plot[1], 2);
  for (theI=0; theI<=100; theI++) 
  {
    x = (float)(theI);
    y = -theFac*theI;
    IupMglPlotAdd2D(plot[1], x, y);
  }
  IupMglPlotEnd(plot[1]);

  /* PLOT 2 - MakeExamplePlot4 */
  IupSetAttribute(plot[2], "TITLE", "Log Scale");
  IupSetAttribute(plot[2], "GRID", "YES");
  IupSetAttribute(plot[2], "BOX", "YES");
  IupSetAttribute(plot[2], "BOXTICKS", "NO");
  IupSetAttribute(plot[2], "AXS_XSCALE", "LOG10");
  IupSetAttribute(plot[2], "AXS_YSCALE", "LOG10");
//  IupSetAttribute(plot[2], "AXS_YSCALE", "LOG2");  // NOT supported yet
  IupSetAttribute(plot[2], "AXS_XLABEL", "Tg (X)");
  IupSetAttribute(plot[2], "AXS_YLABEL", "Tg (Y)");
  IupSetAttribute(plot[2], "AXS_XFONTSTYLE", "BOLD");
//  IupSetAttribute(plot[2], "AXS_YFONTSTYLE", "BOLD");
  IupSetAttribute(plot[2], "AXS_XLABELCENTERED", "NO");
  IupSetAttribute(plot[2], "AXS_YLABELCENTERED", "NO");
//  IupSetAttribute(plot[2], "AXS_YLABELROTATION", "NO");

  theFac = (float)100.0/(100*100*100);
  IupMglPlotBegin(plot[2], 2);
  for (theI=0; theI<=100; theI++) 
  {
    x = (float)(0.0001+theI*0.001);
    y = (float)(0.01+theFac*theI*theI*theI);
    IupMglPlotAdd2D(plot[2], x, y);
  }
  IupMglPlotEnd(plot[2]);
  IupSetAttribute(plot[2], "DS_COLOR", "100 100 200");
  IupSetAttribute(plot[2], "DS_LINESTYLE", "DOTTED");

  /* PLOT 3 - MakeExamplePlot5 */
  IupSetAttribute(plot[3], "TITLE", "Bar Mode");

  {
    const char * kLables[12] = {"jan","feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
    const float kData[12] = {10,20,30,40,50,60,70,80,90,0,10,20};
    IupMglPlotBegin(plot[3], 1);
    for (theI=0;  theI<12; theI++) 
      IupMglPlotAdd1D(plot[3], kLables[theI], kData[theI]);
  }
  IupMglPlotEnd(plot[3]);
  IupSetAttribute(plot[3], "DS_COLOR", "100 100 200");
  IupSetAttribute(plot[3], "DS_MODE", "BAR");
//  IupSetAttribute(plot[3], "AXS_XTICKAUTO", "NO");   // Not supported when names are used
//  IupSetAttribute(plot[3], "AXS_XTICKMAJORSPAN", "-5"); // idem
  

  /* PLOT 4 - MakeExamplePlot6 */
  IupSetAttribute(plot[4], "TITLE", "Marks Mode");
  IupSetAttribute(plot[4], "AXS_XAUTOMIN", "NO");
  IupSetAttribute(plot[4], "AXS_XAUTOMAX", "NO");
  IupSetAttribute(plot[4], "AXS_YAUTOMIN", "NO");
  IupSetAttribute(plot[4], "AXS_YAUTOMAX", "NO");
  IupSetAttribute(plot[4], "AXS_XMIN", "0");
  IupSetAttribute(plot[4], "AXS_XMAX", "0.011");
  IupSetAttribute(plot[4], "AXS_YMIN", "0");
  IupSetAttribute(plot[4], "AXS_YMAX", "0.22");
  IupSetAttribute(plot[4], "AXS_XTICKFORMAT", "%1.3f");
  IupSetAttribute(plot[4], "LEGEND", "YES");
  IupSetAttribute(plot[4], "LEGENDPOS", "BOTTOMRIGHT");
  IupSetAttribute(plot[4], "AXS_YTICKVALUESROTATION", "NO");
  

  theFac = (float)100.0/(100*100*100);
  IupMglPlotBegin(plot[4], 2);
  for (theI=0; theI<=10; theI++) 
  {
    x = (float)(0.0001+theI*0.001);
    y = (float)(0.01+theFac*theI*theI);
    IupMglPlotAdd2D(plot[4], x, y);
  }
  IupMglPlotEnd(plot[4]);
  IupSetAttribute(plot[4], "DS_MODE", "MARKLINE");
  IupSetAttribute(plot[4], "DS_SHOWVALUES", "YES");

  IupMglPlotBegin(plot[4], 2);
  for (theI=0; theI<=10; theI++) 
  {
    x = (float)(0.0001+theI*0.001);
    y = (float)(0.2-theFac*theI*theI);
    IupMglPlotAdd2D(plot[4], x, y);
  }
  IupMglPlotEnd(plot[4]);
  IupSetAttribute(plot[4], "DS_MODE", "MARK");
  IupSetAttribute(plot[4], "DS_MARKSTYLE", "HOLLOW_CIRCLE");
  
  /* PLOT 5 - MakeExamplePlot8 */
  IupSetAttribute(plot[5], "TITLE", "Data Selection and Editing");
  IupSetAttribute(plot[5], "AXS_XCROSSORIGIN", "Yes");
  IupSetAttribute(plot[5], "AXS_YCROSSORIGIN", "Yes");

  theFac = (float)100.0/(100*100*100);
  IupMglPlotBegin(plot[5], 2);
  for (theI=-10; theI<=10; theI++) 
  {
    x = (float)(0.001*theI);
    y = (float)(0.01+theFac*theI*theI*theI);
    IupMglPlotAdd2D(plot[5], x, y);
  }
  IupMglPlotEnd(plot[5]);
  IupSetAttribute(plot[5], "DS_COLOR", "100 100 200");
  IupSetAttribute(plot[5], "DS_EDIT", "YES");
  IupSetCallback(plot[5], "DELETE_CB", (Icallback)delete_cb);
  IupSetCallback(plot[5], "SELECT_CB", (Icallback)select_cb);
  IupSetCallback(plot[5], "POSTDRAW_CB", (Icallback)postdraw_cb);
  IupSetCallback(plot[5], "PREDRAW_CB", (Icallback)predraw_cb);
  IupSetCallback(plot[5], "EDIT_CB", (Icallback)edit_cb);
}

static int tabs_get_index(void)
{
  Ihandle *curr_tab = IupGetHandle(IupGetAttribute(tabs, "VALUE"));
  char *ss = IupGetAttribute(curr_tab, "TABTITLE");
  ss += 5; /* Skip "Plot " */
  return atoi(ss);
}

/* Some processing required by current tab change: the controls at left
   will be updated according to current plot props */
static int tabs_tabchange_cb(Ihandle* self, Ihandle* new_tab)
{
  int ii=0;

  char *value = IupGetAttribute(new_tab, "TABTITLE");
  ii = atoi(value+5); /* Skip "Plot " */

  /* auto scaling X axis */
  if (IupGetInt(plot[ii], "AXS_XAUTOMIN") && IupGetInt(plot[ii], "AXS_XAUTOMAX")) 
  {
    IupSetAttribute(tgg2, "VALUE", "ON");
    IupSetAttribute(dial2, "ACTIVE", "NO");
  }
  else 
  {
    IupSetAttribute(tgg2, "VALUE", "OFF");
    IupSetAttribute(dial2, "ACTIVE", "YES");
  }
  /* auto scaling Y axis */
  if (IupGetInt(plot[ii], "AXS_YAUTOMIN") && IupGetInt(plot[ii], "AXS_YAUTOMAX")) 
  {
    IupSetAttribute(tgg1, "VALUE", "ON");
    IupSetAttribute(dial1, "ACTIVE", "NO");
  }
  else 
  {
    IupSetAttribute(tgg1, "VALUE", "OFF");
    IupSetAttribute(dial1, "ACTIVE", "YES");
  }

  /* grid */
  value = IupGetAttribute(plot[ii], "GRID");
  if (value && strstr(value, "X"))
    IupSetAttribute(tgg3, "VALUE", "ON");
  else
    IupSetAttribute(tgg3, "VALUE", "OFF");
  if (value && strstr(value, "Y"))
    IupSetAttribute(tgg4, "VALUE", "ON");
  else
    IupSetAttribute(tgg4, "VALUE", "OFF");

  /* legend */
  if (IupGetInt(plot[ii], "LEGEND"))
    IupSetAttribute(tgg5, "VALUE", "ON");
  else
    IupSetAttribute(tgg5, "VALUE", "OFF");

  /* box */
  if (IupGetInt(plot[ii], "BOX"))
    IupSetAttribute(tgg6, "VALUE", "ON");
  else
    IupSetAttribute(tgg6, "VALUE", "OFF");

  return IUP_DEFAULT;
}

/* show/hide V grid */
static int tgg3_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v)
  {
    if (IupGetInt(tgg4, "VALUE"))
      IupSetAttribute(plot[ii], "GRID", "YES");
    else
      IupSetAttribute(plot[ii], "GRID", "VERTICAL");
  }
  else
  {
    if (!IupGetInt(tgg4, "VALUE"))
      IupSetAttribute(plot[ii], "GRID", "NO");
    else
      IupSetAttribute(plot[ii], "GRID", "HORIZONTAL");
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* show/hide H grid */
static int tgg4_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v)
  {
    if (IupGetInt(tgg3, "VALUE"))
      IupSetAttribute(plot[ii], "GRID", "YES");
    else
      IupSetAttribute(plot[ii], "GRID", "HORIZONTAL");
  }
  else
  {
    if (!IupGetInt(tgg3, "VALUE"))
      IupSetAttribute(plot[ii], "GRID", "NO");
    else
      IupSetAttribute(plot[ii], "GRID", "VERTICAL");
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}

/* show/hide box */
static int tgg6_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v)
    IupSetAttribute(plot[ii], "BOX", "YES");
  else
    IupSetAttribute(plot[ii], "BOX", "NO");

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}

/* show/hide legend */
static int tgg5_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v)
    IupSetAttribute(plot[ii], "LEGEND", "YES");
  else
    IupSetAttribute(plot[ii], "LEGEND", "NO");

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* auto scale Y */
static int tgg1_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v) {
    IupSetAttribute(dial1, "ACTIVE", "NO");
    IupSetAttribute(plot[ii], "AXS_YAUTOMIN", "YES");
    IupSetAttribute(plot[ii], "AXS_YAUTOMAX", "YES");
  }
  else {
    IupSetAttribute(dial1, "ACTIVE", "YES");
    IupSetAttribute(plot[ii], "AXS_YAUTOMIN", "NO");
    IupSetAttribute(plot[ii], "AXS_YAUTOMAX", "NO");
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* auto scale X */
static int tgg2_cb(Ihandle *self, int v)
{
  int ii = tabs_get_index();

  if (v) {
    IupSetAttribute(dial2, "ACTIVE", "NO");
    IupSetAttribute(plot[ii], "AXS_XAUTOMIN", "YES");
    IupSetAttribute(plot[ii], "AXS_XAUTOMAX", "YES");
  }
  else {
    IupSetAttribute(dial2, "ACTIVE", "YES");
    IupSetAttribute(plot[ii], "AXS_XAUTOMIN", "NO");
    IupSetAttribute(plot[ii], "AXS_XAUTOMAX", "NO");
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* Y zoom */
static int dial1_btndown_cb(Ihandle *self, double angle)
{
  int ii = tabs_get_index();

  IupStoreAttribute(plot[ii], "OLD_YMIN", IupGetAttribute(plot[ii], "AXS_YMIN"));
  IupStoreAttribute(plot[ii], "OLD_YMAX", IupGetAttribute(plot[ii], "AXS_YMAX"));

  return IUP_DEFAULT;
}

static int dial1_btnup_cb(Ihandle *self, double angle)
{
  int ii = tabs_get_index();
  double x1, x2, xm;
  char *ss;

  x1 = IupGetFloat(plot[ii], "OLD_YMIN");
  x2 = IupGetFloat(plot[ii], "OLD_YMAX");

  ss = IupGetAttribute(plot[ii], "AXS_YMODE");
  if ( ss && ss[3]=='2' ) {
    /* LOG2:  one circle will zoom 2 times */
    xm = 4.0 * fabs(angle) / 3.141592;
    if (angle>0.0) { x2 /= xm; x1 *= xm; }
    else { x2 *= xm; x1 /= xm; }
  }
  if ( ss && ss[3]=='1' ) {
    /* LOG10:  one circle will zoom 10 times */
    xm = 10.0 * fabs(angle) / 3.141592;
    if (angle>0.0) { x2 /= xm; x1 *= xm; }
    else { x2 *= xm; x1 /= xm; }
  }
  else {
    /* LIN: one circle will zoom 2 times */
    xm = (x1 + x2) / 2.0;
    x1 = xm - (xm - x1)*(1.0-angle*1.0/3.141592);
    x2 = xm + (x2 - xm)*(1.0-angle*1.0/3.141592);
  }

  if (x1<x2)
  {
    IupSetfAttribute(plot[ii], "AXS_YMIN", "%g", x1);
    IupSetfAttribute(plot[ii], "AXS_YMAX", "%g", x2);
  }

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}


/* X zoom */
static int dial2_btndown_cb(Ihandle *self, double angle)
{
  int ii = tabs_get_index();

  IupStoreAttribute(plot[ii], "OLD_XMIN", IupGetAttribute(plot[ii], "AXS_XMIN"));
  IupStoreAttribute(plot[ii], "OLD_XMAX", IupGetAttribute(plot[ii], "AXS_XMAX"));

  return IUP_DEFAULT;
}

static int dial2_btnup_cb(Ihandle *self, double angle)
{
  int ii = tabs_get_index();
  double x1, x2, xm;

  x1 = IupGetFloat(plot[ii], "OLD_XMIN");
  x2 = IupGetFloat(plot[ii], "OLD_XMAX");

  xm = (x1 + x2) / 2.0;

  x1 = xm - (xm - x1)*(1.0-angle*1.0/3.141592); /* one circle will zoom 2 times */
  x2 = xm + (x2 - xm)*(1.0-angle*1.0/3.141592);

  IupSetfAttribute(plot[ii], "AXS_XMIN", "%g", x1);
  IupSetfAttribute(plot[ii], "AXS_XMAX", "%g", x2);

  IupSetAttribute(plot[ii], "REDRAW", NULL);

  return IUP_DEFAULT;
}

#ifdef USE_IM
static int bt1_cb(Ihandle* self)
{
  imImage* image;
  int w, h;
  void* gldata;
  int ii = tabs_get_index();
  IupGetIntInt(plot[ii], "DRAWSIZE", &w, &h);
  gldata = malloc(w*h*3);
  image = imImageCreate(w, h, IM_RGB, IM_BYTE);
  IupMglPlotPaintTo(plot[ii], "RGB", w, h, 0, gldata);
  imConvertPacking(gldata, image->data[0], w, h, 3, 3, IM_BYTE, 1);
  imProcessFlip(image, image);
  imFileImageSave("../mglplot.png", "PNG", image);
  free(gldata);
  imImageDestroy(image);
  (void)self;
  return IUP_DEFAULT;
}
#else
static int bt1_cb(Ihandle *self)
{
  int ii = tabs_get_index();
//  IupSetAttribute(plot[ii], "CLEAR", "Yes");
//  IupSetAttribute(plot[ii], "REMOVE", "0");
//  IupSetAttribute(plot[ii], "ANTIALIAS", !IupGetInt(plot[ii], "ANTIALIAS")? "Yes": "No");
  IupSetAttribute(plot[ii], "OPENGL", !IupGetInt(plot[ii], "OPENGL")? "Yes": "No");
  IupSetAttribute(plot[ii], "REDRAW", NULL);

//  IupMglPlotPaintTo(plot[ii], "EPS", 0, 0, 0, "../mglplot.eps");
//  IupMglPlotPaintTo(plot[ii], "SVG", 0, 0, 0, "../mglplot.svg");
  return IUP_DEFAULT;
}
#endif

void MglPlotTest(void)
{
  Ihandle *vboxr[MAXPLOT+1];       /* tabs containing the plots */
  Ihandle *dlg, *vboxl, *hbox, *lbl1, *lbl2, *lbl3, *bt1,
          *boxinfo, *boxdial1, *boxdial2, *f1, *f2;
  int ii;

  IupControlsOpen();
  IupMglPlotOpen();     /* init IupMGLPlot library */

  IupSetGlobal("MGLFONTS", "../etc/mglfonts");

  /* create plots */
  for (ii=0; ii<MAXPLOT; ii++)
    plot[ii] = IupMglPlot();

  /* left panel: plot control
     Y zooming               */
  dial1 = IupDial("VERTICAL");
  lbl1 = IupLabel("+");
  lbl2 = IupLabel("-");
  boxinfo = IupVbox(lbl1, IupFill(), lbl2, NULL);
  boxdial1 = IupHbox(boxinfo, dial1, NULL);

  IupSetAttribute(boxdial1, "ALIGNMENT", "ACENTER");
  IupSetAttribute(boxinfo, "ALIGNMENT", "ACENTER");
  IupSetAttribute(boxinfo, "SIZE", "20x52");
  IupSetAttribute(boxinfo, "GAP", "2");
  IupSetAttribute(boxinfo, "MARGIN", "4");
  IupSetAttribute(boxinfo, "EXPAND", "YES");
  IupSetAttribute(lbl1, "EXPAND", "NO");
  IupSetAttribute(lbl2, "EXPAND", "NO");

  IupSetAttribute(dial1, "ACTIVE", "NO");
  IupSetAttribute(dial1, "SIZE", "20x52");
  IupSetCallback(dial1, "BUTTON_PRESS_CB", (Icallback)dial1_btndown_cb);
  IupSetCallback(dial1, "MOUSEMOVE_CB", (Icallback)dial1_btnup_cb);
  IupSetCallback(dial1, "BUTTON_RELEASE_CB", (Icallback)dial1_btnup_cb);

  tgg1 = IupToggle("Y Autoscale", NULL);
  IupSetCallback(tgg1, "ACTION", (Icallback)tgg1_cb);
  IupSetAttribute(tgg1, "VALUE", "ON");

  f1 = IupFrame( IupVbox(boxdial1, tgg1, NULL) );
  IupSetAttribute(f1, "TITLE", "Y Zoom");

  /* X zooming */
  dial2 = IupDial("HORIZONTAL");
  lbl1 = IupLabel("-");
  lbl2 = IupLabel("+");
  boxinfo = IupHbox(lbl1, IupFill(), lbl2, NULL);
  boxdial2 = IupVbox(dial2, boxinfo, NULL);

  IupSetAttribute(boxdial2, "ALIGNMENT", "ACENTER");
  IupSetAttribute(boxinfo, "ALIGNMENT", "ACENTER");
  IupSetAttribute(boxinfo, "SIZE", "64x16");
  IupSetAttribute(boxinfo, "GAP", "2");
  IupSetAttribute(boxinfo, "MARGIN", "4");
  IupSetAttribute(boxinfo, "EXPAND", "HORIZONTAL");

  IupSetAttribute(lbl1, "EXPAND", "NO");
  IupSetAttribute(lbl2, "EXPAND", "NO");

  IupSetAttribute(dial2, "ACTIVE", "NO");
  IupSetAttribute(dial2, "SIZE", "64x16");
  IupSetCallback(dial2, "BUTTON_PRESS_CB", (Icallback)dial2_btndown_cb);
  IupSetCallback(dial2, "MOUSEMOVE_CB", (Icallback)dial2_btnup_cb);
  IupSetCallback(dial2, "BUTTON_RELEASE_CB", (Icallback)dial2_btnup_cb);

  tgg2 = IupToggle("X Autoscale", NULL);
  IupSetCallback(tgg2, "ACTION", (Icallback)tgg2_cb);

  f2 = IupFrame( IupVbox(boxdial2, tgg2, NULL) );
  IupSetAttribute(f2, "TITLE", "X Zoom");

  lbl1 = IupLabel("");
  IupSetAttribute(lbl1, "SEPARATOR", "HORIZONTAL");

  tgg3 = IupToggle("Vertical Grid", NULL);
  IupSetCallback(tgg3, "ACTION", (Icallback)tgg3_cb);
  tgg4 = IupToggle("Horizontal Grid", NULL);
  IupSetCallback(tgg4, "ACTION", (Icallback)tgg4_cb);

  tgg6 = IupToggle("Box", NULL);
  IupSetCallback(tgg6, "ACTION", (Icallback)tgg6_cb);

  lbl2 = IupLabel("");
  IupSetAttribute(lbl2, "SEPARATOR", "HORIZONTAL");

  tgg5 = IupToggle("Legend", NULL);
  IupSetCallback(tgg5, "ACTION", (Icallback)tgg5_cb);

  lbl3 = IupLabel("");
  IupSetAttribute(lbl3, "SEPARATOR", "HORIZONTAL");

  bt1 = IupButton("Export", NULL);
  IupSetCallback(bt1, "ACTION", (Icallback)bt1_cb);

  vboxl = IupVbox(f1, f2, lbl1, tgg3, tgg4, tgg6, lbl2, tgg5, lbl3, bt1, NULL);
  IupSetAttribute(vboxl, "GAP", "4");
  IupSetAttribute(vboxl, "EXPAND", "NO");

  /* right panel: tabs with plots */
  for (ii=0; ii<MAXPLOT; ii++) {
    vboxr[ii] = IupVbox(plot[ii], NULL); /* each plot a tab */
    IupSetfAttribute(vboxr[ii], "TABTITLE", "Plot %d", ii); /* name each tab */
    IupSetHandle(IupGetAttribute(vboxr[ii], "TABTITLE"), vboxr[ii]);
  }
  vboxr[MAXPLOT] = NULL; /* mark end of vector */

  tabs = IupTabsv(vboxr);
  IupSetCallback(tabs, "TABCHANGE_CB", (Icallback)tabs_tabchange_cb);

  hbox = IupHbox(vboxl, tabs, NULL);
  IupSetAttribute(hbox, "MARGIN", "4x4");
  IupSetAttribute(hbox, "GAP", "10");
  
  dlg = IupDialog(hbox);
  IupSetAttribute(dlg, "SIZE", "170x");
  IupSetAttribute(dlg, "TITLE", "IupMglPlot Example");

  InitPlots(); /* It must be able to be done independent of dlg Mapping */

  tabs_tabchange_cb(tabs, vboxr[0]);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupSetAttribute(dlg, "SIZE", NULL);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  MglPlotTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif

#endif
