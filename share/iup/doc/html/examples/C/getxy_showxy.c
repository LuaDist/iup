#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iup.h"
#include "iupcontrols.h"


int resize_cb(Ihandle *ih, int w, int h)
{
  Ihandle* mat = IupGetChild(ih, 0);
	IupSetAttribute(mat, "RASTERWIDTH2", NULL);
	IupSetAttribute(mat, "FITTOSIZE", "COLUMNS");
	return IUP_DEFAULT;
}

int close_cb(Ihandle *ih)
{
  FILE *fp;
	int X = IupGetInt(ih, "X");
	int Y = IupGetInt(ih, "Y");
	printf("close_cb: X,Y=%d,%d\n", X, Y);

  fp = fopen("test.dat", "w");
  fwrite(&X, sizeof(int), 1, fp);
  fwrite(&Y, sizeof(int), 1, fp);
  fclose(fp);
	return IUP_DEFAULT;
}

Ihandle *create_mat(void)
{
  Ihandle *mat = IupMatrix(NULL); 

  IupSetAttribute(mat,"NUMCOL","3"); 
  IupSetAttribute(mat,"NUMLIN","3"); 
  IupSetAttribute(mat,"NUMCOL_VISIBLE","3");
  IupSetAttribute(mat,"NUMLIN_VISIBLE","3");
  IupSetAttribute(mat,"WIDTH0", "0");
  IupSetAttribute(mat,"WIDTH1","100");
  IupSetAttribute(mat,"WIDTH3","70");
  
  IupSetAttribute(mat,"1:0","Medicine");
  IupSetAttribute(mat,"2:0","Food"); 
  IupSetAttribute(mat,"3:0","Energy"); 
  IupSetAttribute(mat,"1:1","5.6");
  IupSetAttribute(mat,"2:1","2.2");
  IupSetAttribute(mat,"3:1","7.2");
  IupSetAttribute(mat,"1:2","4.5");
  IupSetAttribute(mat,"2:2","8.1");
  IupSetAttribute(mat,"3:2","3.4");
  IupSetAttribute(mat,"READONLY","YES");
  IupSetAttribute(mat,"SCROLLBAR","VERTICAL");
  return mat;
}

int main(int argc, char **argv)
{
	int X=100, Y=200;
	FILE *fp;
  Ihandle *dlg, *mat;

	fp = fopen("test.dat", "r");
	if (fp != NULL)
	{
		fread(&X,sizeof(int),1,fp);
		fread(&Y,sizeof(int),1,fp);
		fclose(fp);
	}

	printf("X,Y=%d,%d\n", X, Y);

  IupOpen(&argc, &argv);       
  IupControlsOpen ();

  mat = create_mat();
  dlg = IupDialog(mat);
  IupSetAttribute(dlg,"SHRINK","YES"); 
  IupSetAttribute(dlg, "TITLE", "IupMatrix");
  IupSetCallback(dlg, "RESIZE_CB", (Icallback) resize_cb);
  IupSetCallback(dlg, "CLOSE_CB", (Icallback) close_cb);

  IupShowXY (dlg,X,Y);
  IupMainLoop ();
  IupClose ();  
  return EXIT_SUCCESS;
}
