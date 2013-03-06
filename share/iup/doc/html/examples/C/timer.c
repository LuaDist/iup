/* IupTimer Example in C */

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

Ihandle *timer1, *timer2;

int timer_cb(Ihandle *n)
{
  if(n == timer1)
    printf("timer 1 called\n");

  if(n == timer2)
  {
    printf("timer 2 called\n");
    return IUP_CLOSE;  
  }

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dialog, *text;

  IupOpen(&argc, &argv);

  text = IupLabel("Timer example");
  IupSetHandle("quit", text);

  /* Creating main dialog */
  dialog = IupDialog(IupVbox(text, NULL));
  IupSetAttribute(dialog, "TITLE", "timer example");
  IupSetAttribute(dialog, "SIZE", "200x200");
  IupShowXY(dialog, IUP_CENTER, IUP_CENTER);

  timer1 = IupTimer();
  timer2 = IupTimer();

  IupSetAttribute(timer1, "TIME", "1000");
  IupSetAttribute(timer1, "RUN", "YES");
  IupSetCallback(timer1, "ACTION_CB", (Icallback)timer_cb);

  IupSetAttribute(timer2, "TIME", "4000");
  IupSetAttribute(timer2, "RUN", "YES");
  IupSetCallback(timer2, "ACTION_CB", (Icallback)timer_cb);

  IupMainLoop();

  /* Timers are NOT automatically destroyed, must be manually done */
  IupDestroy(timer1);
  IupDestroy(timer2);

  IupClose();

  return EXIT_SUCCESS;

}
