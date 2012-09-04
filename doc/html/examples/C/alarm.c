/* Iup Alarm: Example in C 
   Shows a dialog similar to the one shown when you exit a program without saving.
*/

#include <stdlib.h>
#include <stdio.h>

/* IUP libraries include */
#include "iup.h"

/* Main program */
int main(int argc, char **argv)
{
  /* Initializes IUP */
  IupOpen(&argc, &argv);

  /* Executes IupAlarm */
  switch (IupAlarm ("IupAlarm Example",
    "File not saved! Save it now?", "Yes", "No", "Cancel"))
  {
    /* Shows a message for each selected button */
    case 1:
      IupMessage ("Save file", "File saved successfully - leaving program");
    break;

    case 2:
      IupMessage ("Save file", "File not saved - leaving program anyway");
    break;

    case 3:
      IupMessage ("Save file", "Operation canceled");
    break;
  }
  
  /* Finishes IUP */
  IupClose();

  /* Program finished successfully */
  return EXIT_SUCCESS;
}