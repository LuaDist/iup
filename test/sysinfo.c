#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

void SysInfoTest(void)
{
  char* sysver;
  int motif = 0;

  printf("IUP System Info:\n");
  printf("  Version: %s\n", IupVersion());
  printf("  Copyright: %s\n", IUP_COPYRIGHT);
  printf("  Driver: %s\n", IupGetGlobal("DRIVER"));
  printf("  System: %s\n", IupGetGlobal("SYSTEM"));
  printf("  System Version: %s\n", IupGetGlobal("SYSTEMVERSION"));
  printf("  System Language: %s\n", IupGetGlobal("SYSTEMLANGUAGE"));
  printf("  System Locale: %s\n\n", IupGetGlobal("SYSTEMLOCALE"));

  sysver = IupGetGlobal("MOTIFVERSION");
  if (sysver) 
  {
    printf("  Motif Version: %s\n", sysver);
    printf("  Motif Number: %s\n", IupGetGlobal("MOTIFNUMBER"));
    printf("  X Server Vendor: %s\n", IupGetGlobal("XSERVERVENDOR"));
    printf("  X Vendor Release: %s\n\n", IupGetGlobal("XVENDORRELEASE"));
    motif = 1;
  }

  sysver = IupGetGlobal("GTKVERSION");
  if (sysver) 
    printf("  GTK Version: %s\n\n", sysver);

  printf("  Screen Depth: %s\n", IupGetGlobal("SCREENDEPTH"));
  printf("  Screen Size: %s\n", IupGetGlobal("SCREENSIZE"));
  printf("  Full Screen Size: %s\n", IupGetGlobal("FULLSIZE"));
  if (!motif)
  {
    printf("  Virtual Screen: %s\n", IupGetGlobal("VIRTUALSCREEN"));
    printf("  Monitors Info: %s\n", IupGetGlobal("MONITORSINFO"));
  }
  printf("  True Color Canvas: %s\n\n", IupGetGlobal("TRUECOLORCANVAS"));

  printf("  Computer Name: %s\n", IupGetGlobal("COMPUTERNAME"));
  printf("  User Name: %s\n", IupGetGlobal("USERNAME"));
  printf("  Default Font: %s\n", IupGetGlobal("DEFAULTFONT"));
}


#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  SysInfoTest();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
