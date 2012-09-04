// tAmbientProperties.h: interface for the tAmbientProperties class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAMBIENTPROPERTIES_H__FC01B4C7_4918_11D4_9DF5_0000B45D6611__INCLUDED_)
#define AFX_TAMBIENTPROPERTIES_H__FC01B4C7_4918_11D4_9DF5_0000B45D6611__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <olectl.h>


class tAmbientProperties  
{
public:
	void setControl(IOleControl *olecontrol);
	void setBackColor(int red, int green, int blue, bool notify);
	void setForeColor(int red, int green, int blue, bool notify);
	BOOL getDesignMode(void);
	LCID getLCID(void);
	IFontDisp * getFontRef(void);
	bool has_font(void);
	OLE_COLOR getForeColor(void);
	OLE_COLOR getBackColor(void);
	void setDesignMode(bool value, bool notify);
	tAmbientProperties();
	virtual ~tAmbientProperties();

protected:

  // Utilizado para notificar mudancas nas propriedades
  LPOLECONTROL m_pIOleControl;

  // propriedades
  OLE_COLOR           m_clrBack;
  OLE_COLOR           m_clrFore;
  IFontDisp          *m_pIFont;       
  LCID                m_lcid;
  BOOL                m_fDesignMode;
  BOOL                m_fUIDead;
  BOOL                m_fHatchHandles;
};

#endif // !defined(AFX_TAMBIENTPROPERTIES_H__FC01B4C7_4918_11D4_9DF5_0000B45D6611__INCLUDED_)
