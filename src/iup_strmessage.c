/** \file
 * \brief String Utilities
 *
 * See Copyright Notice in "iup.h"
 */

 
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <limits.h>  
#include <stdarg.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_strmessage.h"
#include "iup_table.h"


static Itable *istrmessage_table = NULL;   /* the function hast table indexed by the name string */

void iupStrMessageInit(void)
{
  istrmessage_table = iupTableCreate(IUPTABLE_STRINGINDEXED);
}

void iupStrMessageFinish(void)
{
  iupTableDestroy(istrmessage_table);
  istrmessage_table = NULL;
}

char* iupStrMessageGet(const char* message)
{
  return (char*)iupTableGet(istrmessage_table, message);
}

static void iStrMessageSet(const char* message, const char* str)
{
  iupTableSet(istrmessage_table, message, (char*)str, IUPTABLE_POINTER);
}


/**********************************************************************************/


void iupStrMessageShowError(Ihandle* parent, const char* message)
{
  Ihandle* dlg = IupMessageDlg();
  char* title = NULL, *str_message;

  if (parent)
  {
    IupSetAttributeHandle(dlg, "PARENTDIALOG", parent);
    title = IupGetAttribute(parent, "TITLE");
  }

  if (!title)
    title = iupStrMessageGet("IUP_ERROR");

  IupSetAttribute(dlg, "TITLE", title);
  IupSetAttribute(dlg, "DIALOGTYPE", "ERROR");
  IupSetAttribute(dlg, "BUTTONS", "OK");

  str_message = iupStrMessageGet(message);
  if (!str_message)
    str_message = (char*)message;
  IupSetAttribute(dlg, "VALUE", str_message);

  IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);

  IupDestroy(dlg);
}


/**********************************************************************************/

#define ISRTMSG_NUM_LNG 3    /* 2+1 for expansion */

typedef struct _IstdMessage
{
  const char* code;
  const char* lng_msg[ISRTMSG_NUM_LNG];
} IstdMessage;

/* Edit this table to add support for more languages */

static IstdMessage iStdMessages[] =
{
  {"IUP_ERROR", {"Error", "Erro", NULL}},
  {"IUP_YES", {"Yes", "Sim", NULL}},
  {"IUP_NO", {"No", "Não", NULL}},
  {"IUP_INVALIDDIR", {"Invalid directory.", "Diretório inválido.", NULL}},
  {"IUP_FILEISDIR", {"The selected name is a directory.", "O nome selecionado é um diretório.", NULL}},
  {"IUP_FILENOTEXIST", {"File does not exist.", "Arquivo inexistente.", NULL}},
  {"IUP_FILEOVERWRITE", {"Overwrite existing file?", "Sobrescrever arquivo?", NULL}},
  {"IUP_CREATEFOLDER", {"Create Folder", "Criar Diretório", NULL}},
  {"IUP_NAMENEWFOLDER", {"Name of the new folder:", "Nome do novo diretório:", NULL}},
  {"IUP_SAVEAS", {"Save As", "Salvar Como", NULL}},
  {"IUP_OPEN", {"Open", "Abrir", NULL}},
  {"IUP_SELECTDIR", {"Select Directory", "Selecionar Diretório", NULL}},
  {"IUP_OK", {"OK", "OK", NULL}},
  {"IUP_CANCEL", {"Cancel", "Cancelar", NULL}},
  {"IUP_GETCOLOR", {"Color Selection", "Seleção de Cor", NULL}},
  {"IUP_HELP", {"Help", "Ajuda", NULL}},
  {"IUP_RED", {"&Red:", "&Vermelho:", NULL}},
  {"IUP_GREEN", {"&Green:", "V&erde:", NULL}},
  {"IUP_BLUE", {"&Blue:", "&Azul:", NULL}},
  {"IUP_HUE", {"&Hue:", "&Matiz:", NULL}},
  {"IUP_SATURATION", {"&Saturation:", "&Saturação:", NULL}},
  {"IUP_INTENSITY", {"&Intensity:", "&Intensidade:", NULL}},
  {"IUP_OPACITY", {"&Opacity:", "&Opacidade:", NULL}},
  {"IUP_PALETTE", {"&Palette:", "&Paleta:", NULL}},
  {"IUP_TRUE", {"True", "Verdadeiro", NULL}},
  {"IUP_FALSE", {"False", "Falso", NULL}},
  {"IUP_FAMILY", {"Family:", "Família:", NULL}},
  {"IUP_STYLE", {"Style:", "Estilo:", NULL}},
  {"IUP_SIZE", {"Size:", "Tamanho:", NULL}},
  {"IUP_SAMPLE", {"Sample:", "Exemplo:", NULL}},
  {NULL, {NULL, NULL, NULL}}
};

static void iStrRegisterInternalMessages(int lng)
{
  IstdMessage* messages = iStdMessages;
  while (messages->code)
  {
    iStrMessageSet(messages->code, messages->lng_msg[lng]);
    messages++;
  }
}

void iupStrMessageUpdateLanguage(const char* language)
{
  int lng;
  if (iupStrEqualNoCase(language, "ENGLISH"))
    lng = 0;
  else if (iupStrEqualNoCase(language, "PORTUGUESE"))
    lng = 1;
  else
    lng = 0;  /* back to default */
  iStrRegisterInternalMessages(lng);
}

void IupSetLanguage(const char *language)
{
  IupStoreGlobal("LANGUAGE", language);
}

char *IupGetLanguage(void)
{
  return IupGetGlobal("LANGUAGE");
}
