/** \file
 * \brief Text Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>
#include <commctrl.h>
#include <richedit.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_mask.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_array.h"
#include "iup_text.h"
#include "iup_key.h"
#include "iup_dialog.h"

#include "iupwin_drv.h"
#include "iupwin_handle.h"


/* Not defined for Cygwin and MingW */
#ifndef PFN_ARABIC
#define PFN_ARABIC		2
#define PFN_LCLETTER	3
#define	PFN_UCLETTER	4
#define	PFN_LCROMAN		5
#define	PFN_UCROMAN		6
#endif

#ifndef PFNS_PAREN
#define PFNS_PAREN		0x000
#define	PFNS_PARENS		0x100
#define PFNS_PERIOD		0x200
#define PFNS_PLAIN		0x300
#define PFNS_NONUMBER	0x400
#endif

#ifndef CFM_BACKCOLOR
#define CFM_BACKCOLOR		0x04000000
#define	CFM_UNDERLINETYPE	0x00800000
#define	CFM_WEIGHT			0x00400000
#define CFM_DISABLED		0x2000
#define	CFE_DISABLED		CFM_DISABLED
#endif

#ifndef CFU_UNDERLINEDOTTED
#define	CFU_UNDERLINEDOTTED				4
#define	CFU_UNDERLINEDOUBLE				3
#define CFU_UNDERLINE					1
#define CFU_UNDERLINENONE				0
#endif

#ifndef SES_UPPERCASE
#define SES_UPPERCASE			512
#define	SES_LOWERCASE			1024
#endif

#ifndef EM_SETCUEBANNER 
#define ECM_FIRST               0x1500      /* Edit control messages */
#define	EM_SETCUEBANNER	    (ECM_FIRST + 1)
#endif
/*   End Cygwin/MingW */


#define WM_IUPCARET WM_APP+1   /* Custom IUP message */


void iupdrvTextAddSpin(int *w, int h)
{
  *w += h;
}

void iupdrvTextAddBorders(int *w, int *h)
{
  int border_size = 2*3;
  (*w) += border_size;
  (*h) += border_size;
}

static void winTextParseParagraphFormat(Ihandle* formattag, PARAFORMAT2 *paraformat, int convert2twips)
{
  int val;
  char* format;

  ZeroMemory(paraformat, sizeof(PARAFORMAT2));
  paraformat->cbSize = sizeof(PARAFORMAT2);

  format = iupAttribGet(formattag, "NUMBERING");
  if (format)
  {
    paraformat->dwMask |= PFM_NUMBERING;

    if (iupStrEqualNoCase(format, "BULLET"))
      paraformat->wNumbering = PFN_BULLET;
    else if (iupStrEqualNoCase(format, "ARABIC"))
      paraformat->wNumbering = PFN_ARABIC;
    else if (iupStrEqualNoCase(format, "LCLETTER"))
      paraformat->wNumbering = PFN_LCLETTER;
    else if (iupStrEqualNoCase(format, "UCLETTER"))
      paraformat->wNumbering = PFN_UCLETTER;
    else if (iupStrEqualNoCase(format, "LCROMAN"))
      paraformat->wNumbering = PFN_LCROMAN;
    else if (iupStrEqualNoCase(format, "UCROMAN"))
      paraformat->wNumbering = PFN_UCROMAN;
    else
      paraformat->wNumbering = 0;  /* "NONE" */

    format = iupAttribGet(formattag, "NUMBERINGSTYLE");
    if (format)
    {
      paraformat->dwMask |= PFM_NUMBERINGSTYLE;

      if (iupStrEqualNoCase(format, "RIGHTPARENTESES"))
        paraformat->wNumberingStyle = PFNS_PAREN;
      else if (iupStrEqualNoCase(format, "PARENTESES"))
        paraformat->wNumberingStyle = PFNS_PARENS;
      else if (iupStrEqualNoCase(format, "PERIOD"))
        paraformat->wNumberingStyle = PFNS_PERIOD;
      else if (iupStrEqualNoCase(format, "NONUMBER"))
        paraformat->wNumberingStyle = PFNS_NONUMBER;
      else 
        paraformat->wNumberingStyle = PFNS_PLAIN;  /* "NONE" */
    }
   
    format = iupAttribGet(formattag, "NUMBERINGTAB");
    if (format && iupStrToInt(format, &val))
    {
      paraformat->dwMask |= PFM_NUMBERINGTAB;
      paraformat->wNumberingTab = (WORD)(val*convert2twips);
    }
  }

  format = iupAttribGet(formattag, "INDENT");
  if (format && iupStrToInt(format, &val))
  {
    paraformat->dwMask |= PFM_STARTINDENT|PFM_RIGHTINDENT|PFM_OFFSET;
    paraformat->dxStartIndent = val*convert2twips;

    format = iupAttribGet(formattag, "INDENTRIGHT");
    if (format && iupStrToInt(format, &val))
      paraformat->dxRightIndent = val*convert2twips;
    else
      paraformat->dxRightIndent = paraformat->dxStartIndent;
      
    format = iupAttribGet(formattag, "INDENTOFFSET");
    if (format && iupStrToInt(format, &val))
      paraformat->dxOffset = val*convert2twips;
    else
      paraformat->dxOffset = 0;
  }

  format = iupAttribGet(formattag, "ALIGNMENT");
  if (format)
  {
    paraformat->dwMask |= PFM_ALIGNMENT;

    if (iupStrEqualNoCase(format, "JUSTIFY"))
      paraformat->wAlignment = PFA_JUSTIFY;
    else if (iupStrEqualNoCase(format, "RIGHT"))
      paraformat->wAlignment = PFA_RIGHT;
    else if (iupStrEqualNoCase(format, "CENTER"))
      paraformat->wAlignment = PFA_CENTER;
    else
      paraformat->wAlignment = PFA_LEFT;  /* "LEFT" */
  }

  format = iupAttribGet(formattag, "TABSARRAY");
  if (format)
  {
    int pos, align, i = 0;
    LONG tab;
    char* str;

    paraformat->dwMask |= PFM_TABSTOPS;

    while (format)
    {
      str = iupStrCopyUntil((char**)&format, ' ');
      if (!str) break;
      pos = atoi(str)*convert2twips;
      free(str);

      str = iupStrCopyUntil((char**)&format, ' ');
      if (!str) break;

      if (iupStrEqualNoCase(str, "DECIMAL"))
        align = 3;
      else if (iupStrEqualNoCase(str, "RIGHT"))
        align = 2;
      else if (iupStrEqualNoCase(str, "CENTER"))
        align = 1;
      else /* "LEFT" */
        align = 0;
      free(str);

      tab = (pos&0xFFF)|(align<<24);
      paraformat->rgxTabs[i] = tab;
      i++;
      if (i == 32) break;
    }
    paraformat->cTabCount = (SHORT)i;
  }

  format = iupAttribGet(formattag, "SPACEBEFORE");
  if (format && iupStrToInt(format, &val))
  {
    paraformat->dwMask |= PFM_SPACEBEFORE;
    paraformat->dySpaceBefore = val*convert2twips;
  }

  format = iupAttribGet(formattag, "SPACEAFTER");
  if (format && iupStrToInt(format, &val))
  {
    paraformat->dwMask |= PFM_SPACEAFTER;
    paraformat->dySpaceAfter = val*convert2twips;
  }

  format = iupAttribGet(formattag, "LINESPACING");
  if (format)
  {
    paraformat->dwMask |= PFM_LINESPACING;

    if (iupStrEqualNoCase(format, "SINGLE"))
      paraformat->bLineSpacingRule = 0;
    else if (iupStrEqualNoCase(format, "ONEHALF"))
      paraformat->bLineSpacingRule = 1;
    else if (iupStrEqualNoCase(format, "DOUBLE"))
      paraformat->bLineSpacingRule = 2;
    else if (iupStrToInt(format, &val))
    {
      paraformat->bLineSpacingRule = 3;
      paraformat->dyLineSpacing = val*convert2twips;
    }
  }
}

static void winTextParseCharacterFormat(Ihandle* formattag, CHARFORMAT2 *charformat, int pixel2twips)
{
  int val;
  char* format;

  ZeroMemory(charformat, sizeof(CHARFORMAT2));
  charformat->cbSize = sizeof(CHARFORMAT2);

  format = iupAttribGet(formattag, "DISABLED");
  if (format)
  {
    charformat->dwMask |= CFM_DISABLED;
    if (iupStrBoolean(format))
      charformat->dwEffects |= CFE_DISABLED;
  }

  format = iupAttribGet(formattag, "RISE");
  if (format)
  {
    if (iupStrEqualNoCase(format, "SUPERSCRIPT"))
    {
      charformat->dwMask |= CFM_SUPERSCRIPT;
      charformat->dwEffects |= CFE_SUPERSCRIPT;
    }
    else if (iupStrEqualNoCase(format, "SUBSCRIPT"))
    {
      charformat->dwMask |= CFM_SUBSCRIPT;
      charformat->dwEffects |= CFE_SUBSCRIPT;
    } 
    else if (iupStrToInt(format, &val))
    {
      charformat->dwMask |= CFM_OFFSET;
      charformat->yOffset = val;
    }
  }

  format = iupAttribGet(formattag, "ITALIC");
  if (format)
  {
    charformat->dwMask |= CFM_ITALIC;
    if (iupStrBoolean(format))
      charformat->dwEffects |= CFE_ITALIC;
  }

  format = iupAttribGet(formattag, "STRIKEOUT");
  if (format)
  {
    charformat->dwMask |= CFM_STRIKEOUT;
    if (iupStrBoolean(format))
      charformat->dwEffects |= CFE_STRIKEOUT;
  }

  format = iupAttribGet(formattag, "PROTECTED");
  if (format)
  {
    charformat->dwMask |= CFM_PROTECTED;
    if (iupStrBoolean(format))
      charformat->dwEffects |= CFE_PROTECTED;
  }

  format = iupAttribGet(formattag, "FONTSIZE");
  if (format && iupStrToInt(format, &val))
  {
    /* (1/1440 of an inch, or 1/20 of a printer's point) */
    charformat->dwMask |= CFM_SIZE;
    if (val < 0)  /* in pixels */
      charformat->yHeight = (-val)*pixel2twips;
    else
      charformat->yHeight = val*20;
  }

  format = iupAttribGet(formattag, "FONTSCALE");
  if (format && charformat->yHeight != 0)
  {
    float fval = 0;
    if (iupStrEqualNoCase(format, "XX-SMALL"))
      fval = 0.5787037037037f;
    else if (iupStrEqualNoCase(format, "X-SMALL"))
      fval = 0.6444444444444f;
    else if (iupStrEqualNoCase(format, "SMALL"))
      fval = 0.8333333333333f;
    else if (iupStrEqualNoCase(format, "MEDIUM"))
      fval = 1.0f;
    else if (iupStrEqualNoCase(format, "LARGE"))
      fval = 1.2f;
    else if (iupStrEqualNoCase(format, "X-LARGE"))
      fval = 1.4399999999999f;
    else if (iupStrEqualNoCase(format, "XX-LARGE"))
      fval = 1.728f;
    else 
      iupStrToFloat(format, &fval);

    if (fval > 0)
    {
      fval = ((float)charformat->yHeight)*fval;
      charformat->yHeight = iupROUND(fval);
    }
  }

  format = iupAttribGet(formattag, "FONTFACE");
  if (format)
  {
    /* Map standard names to native names */
    const char* mapped_name = iupFontGetWinName(format);
    if (mapped_name)
      strcpy(charformat->szFaceName, mapped_name);
    else
      strcpy(charformat->szFaceName, format);
    charformat->dwMask |= CFM_FACE;
  }

  format = iupAttribGet(formattag, "FGCOLOR");
  if (format)
  {
    unsigned char r, g, b;
    if (iupStrToRGB(format, &r, &g, &b))
    {
      charformat->dwMask |= CFM_COLOR;
      charformat->crTextColor = RGB(r, g, b);
    }
  }

  format = iupAttribGet(formattag, "BGCOLOR");
  if (format)
  {
    unsigned char r, g, b;
    if (iupStrToRGB(format, &r, &g, &b))
    {
      charformat->dwMask |= CFM_BACKCOLOR;
      charformat->crBackColor = RGB(r, g, b);
    }
  }

  format = iupAttribGet(formattag, "UNDERLINE");
  if (format)
  {
    charformat->dwMask |= CFM_UNDERLINETYPE;

    if (iupStrEqualNoCase(format, "SINGLE"))
      charformat->bUnderlineType = CFU_UNDERLINE;
    else if (iupStrEqualNoCase(format, "DOUBLE"))
      charformat->bUnderlineType = CFU_UNDERLINEDOUBLE;
    else if (iupStrEqualNoCase(format, "DOTTED"))
      charformat->bUnderlineType = CFU_UNDERLINEDOTTED;
    else /* "NONE" */
      charformat->bUnderlineType = CFU_UNDERLINENONE;

    if (charformat->bUnderlineType != CFU_UNDERLINENONE)
    {
      charformat->dwMask |= CFM_UNDERLINE;
      charformat->dwEffects |= CFE_UNDERLINE;
    }
  }

  format = iupAttribGet(formattag, "WEIGHT");
  if (format)
  {
    charformat->dwMask |= CFM_WEIGHT;

    if (iupStrEqualNoCase(format, "EXTRALIGHT"))
      charformat->wWeight = FW_EXTRALIGHT;
    else if (iupStrEqualNoCase(format, "LIGHT"))
      charformat->wWeight = FW_LIGHT;
    else if (iupStrEqualNoCase(format, "SEMIBOLD"))
      charformat->wWeight = FW_SEMIBOLD;
    else if (iupStrEqualNoCase(format, "BOLD"))
      charformat->wWeight = FW_BOLD;
    else if (iupStrEqualNoCase(format, "EXTRABOLD"))
      charformat->wWeight = FW_EXTRABOLD;
    else if (iupStrEqualNoCase(format, "HEAVY"))
      charformat->wWeight = FW_HEAVY;
    else /* "NORMAL" */
      charformat->wWeight = FW_NORMAL;

    if (charformat->wWeight != FW_NORMAL)
    {
      charformat->dwMask |= CFM_BOLD;
      charformat->dwEffects |= CFE_BOLD;
    }
  }
}                      

static void winTextUpdateFontFormat(CHARFORMAT2* charformat, const char* value)
{
  int size = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  const char* mapped_name;

  if (!iupGetFontInfo(value, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return;

  /* Map standard names to native names */
  mapped_name = iupFontGetWinName(typeface);
  if (mapped_name)
    strcpy(typeface, mapped_name);

  charformat->dwMask |= CFM_FACE;
  strcpy(charformat->szFaceName, typeface);

  /* (1/1440 of an inch, or 1/20 of a printer's point) */
  charformat->dwMask |= CFM_SIZE;
  if (size < 0)  /* in pixels */
  {
    int pixel2twips = 1440/iupwinGetScreenRes();
    charformat->yHeight = (-size)*pixel2twips;
  }
  else
    charformat->yHeight = size*20;

  charformat->dwMask |= CFM_WEIGHT|CFM_BOLD;
  if (is_bold)
  {
    charformat->wWeight = FW_BOLD;
    charformat->dwEffects |= CFE_BOLD;
  }
  else
    charformat->wWeight = FW_NORMAL;

  charformat->dwMask |= CFM_ITALIC;
  if (is_italic)
    charformat->dwEffects |= CFE_ITALIC;

  charformat->dwMask |= CFM_UNDERLINETYPE;
  if (is_underline)
  {
    charformat->bUnderlineType = CFU_UNDERLINE;
    charformat->dwMask |= CFM_UNDERLINE;
    charformat->dwEffects |= CFE_UNDERLINE;
  }

  charformat->dwMask |= CFM_STRIKEOUT;
  if (is_strikeout)
    charformat->dwEffects |= CFE_STRIKEOUT;
}

static int winTextSetLinColToPosition(Ihandle *ih, int lin, int col)
{
  int linmax, colmax, lineindex;

  lin--; /* IUP starts at 1 */
  col--;
    
  linmax = SendMessage(ih->handle, EM_GETLINECOUNT, 0, 0L);
  if (lin > linmax-1)
    lin = linmax-1;

  lineindex = SendMessage(ih->handle, EM_LINEINDEX, (WPARAM)lin, 0L);

  colmax = SendMessage(ih->handle, EM_LINELENGTH, (WPARAM)lineindex,   0L);
  if (col > colmax)
    col = colmax;    /* after the last character */

  return lineindex + col;
}

static void winTextGetLinColFromPosition(Ihandle* ih, int pos, int* lin, int* col)
{
  /* here "pos" must contains the extra chars if the case */
  int lineindex;

  if (ih->data->has_formatting)
    *lin = SendMessage(ih->handle, EM_EXLINEFROMCHAR, (WPARAM)0, (LPARAM)pos);
  else
    *lin = SendMessage(ih->handle, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);

  lineindex = SendMessage(ih->handle, EM_LINEINDEX, (WPARAM)(*lin), (LPARAM)0L);
  *col = pos - lineindex;  /* lineindex is at the first character of the line */

  (*lin)++; /* IUP starts at 1 */
  (*col)++;
}

static int winTextRemoveExtraChars(Ihandle* ih, int pos)
{
  /* called only if not single line and not formatting */
  int lin = SendMessage(ih->handle, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);
  pos -= lin;  /* remove \r characters from count */
  return pos;
}

static int winTextAddExtraChars(Ihandle* ih, int pos)
{
  /* called only if not single line and not formatting */
  int lin, clin;

  clin = SendMessage(ih->handle, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);

  /* pos is smaller than the actual pos (missing the \r count),
     so we must calculate the line until the returned value is the same as the expected. */ 
  do
  {
    lin = clin;
    clin = SendMessage(ih->handle, EM_LINEFROMCHAR, (WPARAM)(pos+lin+1), (LPARAM)0L);   /* add one because we can be at the last character */
  } while (clin != lin);                                                                /* and it will not change to the next line by 1 */

  pos += lin;  /* add \r characters from count */
  return pos;
}

static int winTextGetCaretPos(Ihandle* ih)
{
  int pos = 0;
  POINT point;

  if (GetFocus() != ih->handle || !GetCaretPos(&point))
  {
    /* if does not have the focus, or could not get caret position,
       then use the selection start position */
    SendMessage(ih->handle, EM_GETSEL, (WPARAM)&pos, 0);
  }
  else
  {
    if (ih->data->has_formatting)
    {
      pos = SendMessage(ih->handle, EM_CHARFROMPOS, 0, (LPARAM)&point);
    }
    else if(point.x < 0 && point.y < 0)
    {
    /* if the caret position is located outside the visible client area,
       then use the selection start position */
      SendMessage(ih->handle, EM_GETSEL, (WPARAM)&pos, 0);
    }
    else
    {
      LRESULT ret;

      /* Workaround for weird behavior because of the return value in GetCaretPos */
      if (ih->data->is_multiline && point.y < 5)
        point.y += 5;

      ret = SendMessage(ih->handle, EM_CHARFROMPOS, 0, MAKELPARAM(point.x, point.y));
      pos = LOWORD(ret);
    }
  }

  return pos;
}

static int winTextGetCaret(Ihandle* ih, int *lin, int *col)
{
  int pos = winTextGetCaretPos(ih);

  if (ih->data->is_multiline)
  {
    winTextGetLinColFromPosition(ih, pos, lin, col);

    if (!ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
      pos = winTextRemoveExtraChars(ih, pos);
  }
  else
  {
    *col = pos;
    (*col)++;  /* IUP starts at 1 */
    *lin = 1;
  }

  return pos;
}

static void winTextGetSelection(Ihandle* ih, int *start, int *end)
{
  *start = 0;
  *end = 0;

  SendMessage(ih->handle, EM_GETSEL, (WPARAM)start, (LPARAM)end);

  if (ih->data->is_multiline && !ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
  {
    (*start) = winTextRemoveExtraChars(ih, *start);
    (*end) = winTextRemoveExtraChars(ih, *end);
  }
}

void iupdrvTextConvertLinColToPos(Ihandle* ih, int lin, int col, int *pos)
{
  *pos = winTextSetLinColToPosition(ih, lin, col);

  if (!ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
    *pos = winTextRemoveExtraChars(ih, *pos);
}

void iupdrvTextConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col)
{
  if (!ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
    pos = winTextAddExtraChars(ih, pos);

  winTextGetLinColFromPosition(ih, pos, lin, col);
}

static int winTextConvertXYToPos(Ihandle* ih, int x, int y)
{
  int pos;

  if (ih->data->has_formatting)
  {
    POINT point;
    point.x = x;
    point.y = y;
    pos = (int)SendMessage(ih->handle, EM_CHARFROMPOS, 0, (LPARAM)&point);
  }
  else
  {
    LRESULT ret = SendMessage(ih->handle, EM_CHARFROMPOS, 0, MAKELPARAM(x, y));
    pos = LOWORD(ret);
  }

  if (ih->data->is_multiline)
  {
    if (!ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
      pos = winTextRemoveExtraChars(ih, pos);
  }

  return pos;
}

static char* winTextStrConvert(Ihandle* ih, const char* str)
{
  if (ih->data->is_multiline)
  {
    if (ih->data->has_formatting)
    {
      if (strchr(str, '\n')!=NULL)
      {
        str = iupStrDup(str);
        iupStrToMac((char*)str);
      }
    }
    else
      str = iupStrToDos(str);
  }
  return (char*)str;
}

/***********************************************************************************************/


static int winTextSetValueAttrib(Ihandle* ih, const char* value)
{
  char* str;
  if (!value) value = "";
  str = winTextStrConvert(ih, value);
  ih->data->disable_callbacks = 1;
  SetWindowText(ih->handle, str);
  ih->data->disable_callbacks = 0;
  if (str != value) free(str);
  return 0;
}

static char* winTextGetValueAttrib(Ihandle* ih)
{
  int count = GetWindowTextLength(ih->handle);
  if (count)
  {
    char* str = iupStrGetMemory(count+1);
    GetWindowText(ih->handle, str, count+1);  /* notice that this function always returns in DOS format */
    if (ih->data->is_multiline)
      iupStrToUnix(str);
    return str;
  }
  else
    return "";
}

static int winTextSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &(ih->data->horiz_padding), &(ih->data->vert_padding), 'x');
  ih->data->vert_padding = 0;
  if (ih->handle)
  {
    SendMessage(ih->handle, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELPARAM(ih->data->horiz_padding, ih->data->horiz_padding));
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int winTextSetSelectedTextAttrib(Ihandle* ih, const char* value)
{
  if (value)
  {
    int start = 0, end = 0;
    char* str;
    
    SendMessage(ih->handle, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
    if (start == end)
      return 0;

    str = winTextStrConvert(ih, value);
    SendMessage(ih->handle, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)str);
    if (str != value) free(str);
  }
  return 0;
}

static char* winTextGetSelectedTextAttrib(Ihandle* ih)
{
  int count = GetWindowTextLength(ih->handle);
  if (count)
  {
    int start = 0, end = 0;
    char* str;
    
    SendMessage(ih->handle, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
    if (start == end)
      return NULL;

    if (ih->data->has_formatting)
    {
      str = iupStrGetMemory(end-start+1);
      SendMessage(ih->handle, EM_GETSELTEXT, 0, (LPARAM)str);
    }
    else
    {
      str = iupStrGetMemory(count+1);
      GetWindowText(ih->handle, str, count+1);  /* notice that this function always returns in DOS format */
      /* returns only the selected text */
      str[end] = 0;
      str += start;
    }

    if (ih->data->is_multiline)
      iupStrToUnix(str);
    return str;
  }
  else
    return NULL;
}

static int winTextSetNCAttrib(Ihandle* ih, const char* value)
{
  if (!iupStrToInt(value, &ih->data->nc))
    ih->data->nc = 0;

  if (ih->handle)
  {
    if (ih->data->has_formatting)
      SendMessage(ih->handle, EM_EXLIMITTEXT, 0, ih->data->nc);   /* so it can be larger than 64k */
    else
      SendMessage(ih->handle, EM_LIMITTEXT, ih->data->nc, 0L);

    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static char* winTextGetCountAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(50);
  int count = GetWindowTextLength(ih->handle);
  if (ih->data->is_multiline)
  {
    int linecount = SendMessage(ih->handle, EM_GETLINECOUNT, 0, 0L);
    count -= linecount-1;  /* ignore 1 character at each line */
  }
  sprintf(str, "%d", count);
  return str;
}

static char* winTextGetLineCountAttrib(Ihandle* ih)
{
  if (ih->data->is_multiline)
  {
    char* str = iupStrGetMemory(50);
    int linecount = SendMessage(ih->handle, EM_GETLINECOUNT, 0, 0L);
    sprintf(str, "%d", linecount);
    return str;
  }
  else
    return "1";
}

static char* winTextGetLineValueAttrib(Ihandle* ih)
{
  if (ih->data->is_multiline)
  {
    int len, lin, col;
    char* str = iupStrGetMemory(200);
    WORD* wstr = (WORD*)str;
    *wstr = 200;
    winTextGetCaret(ih, &lin, &col); 
    lin--; /* from IUP to Win */
    len = SendMessage(ih->handle, EM_GETLINE, (WPARAM)lin, (LPARAM)str);
    str[len]=0;
    return str;
  }
  else
    return winTextGetValueAttrib(ih);
}

static int winTextSetSelectionAttrib(Ihandle* ih, const char* value)
{
  int start=1, end=1;

  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    SendMessage(ih->handle, EM_SETSEL, (WPARAM)-1, (LPARAM)0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    SendMessage(ih->handle, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
    return 0;
  }

  if (ih->data->is_multiline)
  {
    int lin_start=1, col_start=1, lin_end=1, col_end=1;

    if (sscanf(value, "%d,%d:%d,%d", &lin_start, &col_start, &lin_end, &col_end)!=4) return 0;
    if (lin_start<1 || col_start<1 || lin_end<1 || col_end<1) return 0;

    start = winTextSetLinColToPosition(ih, lin_start, col_start);
    end = winTextSetLinColToPosition(ih, lin_end, col_end);
  }
  else
  {
    if (iupStrToIntInt(value, &start, &end, ':')!=2) 
      return 0;

    if(start<1 || end<1) 
      return 0;

    start--; /* IUP starts at 1 */
    end--;
  }

  SendMessage(ih->handle, EM_SETSEL, (WPARAM)start, (LPARAM)end);

  return 0;
}

static char* winTextGetSelectionAttrib(Ihandle* ih)
{
  int start = 0, end = 0;
  char* str;

  SendMessage(ih->handle, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
  if (start == end)
    return NULL;

  str = iupStrGetMemory(100);

  if (ih->data->is_multiline)
  {
    int start_col, start_lin, end_col, end_lin;
    winTextGetLinColFromPosition(ih, start, &start_lin, &start_col);
    winTextGetLinColFromPosition(ih, end,   &end_lin,   &end_col);
    sprintf(str,"%d,%d:%d,%d", start_lin, start_col, end_lin, end_col);
  }
  else
  {
    start++; /* IUP starts at 1 */
    end++;
    sprintf(str, "%d:%d", start, end);
  }

  return str;
}

static int winTextSetSelectionPosAttrib(Ihandle* ih, const char* value)
{
  int start=0, end=0;

  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    SendMessage(ih->handle, EM_SETSEL, (WPARAM)-1, (LPARAM)0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    SendMessage(ih->handle, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
    return 0;
  }

  if (iupStrToIntInt(value, &start, &end, ':')!=2) 
    return 0;

  if(start<0 || end<0) 
    return 0;

  if (ih->data->is_multiline && !ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
  {
    start = winTextAddExtraChars(ih, start);
    end = winTextAddExtraChars(ih, end);
  }

  SendMessage(ih->handle, EM_SETSEL, (WPARAM)start, (LPARAM)end);

  return 0;
}

static char* winTextGetSelectionPosAttrib(Ihandle* ih)
{
  int start = 0, end = 0;
  char* str;

  SendMessage(ih->handle, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
  if (start == end)
    return NULL;

  if (ih->data->is_multiline && !ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
  {
    start = winTextRemoveExtraChars(ih, start);
    end = winTextRemoveExtraChars(ih, end);
  }

  str = iupStrGetMemory(100);

  sprintf(str, "%d:%d", start, end);

  return str;
}

static int winTextSetInsertAttrib(Ihandle* ih, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  if (value)
  {
    char* str = winTextStrConvert(ih, value);
    SendMessage(ih->handle, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)str);
    if (str != value) free(str);
  }
  return 0;
}

static int winTextSetAppendAttrib(Ihandle* ih, const char* value)
{
  int pos;
  char* str;
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  if (!value) value = "";
  str = winTextStrConvert(ih, value);
  
  pos = GetWindowTextLength(ih->handle)+1;
  SendMessage(ih->handle, EM_SETSEL, (WPARAM)pos, (LPARAM)pos);
  if (ih->data->is_multiline && ih->data->append_newline && pos!=1)
  {
    if (ih->data->has_formatting)
      SendMessage(ih->handle, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)"\r");
    else
      SendMessage(ih->handle, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)"\r\n");
  }
  SendMessage(ih->handle, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)str);

  if (str != value) free(str);
  return 0;
}

static int winTextSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
  SendMessage(ih->handle, EM_SETREADONLY, (WPARAM)iupStrBoolean(value), 0);
  return 0;
}

static char* winTextGetReadOnlyAttrib(Ihandle* ih)
{
  DWORD style = GetWindowLong(ih->handle, GWL_STYLE);
  if (style & ES_READONLY)
    return "YES";
  else
    return "NO";
}

static int winTextSetTabSizeAttrib(Ihandle* ih, const char* value)
{
  int tabsize;
  if (!ih->data->is_multiline)
    return 0;

  iupStrToInt(value, &tabsize);
  tabsize *= 4;
  SendMessage(ih->handle, EM_SETTABSTOPS, (WPARAM)1L, (LPARAM)&tabsize);
  iupdrvRedrawNow(ih);
  return 1;
}

static int winTextSetCaretAttrib(Ihandle* ih, const char* value)
{
  int pos = 1;

  if (!value)
    return 0;

  if (ih->data->is_multiline)
  {
    int lin = 1, col = 1;
    iupStrToIntInt(value, &lin, &col, ',');  /* be permissive in SetCaret, do not abort if invalid */
    if (lin < 1) lin = 1;
    if (col < 1) col = 1;

    pos = winTextSetLinColToPosition(ih, lin, col);
  }
  else
  {
    sscanf(value,"%i",&pos);
    if (pos < 1) pos = 1;
    pos--; /* IUP starts at 1 */
  }

  SendMessage(ih->handle, EM_SETSEL, (WPARAM)pos, (LPARAM)pos);
  SendMessage(ih->handle, EM_SCROLLCARET, 0L, 0L);

  return 0;
}

static char* winTextGetCaretAttrib(Ihandle* ih)
{
  int col, lin;
  char* str;

  str = iupStrGetMemory(100);

  winTextGetCaret(ih, &lin, &col);

  if (ih->data->is_multiline)
    sprintf(str, "%d,%d", lin, col);
  else
    sprintf(str, "%d", col);

  return str;
}

static int winTextSetCaretPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);    /* be permissive in SetCaret, do not abort if invalid */
  if (pos < 0) pos = 0;

  if (ih->data->is_multiline && !ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
    pos = winTextAddExtraChars(ih, pos);

  SendMessage(ih->handle, EM_SETSEL, (WPARAM)pos, (LPARAM)pos);
  SendMessage(ih->handle, EM_SCROLLCARET, 0L, 0L);

  return 0;
}

static char* winTextGetCaretPosAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(100);
  int pos = winTextGetCaretPos(ih);
  if (ih->data->is_multiline && !ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
    pos = winTextRemoveExtraChars(ih, pos);
  sprintf(str, "%d", pos);
  return str;
}

static void winTextScrollTo(Ihandle* ih, int lin, int col)
{
  DWORD old_lin = SendMessage(ih->handle, EM_GETFIRSTVISIBLELINE, 0, 0);
  if (ih->data->has_formatting)
    SendMessage(ih->handle, EM_LINESCROLL, (WPARAM)0, (LPARAM)(lin-old_lin));
  else  /* How to get the current horizontal position?????  */
    SendMessage(ih->handle, EM_LINESCROLL, (WPARAM)col, (LPARAM)(lin-old_lin));
}

static int winTextSetScrollToAttrib(Ihandle* ih, const char* value)
{
  int lin = 1, col = 1;

  if (!value)
    return 0;

  if (ih->data->is_multiline)
  {
    iupStrToIntInt(value, &lin, &col, ',');
    if (lin < 1) lin = 1;
    if (col < 1) col = 1;
  }
  else
  {
    sscanf(value,"%i",&col);
    if (col < 1) col = 1;
  }

  lin--;  /* return to Windows referece */
  col--;

  winTextScrollTo(ih, lin, col);

  return 0;
}

static int winTextSetScrollToPosAttrib(Ihandle* ih, const char* value)
{
  int lin, col, pos = 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 0) pos = 0;

  if (ih->data->is_multiline && !ih->data->has_formatting)  /* when formatting or single line text uses only one char per line end */
    pos = winTextAddExtraChars(ih, pos);

  winTextGetLinColFromPosition(ih, pos, &lin, &col);
  lin--;  /* return to Windows referece */
  col--;

  winTextScrollTo(ih, lin, col);

  return 0;
}

static int winTextSetFilterAttrib(Ihandle *ih, const char *value)
{
  int style = 0;

  if (iupStrEqualNoCase(value, "LOWERCASE"))
  {
    if (ih->data->has_formatting)
    {
      SendMessage(ih->handle, EM_SETEDITSTYLE, SES_LOWERCASE, SES_LOWERCASE);
      return 1;
    }
    style = ES_LOWERCASE;
  }
  else if (iupStrEqualNoCase(value, "NUMBER"))
    style = ES_NUMBER;
  else if (iupStrEqualNoCase(value, "UPPERCASE"))
  {
    if (ih->data->has_formatting)
    {
      SendMessage(ih->handle, EM_SETEDITSTYLE, SES_UPPERCASE, SES_UPPERCASE);
      return 1;
    }
    style = ES_UPPERCASE;
  }

  if (style)
    iupwinMergeStyle(ih, ES_LOWERCASE|ES_NUMBER|ES_UPPERCASE, style);

  return 1;
}

static int winTextSetClipboardAttrib(Ihandle *ih, const char *value)
{
  UINT msg = 0;

  if (iupStrEqualNoCase(value, "COPY"))
    msg = WM_COPY;
  else if (iupStrEqualNoCase(value, "CUT"))
    msg = WM_CUT;
  else if (iupStrEqualNoCase(value, "PASTE"))
    msg = WM_PASTE;
  else if (iupStrEqualNoCase(value, "CLEAR"))
    msg = WM_CLEAR;
  else if (iupStrEqualNoCase(value, "UNDO"))
    msg = WM_UNDO;
  else if (ih->data->has_formatting && iupStrEqualNoCase(value, "REDO"))
    msg = EM_REDO;

  if (msg)
    SendMessage(ih->handle, msg, 0, 0);

  return 0;
}

static int winTextSetFgColorAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  iupdrvPostRedraw(ih);
  return 1;
}

static int winTextSetBgColorAttrib(Ihandle *ih, const char *value)
{
  if (ih->data->has_formatting)
  {
    unsigned char r, g, b;
    if (iupStrToRGB(value, &r, &g, &b))
    {
      COLORREF color;
      color = RGB(r,g,b);
      SendMessage(ih->handle, EM_SETBKGNDCOLOR, 0, (LPARAM)color);
    }
  }
  iupdrvPostRedraw(ih);
  return 1;
}

static int winTextSetCueBannerAttrib(Ihandle *ih, const char *value)
{
  if (!ih->data->is_multiline && iupwin_comctl32ver6)
  {
    WCHAR* wstr = iupwinStrChar2Wide(value);
    SendMessage(ih->handle, EM_SETCUEBANNER, (WPARAM)FALSE, (LPARAM)wstr);
    free(wstr);
    return 1;
  }
  return 0;
}

static int winTextSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  int new_style;

  if (iupStrEqualNoCase(value, "ARIGHT"))
    new_style = ES_RIGHT;
  else if (iupStrEqualNoCase(value, "ACENTER"))
    new_style = ES_CENTER;
  else /* "ALEFT" */
    new_style = ES_LEFT;

  iupwinMergeStyle(ih, ES_LEFT|ES_CENTER|ES_RIGHT, new_style);

  return 1;
}

static int winTextSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->has_formatting && ih->handle)
  {
    if (!iupAttribGet(ih, "_IUPWIN_FONTUPDATECHECK"))
    {
      /* avoid setting the same font because this will clear the font formattags. */
      char* cur_value = iupGetFontAttrib(ih);
      if (iupStrEqual(value, cur_value))
        return 0;
    }
    iupAttribSetStr(ih, "_IUPWIN_FONTUPDATECHECK", NULL);
  }

  return iupdrvSetStandardFontAttrib(ih, value);
}

typedef struct
{
  int eventMask;
  DWORD line;
  CHARRANGE oldRange;
} formatTagBulkState;

void* iupdrvTextAddFormatTagStartBulk(Ihandle* ih)
{
  formatTagBulkState* state = (formatTagBulkState*) malloc(sizeof(formatTagBulkState));

  state->line = SendMessage(ih->handle, EM_GETFIRSTVISIBLELINE, 0, 0);  /* save scrollbar */
  SendMessage(ih->handle, EM_EXGETSEL, 0, (LPARAM)&state->oldRange);  /* save selection */
  state->eventMask = SendMessage(ih->handle, EM_SETEVENTMASK, 0, 0);  /* disable events */
  SendMessage(ih->handle, WM_SETREDRAW, FALSE, 0);  /* disable redraw */

  return state;
}

void iupdrvTextAddFormatTagStopBulk(Ihandle* ih, void* stateOpaque)
{
  formatTagBulkState* state = (formatTagBulkState*) stateOpaque;
  DWORD line = SendMessage(ih->handle, EM_GETFIRSTVISIBLELINE, 0, 0);

  SendMessage(ih->handle, EM_EXSETSEL, 0, (LPARAM)&state->oldRange);
  SendMessage(ih->handle, EM_LINESCROLL, 0, state->line - line);
  SendMessage(ih->handle, EM_SETEVENTMASK, 0, state->eventMask);
  SendMessage(ih->handle, WM_SETREDRAW, TRUE, 0);

  free(state);

  iupdrvRedrawNow(ih);
}

void iupdrvTextAddFormatTag(Ihandle* ih, Ihandle* formattag, int bulk)
{
  int convert2twips, pixel2twips;
  char *selection, *units;
  PARAFORMAT2 paraformat;
  CHARFORMAT2 charformat;
  formatTagBulkState* state = NULL;

  /* one twip is 1/1440 inch */
  /* twip = (pixel*1440)/(pixel/inch) */
  pixel2twips = 1440/iupwinGetScreenRes();

  /* default is PIXELS */
  convert2twips = pixel2twips;
  units = iupAttribGet(formattag, "UNITS");
  if (units)
  {
    int val;
    if (iupStrEqualNoCase(units, "TWIPS"))
      convert2twips = 1;
    else if (iupStrToInt(units, &val))
      convert2twips = val;
  }

  /* save state here if not applying a bulk */
  if (!bulk) 
    state = (formatTagBulkState*)iupdrvTextAddFormatTagStartBulk(ih);

  selection = iupAttribGet(formattag, "SELECTION");
  if (selection)
  {
    /* In Windows, the format message use the current selection */
    winTextSetSelectionAttrib(ih, selection);
  }
  else
  {
    char* selectionpos = iupAttribGet(formattag, "SELECTIONPOS");
    if (selectionpos)
    {
      /* In Windows, the format message use the current selection */
      winTextSetSelectionPosAttrib(ih, selectionpos);
    }
  }

  if (iupAttribGet(formattag, "FONTSCALE") && !iupAttribGet(formattag, "FONTSIZE"))
    iupAttribSetStr(formattag, "FONTSIZE", iupGetFontSizeAttrib(ih));

  winTextParseParagraphFormat(formattag, &paraformat, convert2twips);
  if (paraformat.dwMask != 0)
    SendMessage(ih->handle, EM_SETPARAFORMAT, 0, (LPARAM)&paraformat);

  winTextParseCharacterFormat(formattag, &charformat, pixel2twips);
  if (charformat.dwMask != 0)
    SendMessage(ih->handle, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&charformat);

  /* restore state here if not applying a bulk */
  if (!bulk) 
    iupdrvTextAddFormatTagStopBulk(ih, state);
}

static int winTextSetRemoveFormattingAttrib(Ihandle* ih, const char* value)
{
  PARAFORMAT2 paraformat;
  CHARFORMAT2 charformat;
  COLORREF colorref;

  if (!ih->data->has_formatting)
    return 0;

  ZeroMemory(&paraformat, sizeof(PARAFORMAT2));
  paraformat.cbSize = sizeof(PARAFORMAT2);
  paraformat.dwMask = PFM_NUMBERING|PFM_STARTINDENT|PFM_RIGHTINDENT|PFM_OFFSET|
                      PFM_ALIGNMENT|PFM_SPACEBEFORE|PFM_SPACEAFTER|PFM_LINESPACING;
  paraformat.wAlignment = PFA_LEFT;

  ZeroMemory(&charformat, sizeof(CHARFORMAT2));
  charformat.cbSize = sizeof(CHARFORMAT2);
  charformat.dwMask = CFM_DISABLED|CFM_OFFSET|CFM_PROTECTED;

  winTextUpdateFontFormat(&charformat, iupGetFontAttrib(ih));

  if (iupwinGetColorRef(ih, "FGCOLOR", &colorref))
  {
    charformat.dwMask |= CFM_COLOR;
    charformat.crTextColor = colorref;
  }

  if (iupwinGetColorRef(ih, "BGCOLOR", &colorref))
  {
    charformat.dwMask |= CFM_BACKCOLOR;
    charformat.crBackColor = colorref;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    CHARRANGE oldRange;
    SendMessage(ih->handle, EM_EXGETSEL, 0, (LPARAM)&oldRange);
    SendMessage(ih->handle, EM_SETSEL, (WPARAM)-1, (LPARAM)0);
    SendMessage(ih->handle, EM_SETPARAFORMAT, 0, (LPARAM)&paraformat);  /* always for the current selection */
    SendMessage(ih->handle, EM_EXSETSEL, 0, (LPARAM)&oldRange);

    SendMessage(ih->handle, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&charformat);
  }
  else
  {
    SendMessage(ih->handle, EM_SETPARAFORMAT, 0, (LPARAM)&paraformat);
    SendMessage(ih->handle, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&charformat);
  }

  return 0;
}

static int winTextSetOverwriteAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_formatting)
    return 0;

  if (iupAttribGetBoolean(ih, "OVERWRITE"))
  {
    if (!iupStrBoolean(value))
      SendMessage(ih->handle, WM_KEYDOWN, VK_INSERT, 0);  /* toggle from ON to OFF */
  }
  else
  {
    if (iupStrBoolean(value))
      SendMessage(ih->handle, WM_KEYDOWN, VK_INSERT, 0);  /* toggle from OFF to ON */
  }
  return 1;
}


static int winTextSetVisibleAttrib(Ihandle* ih, const char* value)
{
  HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
  if (hSpin)
    ShowWindow(hSpin, iupStrBoolean(value)? SW_SHOWNORMAL: SW_HIDE);

  return iupBaseSetVisibleAttrib(ih, value);
}

static void winTextCropSpinValue(Ihandle* ih, HWND hSpin, int min, int max)
{
  /* refresh if internally cropped, but text still shows an invalid value */
  int pos = SendMessage(hSpin, UDM_GETPOS32, 0, 0);
  ih->data->disable_callbacks = 1;
  if (pos <= min)  
    SendMessage(hSpin, UDM_SETPOS32, 0, min);
  if (pos >= max)
    SendMessage(hSpin, UDM_SETPOS32, 0, max);
  ih->data->disable_callbacks = 0;
}

static int winTextSetSpinMinAttrib(Ihandle* ih, const char* value)
{
  HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
  if (hSpin)
  {
    int min;
    if (iupStrToInt(value, &min))
    {
      int max = iupAttribGetInt(ih, "SPINMAX");
      SendMessage(hSpin, UDM_SETRANGE32, min, max);

      winTextCropSpinValue(ih, hSpin, min, max);
    }
  }
  return 1;
}

static int winTextSetSpinMaxAttrib(Ihandle* ih, const char* value)
{
  HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
  if (hSpin)
  {
    int max;
    if (iupStrToInt(value, &max))
    {
      int min = iupAttribGetInt(ih, "SPINMIN");
      SendMessage(hSpin, UDM_SETRANGE32, min, max);

      winTextCropSpinValue(ih, hSpin, min, max);
    }
  }
  return 1;
}

static int winTextSetSpinIncAttrib(Ihandle* ih, const char* value)
{
  HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
  if (hSpin)
  {
    int inc;
    if (iupStrToInt(value, &inc))
    {
      UDACCEL paAccels[3];
      paAccels[0].nInc = inc;
      paAccels[0].nSec = 0;
      paAccels[1].nInc = inc*5;
      paAccels[1].nSec = 2;
      paAccels[2].nInc = inc*20;
      paAccels[2].nSec = 5;
      SendMessage(hSpin, UDM_SETACCEL, 3, (LPARAM)paAccels);
    }
  }
  return 1;
}

static int winTextSetSpinValueAttrib(Ihandle* ih, const char* value)
{
  HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
  if (hSpin)
  {
    int pos;
    if (iupStrToInt(value, &pos))
    {
      ih->data->disable_callbacks = 1;
      SendMessage(hSpin, UDM_SETPOS32, 0, pos);
      ih->data->disable_callbacks = 0;
    }
  }
  return 1;
}

static char* winTextGetSpinValueAttrib(Ihandle* ih)
{
  HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
  if (hSpin)
  {
    int pos = SendMessage(hSpin, UDM_GETPOS32, 0, 0);
    char *str = iupStrGetMemory(50);
    sprintf(str, "%d", pos);
    return str;
  }
  return NULL;
}


/****************************************************************************************/


static int winTextCtlColor(Ihandle* ih, HDC hdc, LRESULT *result)
{
  COLORREF cr;

  if (iupwinGetColorRef(ih, "FGCOLOR", &cr))
    SetTextColor(hdc, cr);

  if (iupwinGetColorRef(ih, "BGCOLOR", &cr))
  {
    SetBkColor(hdc, cr);
    SetDCBrushColor(hdc, cr);
    *result = (LRESULT)GetStockObject(DC_BRUSH);
    return 1;
  }
  return 0;
}

static void winTextCallCaretCb(Ihandle* ih)
{
  int col, lin, pos;

  IFniii cb = (IFniii)IupGetCallback(ih, "CARET_CB");
  if (!cb) return;

  pos = winTextGetCaret(ih, &lin, &col);

  if (pos != ih->data->last_caret_pos)
  {
    ih->data->last_caret_pos = pos;

    cb(ih, lin, col, pos);
  }
}

static int winTextCallActionCb(Ihandle* ih, const char* insert_value, int key, int dir)
{
  int start, end, ret = 1;
  char *value, *new_value;

  IFnis cb = (IFnis)IupGetCallback(ih, "ACTION");
  if (!cb && !ih->data->mask)
    return 1;

  winTextGetSelection(ih, &start, &end);

  value = winTextGetValueAttrib(ih);

  if (value[0]==0)
    new_value = iupStrDup(insert_value);
  else if (insert_value)
    new_value = iupStrInsert(value, insert_value, start, end);
  else
  {
    new_value = value;
    iupStrRemove(value, start, end, dir);
  }

  if (!new_value)
    return 0; /* abort */

  if (ih->data->nc && (int)strlen(new_value) > ih->data->nc)
  {
    if (new_value != value) free(new_value);
    return 0; /* abort */
  }

  if (ih->data->mask && iupMaskCheck(ih->data->mask, new_value)==0)
  {
    if (new_value != value) free(new_value);
    return 0; /* abort */
  }

  if (cb)
  {
    int cb_ret = cb(ih, key, (char*)new_value);
    if (cb_ret==IUP_IGNORE)
      ret = 0;     /* abort processing */
    else if (cb_ret==IUP_CLOSE)
    {
      IupExitLoop();
      ret = 0;     /* abort processing */
    }
    else if (cb_ret!=0 && key!=0 && 
             cb_ret != IUP_DEFAULT && cb_ret != IUP_CONTINUE)  
    {
      WNDPROC oldProc = (WNDPROC)IupGetCallback(ih, "_IUPWIN_OLDPROC_CB");
      CallWindowProc(oldProc, ih->handle, WM_CHAR, cb_ret, 0);  /* replace key */
      ret = 0;     /* abort processing */
    }
  }

  if (new_value != value) free(new_value);
  return ret;
}

static int winTextSpinWmNotify(Ihandle* ih, NMHDR* msg_info, int *result)
{
  if (msg_info->code == UDN_DELTAPOS)
  {
    NMUPDOWN *updown = (NMUPDOWN*)msg_info;
    HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
    int pos = updown->iPos+updown->iDelta;
    int min, max;
    SendMessage(hSpin, UDM_GETRANGE32, (WPARAM)&min, (LPARAM)&max);
    if (pos>=min && pos<=max)
    {
      IFni cb = (IFni) IupGetCallback(ih, "SPIN_CB");
      if (cb) 
      {
        int ret = cb(ih, pos);
        if (ret == IUP_IGNORE)
        {
          *result = 1;
          return 1;
        }
      }
    }
  }
  
  (void)result;
  return 0; /* result not used */
}

static int winTextProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{
  int ret = 0;

  if (msg==WM_KEYDOWN) /* process K_ANY before text callbacks */
  {
    ret = iupwinBaseProc(ih, msg, wp, lp, result);
    if (ret) 
    {
      iupAttribSetStr(ih, "_IUPWIN_IGNORE_CHAR", "1");
      *result = 0;
      return 1;
    }
    else
      iupAttribSetStr(ih, "_IUPWIN_IGNORE_CHAR", NULL);
  }

  switch (msg)
  {
  case WM_CHAR:
    {
      /* even aborting WM_KEYDOWN, a WM_CHAR will be sent, so ignore it also */
      /* if a dialog was shown, the loop will be processed, so ignore out of focus WM_CHAR messages */
      if (GetFocus() != ih->handle || iupAttribGet(ih, "_IUPWIN_IGNORE_CHAR"))
      {
        iupAttribSetStr(ih, "_IUPWIN_IGNORE_CHAR", NULL);
        *result = 0;
        return 1;
      }

      if ((char)wp == '\b')
      {              
        if (!winTextCallActionCb(ih, NULL, 0, -1))
          ret = 1;
      }
      else if ((char)wp == '\n' || (char)wp == '\r')
      {
        if (ih->data->is_multiline && 
            !ih->data->has_formatting && !(GetKeyState(VK_CONTROL) & 0x8000)) /* when formatting is processed in WM_KEYDOWN */
        {
          char insert_value[2];
          insert_value[0] = '\n';
          insert_value[1] = 0;

          if (!winTextCallActionCb(ih, insert_value, wp, 1))
            ret = 1;
        }
      }
      else if (!(GetKeyState(VK_CONTROL) & 0x8000 ||
                 GetKeyState(VK_MENU) & 0x8000 ||
                 GetKeyState(VK_LWIN) & 0x8000 || 
                 GetKeyState(VK_RWIN) & 0x8000))
      {
        char insert_value[2];
        insert_value[0] = (char)wp;
        insert_value[1] = 0;

        if (!winTextCallActionCb(ih, insert_value, wp, 1))
          ret = 1;
      }

      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);

      if (!ih->data->is_multiline && 
          (wp==VK_RETURN || wp==VK_ESCAPE || wp==VK_TAB))  /* the keys have the same definitions as the chars */
        ret = 1;  /* abort default processing to avoid beep */

      if (ih->data->is_multiline && 
          (wp=='\n' && (GetKeyState(VK_CONTROL) & 0x8000)))
        ret = 1;  /* abort default processing to avoid inserting a new line */

      break;
    }
  case WM_KEYDOWN:
    {
      if (wp == VK_DELETE) /* Del does not generates a WM_CHAR */
      {
        if (!winTextCallActionCb(ih, NULL, 0, 1))
          ret = 1;
      }
      else if (wp == VK_INSERT && ih->data->has_formatting)
      {
        if (iupAttribGetBoolean(ih, "OVERWRITE"))
          iupAttribSetStr(ih, "OVERWRITE", "OFF"); /* toggle from ON to OFF */
        else
          iupAttribSetStr(ih, "OVERWRITE", "ON");  /* toggle from OFF to ON */
      }
      else if (wp == 'A' && GetKeyState(VK_CONTROL) & 0x8000)   /* Ctrl+A = Select All */
      {
        SendMessage(ih->handle, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
      }
      else if (wp == VK_RETURN && ih->data->has_formatting && !(GetKeyState(VK_CONTROL) & 0x8000))
      {
        char insert_value[2];
        insert_value[0] = '\n';
        insert_value[1] = 0;

        if (!winTextCallActionCb(ih, insert_value, '\n', 1))
          ret = 1;
      }

      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);

      if (ret)       /* if abort processing, then the result is 0 */
      {
        *result = 0;
        return 1;
      }
      else
        return 0;  /* already processed at the begining of this function */
    }
  case WM_KEYUP:
    {
      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_CLEAR:
    {
      if (!winTextCallActionCb(ih, NULL, 0, 1))
        ret = 1;

      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_CUT:
    {
      if (!winTextCallActionCb(ih, NULL, 0, 1))
        ret = 1;

      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_PASTE:
    {
      if (IupGetCallback(ih,"ACTION") || ih->data->mask) /* test before to avoid alocate clipboard text memory */
      {
        char* insert_value = iupwinGetClipboardText(ih);
        if (insert_value)
        {
          if (!winTextCallActionCb(ih, insert_value, 0, 1))
            ret = 1;
          free(insert_value);
        }
      }

      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_UNDO:
    {
      IFnis cb = (IFnis)IupGetCallback(ih, "ACTION");
      if (cb)
      {
        char* value;
        WNDPROC oldProc = (WNDPROC)IupGetCallback(ih, "_IUPWIN_OLDPROC_CB");
        CallWindowProc(oldProc, ih->handle, WM_UNDO, 0, 0);

        value = winTextGetValueAttrib(ih);
        cb(ih, 0, (char*)value);

        ret = 1;
      }

      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_LBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
    {
      if (iupwinButtonDown(ih, msg, wp, lp)==-1)
      {
        *result = 0;
        return 1;
      }
      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
  case WM_LBUTTONUP:
    {
      if (iupwinButtonUp(ih, msg, wp, lp)==-1)
      {
        *result = 0;
        return 1;
      }
      PostMessage(ih->handle, WM_IUPCARET, 0, 0L);
      break;
    }
  case WM_IUPCARET:
    {
      winTextCallCaretCb(ih);
      break;
    }
  case WM_MOUSEMOVE:
    {
      iupwinMouseMove(ih, msg, wp, lp);
      break;
    }
  case WM_VSCROLL:
  case WM_HSCROLL:
    {
      if (ih->data->has_formatting)
      {
        /* fix weird behavior when dialog has COMPOSITE=YES, 
           scrollbars are not updated when dragging */
        if (LOWORD(wp) == SB_THUMBTRACK)
          SendMessage(ih->handle, EM_SHOWSCROLLBAR, msg==WM_VSCROLL? SB_VERT: SB_HORZ, TRUE);
      }
      break;
    }
  }

  if (ret)       /* if abort processing, then the result is 0 */
  {
    *result = 0;
    return 1;
  }
  else
    return iupwinBaseProc(ih, msg, wp, lp, result);
}

static void winTextCreateSpin(Ihandle* ih)
{
  HWND hSpin;
  DWORD dwStyle = WS_CHILD|WS_CLIPSIBLINGS|UDS_ARROWKEYS|UDS_HOTTRACK|UDS_NOTHOUSANDS;
  int serial = iupDialogGetChildId(ih);

  if (iupStrEqualNoCase(iupAttribGetStr(ih, "SPINALIGN"), "LEFT"))
    dwStyle |= UDS_ALIGNLEFT;
  else
    dwStyle |= UDS_ALIGNRIGHT;

  if (iupAttribGetBoolean(ih, "SPINWRAP"))
    dwStyle |= UDS_WRAP;

  if (iupAttribGetBoolean(ih, "SPINAUTO"))
    dwStyle |= UDS_SETBUDDYINT;

  hSpin = CreateWindowEx(0, /* extended window style */
    UPDOWN_CLASS,           /* window class */
    NULL,                   /* title */
    dwStyle,                /* window style */
    0,                      /* x-position */
    0,                      /* y-position */
    10,                     /* default width to avoid 0 */
    10,                     /* default height to avoid 0 */
    GetParent(ih->handle),  /* window parent */
    (HMENU)serial,          /* child identifier */
    iupwin_hinstance,       /* instance of app. */
    NULL);

  if (!hSpin)
    return;

  iupwinHandleAdd(ih, hSpin);

  /* Process WM_NOTIFY */
  IupSetCallback(ih, "_IUPWIN_NOTIFY_CB", (Icallback)winTextSpinWmNotify);

  SendMessage(hSpin, UDM_SETBUDDY, (WPARAM)ih->handle, 0);
  iupAttribSetStr(ih, "_IUPWIN_SPIN", (char*)hSpin);

  /* default values */
  ih->data->disable_callbacks = 1;
  SendMessage(hSpin, UDM_SETRANGE32, 0, 100);
  SendMessage(hSpin, UDM_SETPOS32, 0, 0);
  ih->data->disable_callbacks = 0;
}

static int winTextWmCommand(Ihandle* ih, WPARAM wp, LPARAM lp)
{
  int cmd = HIWORD(wp);
  switch (cmd)
  {
  case EN_CHANGE:
    {
      if (ih->data->disable_callbacks)
        return 0;

      iupBaseCallValueChangedCb(ih);
      break;
    }
  }

  (void)lp;
  return 0; /* not used */
}

static void winTextLayoutUpdateMethod(Ihandle* ih)
{
  HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
  if (hSpin)
  {
    if (iupStrEqualNoCase(iupAttribGetStr(ih, "SPINALIGN"), "LEFT"))
    {
      SetWindowPos(ih->handle, NULL, ih->x+ih->currentheight-1, ih->y, ih->currentwidth-ih->currentheight, ih->currentheight,
                   SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);

      SetWindowPos(hSpin, NULL, ih->x, ih->y, ih->currentheight, ih->currentheight,
                   SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
    }
    else
    {
      SetWindowPos(ih->handle, NULL, ih->x, ih->y, ih->currentwidth-ih->currentheight, ih->currentheight,
                   SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);

      SetWindowPos(hSpin, NULL, ih->x+ih->currentwidth-ih->currentheight-1, ih->y, ih->currentheight, ih->currentheight,
                   SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
    }
  }
  else
    iupdrvBaseLayoutUpdateMethod(ih);
}

static void winTextUnMapMethod(Ihandle* ih)
{
  HWND hSpin = (HWND)iupAttribGet(ih, "_IUPWIN_SPIN");
  if (hSpin)
  {
    iupwinHandleRemove(hSpin);
    DestroyWindow(hSpin);
  }

  iupdrvBaseUnMapMethod(ih);
}

static int winTextMapMethod(Ihandle* ih)
{
  DWORD dwStyle = WS_CHILD|WS_CLIPSIBLINGS, 
      dwExStyle = 0;
  char* winclass = "EDIT", *value;

  if (!ih->parent)
    return IUP_ERROR;

  if (iupAttribGetBoolean(ih, "CANFOCUS"))
    dwStyle |= WS_TABSTOP;

  if (ih->data->has_formatting)
  {
    /* enable richedit 3.0 */
    static HMODULE richedit = NULL;
    if (!richedit)
      richedit = LoadLibrary("Riched20.dll");
    if (!richedit)
      return IUP_ERROR;

    winclass = RICHEDIT_CLASS;
  }

  if (ih->data->is_multiline)
  {
    dwStyle |= ES_AUTOVSCROLL|ES_MULTILINE|ES_WANTRETURN;

    if (iupAttribGetBoolean(ih, "WORDWRAP"))
    {
      ih->data->sb &= ~IUP_SB_HORIZ;  /* must remove the horizontal scroolbar */
                                      /* and do not specify ES_AUTOHSCROLL, the control automatically wraps words */
    }
    else                           
      dwStyle |= ES_AUTOHSCROLL;   

    if (ih->data->sb & IUP_SB_HORIZ)
      dwStyle |= WS_HSCROLL;
    if (ih->data->sb & IUP_SB_VERT)
      dwStyle |= WS_VSCROLL;

    if (ih->data->has_formatting && ih->data->sb != IUP_SB_NONE)
    {
      if (!iupAttribGetBoolean(ih, "AUTOHIDE"))
        dwStyle |= ES_DISABLENOSCROLL;
    }
  }
  else
  {
    dwStyle |= ES_AUTOHSCROLL;

    if (iupAttribGetBoolean(ih, "PASSWORD"))
      dwStyle |= ES_PASSWORD;
  }

  value = iupAttribGet(ih, "ALIGNMENT");
  if (value)
  {
    if (iupStrEqualNoCase(value, "ARIGHT"))
      dwStyle |= ES_RIGHT;
    else if (iupStrEqualNoCase(value, "ACENTER"))
      dwStyle |= ES_CENTER;
    else /* default "ALEFT" */
      dwStyle |= ES_LEFT;
  }

  if (iupAttribGetBoolean(ih, "BORDER"))
    dwExStyle |= WS_EX_CLIENTEDGE;

  if (!iupwinCreateWindowEx(ih, winclass, dwExStyle, dwStyle))
    return IUP_ERROR;

  /* Process ACTION_CB and CARET_CB */
  IupSetCallback(ih, "_IUPWIN_CTRLPROC_CB", (Icallback)winTextProc);

  /* Process background color */
  IupSetCallback(ih, "_IUPWIN_CTLCOLOR_CB", (Icallback)winTextCtlColor);

  /* Process WM_COMMAND */
  IupSetCallback(ih, "_IUPWIN_COMMAND_CB", (Icallback)winTextWmCommand);

  /* set defaults */
  SendMessage(ih->handle, EM_LIMITTEXT, 0, 0L);
  {
    int tabsize = 8*4;
    SendMessage(ih->handle, EM_SETTABSTOPS, (WPARAM)1L, (LPARAM)&tabsize);
  }

  if (!ih->data->is_multiline && iupAttribGetBoolean(ih, "SPIN"))
    winTextCreateSpin(ih);

  /* configure for DROP of files */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSetStr(ih, "DROPFILESTARGET", "YES");

  if (ih->data->has_formatting)
  {
    SendMessage(ih->handle, EM_SETTEXTMODE, (WPARAM)(TM_RICHTEXT|TM_MULTILEVELUNDO|TM_SINGLECODEPAGE), 0);
    SendMessage(ih->handle, EM_SETEVENTMASK, 0, ENM_CHANGE);

    /* must update FONT before updating the formattags */
    iupAttribSetStr(ih, "_IUPWIN_FONTUPDATECHECK", "1");
    iupUpdateStandardFontAttrib(ih);

    if (ih->data->formattags)
      iupTextUpdateFormatTags(ih);
  }

  IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)winTextConvertXYToPos);

  return IUP_NOERROR;
}

void iupdrvTextInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = winTextMapMethod;
  ic->LayoutUpdate = winTextLayoutUpdateMethod;
  ic->UnMap = winTextUnMapMethod;

  /* Driver Dependent Attribute functions */

  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, winTextSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, winTextSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);  
  iupClassRegisterAttribute(ic, "VISIBLE", iupBaseGetVisibleAttrib, winTextSetVisibleAttrib, "YES", "NO", IUPAF_NO_SAVE|IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, winTextSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_NOT_MAPPED);  /* usually black */    
  iupClassRegisterAttribute(ic, "AUTOREDRAW", NULL, iupwinSetAutoRedrawAttrib, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* IupText only */
  iupClassRegisterAttribute(ic, "PADDING", iupTextGetPaddingAttrib, winTextSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "VALUE", winTextGetValueAttrib, winTextSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINEVALUE", winTextGetLineValueAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", winTextGetSelectedTextAttrib, winTextSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", winTextGetSelectionAttrib, winTextSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", winTextGetSelectionPosAttrib, winTextSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", winTextGetCaretAttrib, winTextSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", winTextGetCaretPosAttrib, winTextSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, winTextSetInsertAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, winTextSetAppendAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "READONLY", winTextGetReadOnlyAttrib, winTextSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "NC", iupTextGetNCAttrib, winTextSetNCAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, winTextSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, winTextSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, winTextSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMIN", NULL, winTextSetSpinMinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINMAX", NULL, winTextSetSpinMaxAttrib, IUPAF_SAMEASSYSTEM, "100", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPININC", NULL, winTextSetSpinIncAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINVALUE", winTextGetSpinValueAttrib, winTextSetSpinValueAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", winTextGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINECOUNT", winTextGetLineCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* IupText Windows and GTK only */
  iupClassRegisterAttribute(ic, "ADDFORMATTAG", NULL, iupTextSetAddFormatTagAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ADDFORMATTAG_HANDLE", NULL, iupTextSetAddFormatTagHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, winTextSetAlignmentAttrib, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORMATTING", iupTextGetFormattingAttrib, iupTextSetFormattingAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OVERWRITE", NULL, winTextSetOverwriteAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REMOVEFORMATTING", NULL, winTextSetRemoveFormattingAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABSIZE", NULL, winTextSetTabSizeAttrib, IUPAF_SAMEASSYSTEM, "8", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "PASSWORD", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* IupText Windows only */
  iupClassRegisterAttribute(ic, "CUEBANNER", NULL, winTextSetCueBannerAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTER", NULL, winTextSetFilterAttrib, NULL, NULL, IUPAF_NO_INHERIT);
}
