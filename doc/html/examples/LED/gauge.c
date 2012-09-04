#include <stdio.h>
#include "iup.h"
#include "iupcontrols.h"
#include "cd.h"
#include "cdiup.h"

/* global variables that store handles used by the idle function */
Ihandle *dlg=NULL;
Ihandle *gauge=NULL;

/* speed with which the progress indicator advances */
double velocidade = 0.001 ;

/* idle callback */
int idle_cb(void)
{
  char newvalue[40];
  double value = IupGetFloat(gauge, "VALUE") ;
  value+=velocidade ;
  if(value < 0.0) value = 0.0 ;
  if(value > 1.0) value = 1.0 ;
  sprintf(newvalue, "%.7f",value) ;
  IupSetAttribute(gauge, "VALUE", newvalue) ;

  return IUP_DEFAULT ;
}

/* pause button callback */
int pausa_cb(void)
{
  if(IupGetFunction("IDLE_ACTION") == NULL)
  {
    // set idle callback 
    IupSetFunction("IDLE_ACTION", (Icallback) idle_cb);
  }
  else
  {
    // reset idle callback 
    IupSetFunction("IDLE_ACTION", NULL);
  }
  return IUP_DEFAULT ;
}

/* start button callback */
int inicio_cb(void)
{
  IupSetAttribute(gauge, "VALUE", "0") ;
	
  return IUP_DEFAULT ;
}

/* accelerate button callback */
int acelera_cb(void)
{
  velocidade += 0.0001 ;
  if(velocidade > 1.0)velocidade = 1.0 ;

  return IUP_DEFAULT ;
}

/* decelerate button callback */
int freia_cb(void)
{
  velocidade -= 0.0001 ;
  if(velocidade < 0.0)velocidade = 0.0 ;
	
  return IUP_DEFAULT ;
}

/* show button callback */
int exibe_cb(void)
{
  if(IupGetAttribute(gauge,"SHOW_TEXT") == "NO")
  {
    /* shows percentage in gauge */
    IupSetAttribute(gauge,"SHOW_TEXT","YES");
  }  
  else
  {
    /* does not show percentage in gauge */
    IupSetAttribute(gauge,"SHOW_TEXT","NO");
  }	
  return IUP_DEFAULT ;
}

/* main program */
int main(int argc, char **argv)
{
  char *error=NULL;
  
  /* IUP initialization */
  IupOpen(&argc, &argv);       
  IupControlsOpen () ;

  /* loads LED */
  if(error = IupLoad("iupgauge.led"))
  {
    IupMessage("LED error", error);
    return 1 ;
  }

  dlg = IupGetHandle("dialog_name");
  gauge = IupGetHandle("gauge_name");

  /* sets callbacks */
  IupSetFunction( "acao_pausa", (Icallback) pausa_cb );
  IupSetFunction( "acao_inicio", (Icallback) inicio_cb );
  IupSetFunction( "acao_acelera", (Icallback) acelera_cb );
  IupSetFunction( "acao_freia", (Icallback) freia_cb );
  IupSetFunction( "acao_exibe", (Icallback) exibe_cb );
  IupSetFunction( "IDLE_ACTION", (Icallback) idle_cb);
  
  /* shows dialog */
  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);

  /* main loop */
  IupMainLoop();

  IupDestroy(dlg);

  /* ends IUP */
  IupControlsClose() ;
  IupClose();

  return 0 ;
}