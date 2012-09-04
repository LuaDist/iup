/*  IupGetColor: Example in C
    Creates a predefined color selection dialog. The user receives the color in the RGB format.
*/

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

int main(int argc, char **argv)
{
  unsigned char r, g, b;

  IupOpen(&argc, &argv);
  IupControlsOpen ();

  if(IupGetColor(100, 100, &r, &g, &b))
    printf("RGB = %.2X%.2X%.2X\n", r, g, b);

  IupClose();
  return EXIT_SUCCESS;
}