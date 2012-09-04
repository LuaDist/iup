/* "IDLE_ACTION": Example in C
   Creating a program that computes a number’s factorial using the idle function.
*/

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

/************************************************************************************ 
* structure that stores the computation’s state
************************************************************************************/
static struct
{
  int step;       /* iteration step */
  double fatorial; /* last computed value */
} calc;

/************************************************************************************ 
* end of computation: defines the function associated to the idle function as NULL
************************************************************************************/
static int end_compute (void)
{
  IupSetFunction ("IDLE_ACTION", (Icallback) NULL);
  return IUP_DEFAULT;
}

/************************************************************************************ 
* computes one iteration step of the computation; this function will be 
* called every time the user is not interacting with the application
************************************************************************************/
static int idle_function (void)
{
  static char str[80]; /* must be static because will be passed*/

  /* "TITLE" value for IUP */
  calc.step++; /* next iteration step */
  calc.fatorial *= calc.step; /* executes one computation step */

  /* feedback to the user on the current step and the last computed value */
  sprintf (str, "%d -> %10.4g",calc.step,calc.fatorial);
  IupStoreAttribute (IupGetHandle("mens"), "VALUE", str);

  if (calc.step == 100) /* computation ends when step = 100 */
   end_compute();

  return IUP_DEFAULT;
}

/************************************************************************************ 
* begin computation: initializes initial values and sets idle function
************************************************************************************/
static int start_calc (void)
{
  calc.step = 0;
  calc.fatorial = 1.0;
  IupSetFunction ("IDLE_ACTION", (Icallback) idle_function);
  IupSetAttribute (IupGetHandle("mens"), "VALUE", "Computing...");
  return IUP_DEFAULT;
}

int close(void)
{
  return IUP_CLOSE;
}

/************************************************************************************ 
* main program
************************************************************************************/
int main(int argc, char **argv)
{
  Ihandle *text, *bt, *dlg;
  IupOpen(&argc, &argv);
 
  text = IupText(NULL);
  IupSetAttribute(text, "SIZE", "100x20");
  IupSetHandle("mens", text);
  IupSetCallback(text, "K_CR", (Icallback)close);

  bt = IupButton("Calculate", "calc");
  IupSetCallback(bt, "ACTION", (Icallback)start_calc);

  dlg = IupDialog(IupVbox(text, bt, NULL));
  IupSetAttribute(dlg, "TITLE", "Idle");


  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();
  IupClose();
  return EXIT_SUCCESS;
}
