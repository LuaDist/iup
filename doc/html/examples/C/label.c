/* IupLabel: Example in C 
   Creates three labels, one using all attributes except for image, other with normal text and the last one with an image.
*/

/* ANSI C libraries include */
#include <stdlib.h>
#include <stdio.h>

/* IUP libraries include */
#include <iup.h>

/* Global variables */
static unsigned char pixmap_star [ ] =
{
  1,1,1,1,1,1,2,1,1,1,1,1,1,
  1,1,1,1,1,1,2,1,1,1,1,1,1,
  1,1,1,1,1,2,2,2,1,1,1,1,1,
  1,1,1,1,1,2,2,2,1,1,1,1,1,
  1,1,2,2,2,2,2,2,2,2,2,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,
  1,1,1,2,2,2,2,2,2,2,1,1,1,
  1,1,1,1,2,2,2,2,2,1,1,1,1,
  1,1,1,1,2,2,2,2,2,1,1,1,1,
  1,1,1,2,2,1,1,2,2,2,1,1,1,
  1,1,2,2,1,1,1,1,1,2,2,1,1,
  1,2,2,1,1,1,1,1,1,1,2,2,1,
  2,2,1,1,1,1,1,1,1,1,1,2,2
};

/* Main program */
int main(int argc, char **argv)
{
  /* IUP identifiers */
  Ihandle *dlg;
  Ihandle *img_star;
  Ihandle *lbl, *lbl_explain, *lbl_star;

  /* Initializes IUP */
  IupOpen(&argc, &argv);

  /* Program begin */

  /* Creates the star image */
  img_star = IupImage ( 13, 13, pixmap_star );

  /* Sets star image colors */
  IupSetAttribute ( img_star, "1", "0 0 0");
  IupSetAttribute ( img_star, "2", "0 198 0");

  /* Associates "img_star" to image img_star */
  IupSetHandle ( "img_star", img_star );
    
  
  /* Creates a label */
  lbl = IupLabel ( "This label has the following attributes set:\nBGCOLOR = 255 255 0\nFGCOLOR = 0 0 255\nFONT = COURIER_NORMAL_14\nTITLE = All text contained here\nALIGNMENT = ACENTER" );
  
  /* Sets all the attributes of label lbl, except for IMAGE */
  IupSetAttributes ( lbl, "BGCOLOR = \"255 255 0\", FGCOLOR = \"0 0 255\", FONT = COURIER_NORMAL_14, ALIGNMENT = ACENTER");

  /* Creates a label to explain that the label on the right has an image */
  lbl_explain = IupLabel ( "The label on the right has the image of a star" );

  /* Creates a label whose title is not important, cause it will have an image */
  lbl_star = IupLabel (NULL);

  /* Associates image "img_star" with label lbl_star */
  IupSetAttribute ( lbl_star, "IMAGE", "img_star" );
  
  /* Creates dialog with the label */
  dlg = IupDialog ( IupVbox ( lbl, IupHbox ( lbl_explain, lbl_star, NULL ), NULL ) );

  /* Sets title of the dialog */
  IupSetAttribute ( dlg, "TITLE", "IupLabel Example" );

  /* Shows dialog in the center of the screen */
  IupShowXY ( dlg, IUP_CENTER, IUP_CENTER );

  /* Initializes IUP main loop */
  IupMainLoop();

  /* Finishes IUP */
  IupClose();

  /* Program finished successfully */
  return EXIT_SUCCESS;

}