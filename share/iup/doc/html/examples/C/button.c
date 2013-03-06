/*****************************************************************************
 *                             IupButton example                             * 
 *   Description : Creates four buttons. The first uses images, the second   * 
 *                 turns the first on and off, the third exits the           * 
 *                 application and the last does nothing                     * 
 *****************************************************************************/
 
/* Includes ANSI C libraries */
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

/* Includes IUP libraries */
#include <iup.h>

/* Declares global C variables */
/* Defines released button's image */
static unsigned char pixmap_release[] = 
{
       1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
       1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,4,4,3,3,3,3,2,2,
       1,1,3,3,3,3,3,4,4,4,4,3,3,3,2,2,
       1,1,3,3,3,3,3,4,4,4,4,3,3,3,2,2,
       1,1,3,3,3,3,3,3,4,4,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
       2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
};

/* Defines pressed button's image */
static unsigned char pixmap_press[] = 
{
       1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
       1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,4,4,3,3,3,3,3,2,2,
       1,1,3,3,3,3,4,4,4,4,3,3,3,3,2,2,
       1,1,3,3,3,3,4,4,4,4,3,3,3,3,2,2,
       1,1,3,3,3,3,3,4,4,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
       2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
}; 

/* Defines inactive button's image */
static unsigned char pixmap_inactive[] = 
{
       1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
       1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,4,4,3,3,3,3,2,2,
       1,1,3,3,3,3,3,4,4,4,4,3,3,3,2,2,
       1,1,3,3,3,3,3,4,4,4,4,3,3,3,2,2,
       1,1,3,3,3,3,3,3,4,4,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,
       1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
       2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
};

/******************************************************************************
 * Function:                                                                  *
 * On off button callback                                                     *
 *                                                                            *
 * Description:                                                               *
 * Callback called when the on/off button is activated. Turns the button with *
 * image on and off                                                           *
 *                                                                            *
 * Parameters received:                                                       *
 * self - element identifier                                                  *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_on_off_cb(Ihandle *self)
{
  /* IUP handles */
  Ihandle *btn_image;
    
  /* Recovers "btn_image" handle */
  btn_image = IupGetHandle( "btn_image" );
  
  /* If the button with with image is active...*/
  if(!strcmp( IupGetAttribute( btn_image, "ACTIVE" ), "YES" ) )
  {
    /* Deactivates the button with image */
    IupSetAttribute( btn_image, "ACTIVE","NO" );
  }
  /* else it is inactive */
  else
  {
    /* Activates the button with image */
    IupSetAttribute( btn_image, "ACTIVE", "YES" );
  }

  /* Executed function successfully */
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Button with image button callback                                          *
 *                                                                            *
 * Description:                                                               *
 * Callback called when the exit button is pressed or released. Shows a       *
 * message saying if the button was pressed or released                       *
 *                                                                            *
 * Parameters received:                                                       *
 * self - identifies the canvas that activated the function’s execution.      *
 * b    - identifies the activated mouse button:                              *
 *                                                                            *
 * IUP_BUTTON1 left mouse button (button 1)                                   *
 * IUP_BUTTON2 middle mouse button (button 2)                                 *
 * IUP_BUTTON3 right mouse button (button 3)                                  *
 *                                                                            *
 * e     - indicates the state of the button:                                 *
 *                                                                            *
 * 0 mouse button was released                                                *
 * 1 mouse button was pressed                                                 *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_image_button_cb( Ihandle *self,int b, int e )
{
  /* If the left button changed its state... */
  if( b == IUP_BUTTON1 )
  {
    /* IUP handles */
    Ihandle* text;
    
    /* Recovers "text" handle */
    text = IupGetHandle( "text" );
    
    /* If the button was pressed... */
    if(e == 1) 
    {
      /* Sets text's value */ 
      IupSetAttribute( text, "VALUE", "Red button pressed" );
    }
    /* else the button was released */
    else       
    {
      /* Sets text's value */ 
      IupSetAttribute( text, "VALUE", "Red button released" );
    }
  }
  
  /* Executed function successfully */
  return IUP_DEFAULT;
}

int btn_big_button_cb(Ihandle *self, int button, int press)
{
  printf("BUTTON_CB(button=%c, press=%d)\n", button, press);
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Exit button callback                                                       *
 *                                                                            *
 * Description:                                                               *
 * Callback called when exit button is activated. Exits the program           *
 *                                                                            *
 * Parameters received:                                                       *
 * self - element identifier                                                  *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_exit_cb( Ihandle *self )
{
  /* Exits the program */
  return IUP_CLOSE;
}

/******************************************************************************
 * Main function                                                              *
 ******************************************************************************/
int main(int argc, char **argv)
{
  /* IUP handles */
  Ihandle *dlg;
  Ihandle *btn_image, *btn_exit, *btn_big, *btn_on_off;
  Ihandle *img_release, *img_press, *img_inactive;
  Ihandle *text;

  /* Initializes IUP */
  IupOpen(&argc, &argv);

  /* Creates a text */
  text = IupText( NULL );                                  
  
  /* Turns on read-only mode */
  IupSetAttribute( text, "READONLY", "YES");
  
  /* Associates text with handle "text" */
  IupSetHandle ( "text", text );                            
  
  /* Defines released button's image size */
  img_release = IupImage(16, 16, pixmap_release);        
  
  /* Defines released button's image colors */
  IupSetAttribute( img_release, "1", "215 215 215" );
  IupSetAttribute( img_release, "2", "40 40 40" );   
  IupSetAttribute( img_release, "3", "30 50 210" );  
  IupSetAttribute( img_release, "4", "240 0 0" );    
  
  /* Associates img_release with handle "img_release" */
  IupSetHandle( "img_release", img_release );
  
  /* Defines pressed button's image size */
  img_press = IupImage( 16, 16, pixmap_press );            
  
  /* Defines pressed button's image colors */
  IupSetAttribute( img_press, "1", "40 40 40" );    
  IupSetAttribute( img_press, "2", "215 215 215" ); 
  IupSetAttribute( img_press, "3", "0 20 180" );    
  IupSetAttribute( img_press, "4", "210 0 0" );     
  
  /* Associates img_press with handle "img_press" */
  IupSetHandle ( "img_press", img_press );

  /* Defines inactive button's image size */
  img_inactive = IupImage( 16, 16, pixmap_inactive );      
  
  /* Defines inactive button's image colors */
  IupSetAttribute( img_inactive, "1", "215 215 215" );   
  IupSetAttribute( img_inactive, "2", "40 40 40" );      
  IupSetAttribute( img_inactive, "3", "100 100 100" );   
  IupSetAttribute( img_inactive, "4", "200 200 200" );   
  
  /* Associates img_inactive with handle "img_inactive" */
  IupSetHandle ("img_inactive", img_inactive );

  /* Creates a button */
  btn_image = IupButton ( "Button with image", "btn_image");       
  
  /* Sets released, pressed and inactive button images */  
  IupSetAttribute( btn_image, "IMAGE", "img_release" );        
  IupSetAttribute( btn_image, "IMPRESS", "img_press" );      
  IupSetAttribute( btn_image, "IMINACTIVE", "img_inactive" );  
  
  /* Associates button callback with action bti_button_act */
  IupSetAttribute( btn_image, "BUTTON_CB", "btn_image_button"); 
  
  /* Associates btn_image with handle "btn_image" */
  IupSetHandle( "btn_image", btn_image );

  /* Creates a button */
  btn_big = IupButton( "Big useless button", "" );             
  
  /* Sets big button size */
  IupSetAttribute( btn_big, "SIZE", "EIGHTHxEIGHTH" );
  
  /* Creates a button entitled Exit associated with action exit_act */
  btn_exit = IupButton( "Exit", "btn_exit");
  
  /* Creates a button entitled on/off associated with action onoff_act */
  btn_on_off = IupButton( "on/off", "btn_on_off");              

  /* Creates dialog with the four buttons and the text*/
  dlg = IupDialog
        (
          IupVbox
          (
            IupHbox
            (
              btn_image,
              btn_on_off,
              btn_exit,
              NULL
            ),
            text,
            btn_big,
            NULL
          )
        );
  
  /* Sets dialogs title to IupButton turns resize, maximize, minimize and    */
  /* menubox off                                                             */ 
  IupSetAttributes( dlg, "EXPAND = YES, TITLE = IupButton, RESIZE = NO" );
  IupSetAttributes( dlg, "MENUBOX = NO, MAXBOX = NO, MINBOX = NO" );

  /* Registers callbacks */  
  IupSetCallback( btn_exit, "ACTION", (Icallback) btn_exit_cb );
  IupSetCallback( btn_on_off, "ACTION", (Icallback) btn_on_off_cb );     
  IupSetCallback( btn_image, "ACTION", (Icallback) btn_image_button_cb );

  IupSetAttribute(btn_big, "BUTTON_CB", "bigtest");
  IupSetFunction("bigtest", (Icallback)btn_big_button_cb);
//  IupSetCallback(btn_big, "BUTTON_CB", (Icallback)btn_big_button_cb);
  
  /* Shows dialog on the center of the screen */
  IupShowXY( dlg, IUP_CENTER, IUP_CENTER );
    
  /* Initializes IUP main loop */
  IupMainLoop();

  /* Finishes IUP */
  IupClose();  

  /* Program finished successfully */
  return EXIT_SUCCESS;

}