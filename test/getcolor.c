#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

void GetColorTest(void)
{
  unsigned char r = 10, g = 100, b = 25;
  if (IupGetColor(100, 100, &r, &g, &b))
    printf("RGB = %d, %d, %d  Hex=[%.2X%.2X%.2X]\n", r, g, b, r, g, b);
}


#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  GetColorTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
