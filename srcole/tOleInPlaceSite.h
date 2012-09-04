// tOleInPlaceSite.h: interface for the tOleInPlaceSite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOLEINPLACESITE_H__37A5ADEB_91FB_11D3_A906_0004AC252223__INCLUDED_)
#define AFX_TOLEINPLACESITE_H__37A5ADEB_91FB_11D3_A906_0004AC252223__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include "tOleInPlaceFrame.h"

class tOleInPlaceSite : public IOleInPlaceSite  
{
protected:
  ULONG               m_cRef;
  class tOleHandler  *m_pTen;
  LPUNKNOWN           m_pUnkOuter;

public:
	tOleInPlaceFrame * m_oleinplaceframe;
  tOleInPlaceSite(class tOleHandler *, LPUNKNOWN);
  ~tOleInPlaceSite(void);

  STDMETHODIMP QueryInterface(REFIID, LPVOID*);
  STDMETHODIMP_(ULONG) AddRef(void);
  STDMETHODIMP_(ULONG) Release(void);

  STDMETHODIMP GetWindow(HWND *);
  STDMETHODIMP ContextSensitiveHelp(BOOL);
  STDMETHODIMP CanInPlaceActivate(void);
  STDMETHODIMP OnInPlaceActivate(void);
  STDMETHODIMP OnUIActivate(void);
  STDMETHODIMP GetWindowContext(LPOLEINPLACEFRAME *
                  , LPOLEINPLACEUIWINDOW *, LPRECT, LPRECT
                  , LPOLEINPLACEFRAMEINFO);
  STDMETHODIMP Scroll(SIZE);
  STDMETHODIMP OnUIDeactivate(BOOL);
  STDMETHODIMP OnInPlaceDeactivate(void);
  STDMETHODIMP DiscardUndoState(void);
  STDMETHODIMP DeactivateAndUndo(void);
  STDMETHODIMP OnPosRectChange(LPCRECT);
};

#endif // !defined(AFX_TOLEINPLACESITE_H__37A5ADEB_91FB_11D3_A906_0004AC252223__INCLUDED_)
