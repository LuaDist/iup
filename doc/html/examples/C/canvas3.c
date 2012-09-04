/*
 * IupCanvas Redraw example
 */

#include <stdlib.h>
#include <stdio.h>

#include <iup.h>
#include <iupcontrols.h>
#include <cd.h>
#include <cdiup.h>

Ihandle *dlg     = NULL;
Ihandle *bt      = NULL;
Ihandle *gauge   = NULL;
Ihandle *tabs    = NULL;
Ihandle *cv      = NULL;
cdCanvas*cdcanvas= NULL;

int need_redraw, redraw_count = 0;

int toggle_redraw(void)
{
  cdCanvasActivate(cdcanvas);
  need_redraw = !need_redraw;
  return IUP_DEFAULT;
}

int redraw(void)
{
  if (need_redraw)
  {
    cdCanvasBox(cdcanvas, 0, 300, 0, redraw_count/100);
    IupSetfAttribute(gauge, "VALUE", "%g", (float)redraw_count/30000.0f);

    redraw_count++;
    if (redraw_count == 30000)
    {
      cdCanvasClear(cdcanvas);
      redraw_count = 0;
      need_redraw = 0;
    }
  }
  return IUP_DEFAULT;
}

int main(int argc, char **argv) 
{
  IupOpen(&argc, &argv);
//  IupControlsOpen();
  
  gauge = IupProgressBar();
//  gauge = IupGauge();
  cv    = IupCanvas(NULL);
  bt    = IupButton("Start/Stop", NULL);
  IupSetAttribute(bt,    "SIZE", "50x50");
  IupSetAttribute(gauge, "SIZE", "200x15");
  IupSetAttribute(cv,    "SIZE", "200x200");
  dlg   = IupDialog(IupVbox(cv, IupHbox(gauge, bt, NULL), NULL));
  IupSetAttribute(dlg, "TITLE", "Redraw test");

  IupSetFunction("IDLE_ACTION", (Icallback)redraw);

  IupMap(dlg);
  
  cdcanvas = cdCreateCanvas(CD_IUP, cv);
  cdCanvasForeground(cdcanvas, CD_BLUE);
  cdCanvasClear(cdcanvas);
  
  IupSetCallback(bt, "ACTION", (Icallback)toggle_redraw);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;
}
