/** \file
 * \brief parser for LED. 
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>         

#include "iup.h"

#include "iup_object.h"
#include "iup_ledlex.h"
#include "iup_str.h"
#include "iup_assert.h"
            
            
#define IPARSE_SYMBEXIST       1
#define IPARSE_SYMBNOTDEF      2

static Ihandle* iParseExp (void);
static Ihandle* iParseFunction (Iclass *ic);
static int iParseError (int err, char *s);

static int iparse_error = 0;
#define IPARSE_RETURN_IF_ERRO(_e)        {iparse_error=(_e); if (iparse_error) return NULL;}
#define IPARSE_RETURN_IF_ERRO2(_e, _x)   {iparse_error=(_e); if (iparse_error) { if (_x) free(_x); return NULL;} }


char* IupLoad(const char *filename)
{
  iupASSERT(filename!=NULL);
  if (!filename)
    return "invalid file name";

  iparse_error = iupLexStart(filename, 1);
  if (iparse_error)
  {
    iupLexClose();
    return iupLexGetError();
  }

  while (iupLexLookAhead() != IUPLEX_TK_END)
  {
    iParseExp();
    if (iparse_error)
    {
      iupLexClose();
      return iupLexGetError();
    }
  }

  iupLexClose();
  return NULL;
}

char* IupLoadBuffer(const char *buffer)
{
  iupASSERT(buffer!=NULL);
  if (!buffer)
    return "invalid buffer";

  iparse_error = iupLexStart(buffer, 0);
  if (iparse_error)
  {
    iupLexClose();
    return iupLexGetError();
  }

  while (iupLexLookAhead() != IUPLEX_TK_END)
  {
    iParseExp();
    if (iparse_error)
    {
      iupLexClose();
      return iupLexGetError();
    }
  }

  iupLexClose();
  return NULL;
}

static Ihandle* iParseExp(void)
{
  char* nm = NULL;
  Ihandle* ih = NULL;

  int match = iupLexSeenMatch(IUPLEX_TK_FUNC,&iparse_error);
  IPARSE_RETURN_IF_ERRO(iparse_error);

  if (match)
    return iParseFunction(iupLexGetClass());

  if (iupLexLookAhead() == IUPLEX_TK_NAME)
  {
    nm = iupLexGetName();
    IPARSE_RETURN_IF_ERRO(iupLexAdvance());
  }
  else
  {
    iparse_error = iupLexMatch(IUPLEX_TK_NAME);
    return NULL;  /* force iparse_error */
  }

  match = iupLexSeenMatch(IUPLEX_TK_SET,&iparse_error); 
  IPARSE_RETURN_IF_ERRO(iparse_error);

  if (match)
  {
    ih = iParseExp();  
    IPARSE_RETURN_IF_ERRO(iparse_error);
    IupSetHandle(nm, ih);
  }
  else
  {
    ih = IupGetHandle(nm);
    if (!ih)
      IPARSE_RETURN_IF_ERRO(iParseError(IPARSE_SYMBNOTDEF,nm));
  }

  if (nm) free(nm);
  return ih;
}

static void* iParseControlParam(char type)
{
  switch(type)
  {
  case 'a':
    IPARSE_RETURN_IF_ERRO(iupLexMatch(IUPLEX_TK_NAME));
    return iupLexGetName();

  case 's':
    IPARSE_RETURN_IF_ERRO(iupLexMatch(IUPLEX_TK_STR));
    return iupLexGetName();

  case 'b':
  case 'c':
    IPARSE_RETURN_IF_ERRO(iupLexMatch(IUPLEX_TK_NAME));
    return (void*)(unsigned long)iupLexByte();

  case 'i':
  case 'j':
    IPARSE_RETURN_IF_ERRO(iupLexMatch(IUPLEX_TK_NAME));
    return (void*)(unsigned long)iupLexInt();

  case 'f':
    IPARSE_RETURN_IF_ERRO(iupLexMatch(IUPLEX_TK_NAME));
    {
      float f = iupLexFloat();
      unsigned long* l = (unsigned long*)&f;
      return (void*)*l;
    }

  case 'g':
  case 'h':
    {
      char *new_control = (char*)iParseExp();
      IPARSE_RETURN_IF_ERRO(iparse_error);
      return new_control;
    }

  default:
    return 0;
  }
}

static Ihandle* iParseControl(Iclass *ic)
{
  const char *format = ic->format;
  if (!format || format[0] == 0)
    return iupObjectCreate(ic, NULL); 
  else
  { 
    Ihandle *new_control;
    void** params;
    int i, alloc_arg, num_arg,
        num_format = strlen(format);
                      
    num_arg = num_format;
    alloc_arg = num_arg+20;
    params = (void**)malloc(sizeof(void*)*alloc_arg);

    for (i = 0; i < num_arg; )
    {
      char p_format = format[i];

      if (i > 0)
        IPARSE_RETURN_IF_ERRO2(iupLexMatch (IUPLEX_TK_COMMA), params);

      if (p_format != 'j' &&    /* not array */
          p_format != 'g' && 
          p_format != 'c')
      {
        params[i] = iParseControlParam(p_format);
        i++;
        IPARSE_RETURN_IF_ERRO2(iparse_error, params);
      }
      else    /* array */
      {
        int match;
        do
        {
          if (num_arg == i)
          {
            num_arg++;
            if (num_arg >= alloc_arg)
            {
              alloc_arg = num_arg+20;
              params = realloc(params, sizeof(void*)*alloc_arg);
            }
          }
          params[i] = iParseControlParam(p_format);
          i++;
          IPARSE_RETURN_IF_ERRO2(iparse_error, params);
          match = iupLexSeenMatch(IUPLEX_TK_COMMA,&iparse_error);
          IPARSE_RETURN_IF_ERRO2(iparse_error, params);
        } while (match); 

        /* after an array of parameters there are no more parameters */
        break;
      }
    }

    params[i] = NULL;
    new_control = iupObjectCreate(ic, params);

    for (i = 0; params[i] && i<num_format ; i++)
    {
      if (format[i] == 'j' ||
          format[i] == 'g' || 
          format[i] == 'c')
        break;

      if (format[i] == 'a' || format[i] == 's')
        free(params[i]);   /* iupLexGetName returned a duplicated string */
    }

    free(params);
    return new_control;
  }
}

static Ihandle* iParseFunction(Iclass *ic)
{
  Ihandle* ih = NULL;
  char *attr = NULL;

  int match = iupLexSeenMatch(IUPLEX_TK_ATTR,&iparse_error); 
  IPARSE_RETURN_IF_ERRO(iparse_error);

  if (match)
    attr = (char*)iupStrDup(iupLexName());

  IPARSE_RETURN_IF_ERRO(iupLexMatch(IUPLEX_TK_BEGP));

  ih = iParseControl(ic);
  IPARSE_RETURN_IF_ERRO(iparse_error);

  if (attr)
  {
    IupSetAttributes(ih, attr);
    free(attr);
  }

  IPARSE_RETURN_IF_ERRO(iupLexMatch(IUPLEX_TK_ENDP));
  return ih;
}

static int iParseError(int err, char *s)
{
  char msg[256] = "";

  switch (err)
  {
  case IPARSE_SYMBEXIST:
    sprintf(msg, "symbol '%s' already exists", s);
    break;
  case IUPLEX_NOTMATCH:
    sprintf(msg, "symbol '%s' not defined", s);
    break;
  }

  return iupLexError(IUPLEX_PARSEERROR, msg);
}
