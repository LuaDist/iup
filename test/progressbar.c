#include <stdlib.h>
#include <stdio.h>

#include "iup.h"
#include "iupcontrols.h"

static float increment = 0.01f;
static Ihandle *progressbar1;
static Ihandle *progressbar2;
static Ihandle *btn_pause;
static Ihandle *timer = NULL;

static unsigned char pixmap_play[] = 
{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 };

static unsigned char pixmap_restart[] = 
{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2
 ,2,2,2,2,1,1,2,2,2,2,2,2,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,1,1,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,1,1,2,2,1,1,1,1,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,1,1,2,2,1,1,1,1,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,1,1,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,1,1,2,2,2,2,2,2,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 };

static unsigned char pixmap_rewind[] = 
{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,1,1,2,2,2,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,1,2,2,1,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,2,1,1,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,1,1,1,1,2,1,1,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,1,2,2,1,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,1,1,2,2,2,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 };

static unsigned char pixmap_forward[] = 
{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,2,1,1,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,1,2,2,1,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,1,1,2,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,1,1,2,1,1,1,1,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,1,2,2,1,1,1,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,2,1,1,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 };

static unsigned char pixmap_pause[] = 
{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,1,1,2,2,1,1,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
 ,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 };

static void createimg_s (void)
{ 
  Ihandle *img_restart, *img_play, *img_forward, *img_rewind, *img_pause;
  
  img_restart = IupImage(22,22, pixmap_restart);
  img_play = IupImage(22,22, pixmap_play);
  img_forward = IupImage(22,22, pixmap_forward);
  img_rewind = IupImage(22,22, pixmap_rewind);
  img_pause = IupImage(22,22, pixmap_pause);
  
  IupSetHandle ("img_restart", img_restart); 
  IupSetHandle ("img_play", img_play);
  IupSetHandle ("img_forward", img_forward); 
  IupSetHandle ("img_rewind", img_rewind);
  IupSetHandle ("img_pause", img_pause);
  
  IupSetAttribute (img_restart, "1", "0 0 0"); 
  IupSetAttribute (img_restart, "2", "BGCOLOR");
  IupSetAttribute (img_play, "1", "0 0 0"); 
  IupSetAttribute (img_play, "2", "BGCOLOR"); 
  IupSetAttribute (img_forward, "1", "0 0 0"); 
  IupSetAttribute (img_forward, "2", "BGCOLOR");
  IupSetAttribute (img_rewind, "1", "0 0 0"); 
  IupSetAttribute (img_rewind, "2", "BGCOLOR"); 
  IupSetAttribute (img_pause, "1", "0 0 0"); 
  IupSetAttribute (img_pause, "2", "BGCOLOR");
}

static int time_cb(void)
{
  float value = IupGetFloat(progressbar2, "VALUE");
  value += increment*50;
  if (value > 50) value = 0; /* start over */
  IupSetfAttribute(progressbar2, "VALUE", "%g", (double)value);
  return IUP_DEFAULT;
}

static int btn_pause_cb(void)
{
  if (!IupGetInt(timer, "RUN"))
  {
    IupSetAttribute(progressbar1, "MARQUEE", "YES");
    IupSetAttribute(timer, "RUN", "YES");
    IupSetAttribute(btn_pause, "IMAGE", "img_pause");
  }
  else
  {
    IupSetAttribute(progressbar1, "MARQUEE", "NO");
    IupSetAttribute(timer, "RUN", "NO");
    IupSetAttribute(btn_pause, "IMAGE", "img_play");
  }
  
  return IUP_DEFAULT;
}


static int unmap_cb(Ihandle* ih)
{
  IupDestroy(timer);
  timer = NULL;
  return IUP_DEFAULT;
}

static int btn_restart_cb(void)
{
  IupSetAttribute(progressbar2, "VALUE", "0");
  return IUP_DEFAULT;
}

static int btn_accelerate_cb(void)
{
  increment *= 2;
  return IUP_DEFAULT;
}

static int btn_decelerate_cb(void)
{
  increment /= 2;
  return IUP_DEFAULT;
}

static int btn_show1_cb(void)
{
  if (!IupGetInt(progressbar2, "DASHED"))
    IupSetAttribute(progressbar2, "DASHED", "YES");
  else
    IupSetAttribute(progressbar2, "DASHED", "NO");
  
  return IUP_DEFAULT;
}

static int btn_show2_cb(void)
{
  if (!IupGetInt(progressbar1, "MARQUEE"))
    IupSetAttribute(progressbar1, "MARQUEE", "YES");
  else
    IupSetAttribute(progressbar1, "MARQUEE", "NO");
  
  return IUP_DEFAULT;
}

void ProgressbarTest(void)
{
  Ihandle *dlg, *vbox, *hbox;
  Ihandle *btn_restart, *btn_accelerate, *btn_decelerate, *btn_show1, *btn_show2;

  /* timer to update progressbar2 */
  if (timer)
    IupDestroy(timer);
  timer = IupTimer();
  IupSetCallback(timer, "ACTION_CB", (Icallback)time_cb);
  IupSetAttribute(timer, "TIME", "100");
  
  progressbar1 = IupProgressBar();
  progressbar2 = IupProgressBar();
 
  IupSetAttribute(progressbar1, "EXPAND", "YES");
  IupSetAttribute(progressbar1, "MARQUEE", "YES");

  IupSetAttribute(progressbar2, "ORIENTATION", "VERTICAL");
  IupSetAttribute(progressbar2, "BGCOLOR", "255 0 128");
  IupSetAttribute(progressbar2, "FGCOLOR", "0 128 0");
  IupSetAttribute(progressbar2, "RASTERSIZE", "30x100");
  IupSetAttribute(progressbar2, "MAX", "50");
  IupSetAttribute(progressbar2, "VALUE", "25");
  //IupSetAttribute(progressbar2, "DASHED", "YES");

  btn_restart = IupButton(NULL, NULL);
  btn_pause = IupButton(NULL, NULL);
  btn_accelerate = IupButton(NULL, NULL);
  btn_decelerate = IupButton(NULL, NULL);
  btn_show1 = IupButton("Dashed", NULL);
  btn_show2 = IupButton("Marquee", NULL);

  createimg_s();

  IupSetAttribute(btn_restart, "IMAGE", "img_restart");
  IupSetAttribute(btn_restart, "TIP", "Restart" );
  IupSetAttribute(btn_pause, "IMAGE", "img_pause");
  IupSetAttribute(btn_pause, "TIP", "Play/Pause");
  IupSetAttribute(btn_accelerate, "IMAGE", "img_forward");
  IupSetAttribute(btn_accelerate, "TIP", "Accelerate");
  IupSetAttribute(btn_decelerate, "IMAGE", "img_rewind");
  IupSetAttribute(btn_decelerate, "TIP", "Decelerate");
  IupSetAttribute(btn_show1, "TIP", "Dashed or Continuous");
  IupSetAttribute(btn_show2, "TIP", "Marquee or Defined");

  hbox = IupHbox
         (
           IupFill(), 
           btn_pause,
           btn_restart,
           btn_decelerate,
           btn_accelerate,
           btn_show1,
           btn_show2,
           IupFill(),
           NULL
         );

  vbox = IupHbox(IupVbox(progressbar1, hbox, NULL), progressbar2, NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "5");

  dlg = IupDialog(vbox);
  
  IupSetAttribute(dlg, "TITLE", "IupProgressBar Test");
  IupSetCallback(dlg, "UNMAP_CB", (Icallback) unmap_cb);

  IupSetCallback(btn_pause, "ACTION", (Icallback) btn_pause_cb);
  IupSetCallback(btn_restart, "ACTION", (Icallback) btn_restart_cb);
  IupSetCallback(btn_accelerate, "ACTION", (Icallback) btn_accelerate_cb);
  IupSetCallback(btn_decelerate, "ACTION", (Icallback) btn_decelerate_cb);
  IupSetCallback(btn_show1, "ACTION", (Icallback) btn_show1_cb);
  IupSetCallback(btn_show2, "ACTION", (Icallback) btn_show2_cb);
  
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupSetAttribute(timer, "RUN", "YES");
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ProgressbarTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
