// tAmbientProperties.cpp: implementation of the tAmbientProperties class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "tAmbientProperties.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

tAmbientProperties::tAmbientProperties()
{
    //0x80000000 in OLE_COLOR indicates low byte is color index.
  m_clrBack=0x80000000+COLOR_WINDOW;
  m_clrFore=0x80000000+COLOR_WINDOWTEXT;
  m_lcid=LOCALE_USER_DEFAULT;
  m_fDesignMode = TRUE;

  m_pIOleControl = NULL;

#if 0
  {
    int cy=MulDiv(-14, LOMETRIC_PER_INCH, 72);
    //CHAPTER24MOD
    //End CHAPTER24MOD

    HFONT m_hFont=CreateFont(cy, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE
        , ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY
        , VARIABLE_PITCH | FF_SWISS, TEXT("Arial"));


  //m_hFont=(HFONT)GetStockObject(ANSI_VAR_FONT);

  //Need a standard font object for ambient properties

  FONTDESC    fd={sizeof(FONTDESC), L"Arial"
              , FONTSIZE(14), FW_NORMAL, DEFAULT_CHARSET
              , FALSE, FALSE, FALSE};

  OleCreateFontIndirect(
    &fd, IID_IFontDisp, (void **)&m_pIFont
    );
#endif
  m_pIFont = NULL;
}

tAmbientProperties::~tAmbientProperties()
{
  if(m_pIOleControl)
    m_pIOleControl->Release();

  if(m_pIFont)
    m_pIFont->Release();
}

void tAmbientProperties::setDesignMode(bool value, bool notify)
{
  switch(value)
  {
  case true:
    m_fDesignMode = FALSE;
    break;

  case false:
    m_fDesignMode = FALSE;
    break;
  }

  assert(m_pIOleControl);
  if(m_pIOleControl == NULL)
    return;


  // Notifica controle da mudanca

  if(notify)
    m_pIOleControl->OnAmbientPropertyChange(DISPID_AMBIENT_USERMODE);
}

OLE_COLOR tAmbientProperties::getBackColor()
{
  return m_clrBack;
}

OLE_COLOR tAmbientProperties::getForeColor()
{
  return m_clrFore;
}

bool tAmbientProperties::has_font()
{
  if(m_pIFont != NULL)
    return true;
  else
    return false;
}

IFontDisp * tAmbientProperties::getFontRef()
{
  m_pIFont->AddRef();
  return m_pIFont;
}

LCID tAmbientProperties::getLCID()
{
  return m_lcid;
}

BOOL tAmbientProperties::getDesignMode()
{
  return m_fDesignMode;
}

void tAmbientProperties::setForeColor(int red, 
                                      int green, 
                                      int blue, 
                                      bool notify)
{
  m_clrFore = blue << 16 | green << 8 | red;

  assert(m_pIOleControl);
  if(m_pIOleControl == NULL)
    return;

  if(notify)
    m_pIOleControl->
      OnAmbientPropertyChange(DISPID_AMBIENT_FORECOLOR);
}

void tAmbientProperties::setBackColor(int red,
                                      int green, 
                                      int blue, 
                                      bool notify)
{
  m_clrBack = blue << 16 | green << 8 | red;

  assert(m_pIOleControl);
  if(m_pIOleControl == NULL)
    return;

  if(notify)
    m_pIOleControl->
      OnAmbientPropertyChange(DISPID_AMBIENT_BACKCOLOR);
}

void tAmbientProperties::setControl(IOleControl * olecontrol)
{
  assert(olecontrol && m_pIOleControl == NULL);

  if(m_pIOleControl != NULL)
    return;

  olecontrol->AddRef();
  m_pIOleControl = olecontrol;
}
