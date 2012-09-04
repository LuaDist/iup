#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

void ScanfTest(void)
{
  int ret;
  int integer = 12;
  float real = 1e-3f;
  char text[300]="This is a vector of characters";
  char *fmt =
  {
   "IupScanf Example\n"
   "Text: %300.5%s\n"
   "Real: %20.10%g\n"
   "Integer: %20.10%d\n"
  };
  
//  IupSetLanguage("ENGLISH");

  ret = IupScanf(fmt, text, &real, &integer);
  if (ret == -1)
    IupMessage("IupScanf","Operation canceled");
  else
    IupMessagef("IupScanf","Text: %s\nReal: %f\nInteger: %d\nFields read successfully: %d",text,real,integer,ret);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ScanfTest();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
