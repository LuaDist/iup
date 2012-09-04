// tOleClientSite.h: interface for the tOleClientSite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOLECLIENTSITE_H__49B97C0A_8DEB_11D3_A906_0004ACE655F9__INCLUDED_)
#define AFX_TOLECLIENTSITE_H__49B97C0A_8DEB_11D3_A906_0004ACE655F9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>

class tOleHandler;

class tOleClientSite : public IOleClientSite  
{
public:
	tOleClientSite(LPUNKNOWN pUnkOuter, tOleHandler *olehandler);
	virtual ~tOleClientSite();

  STDMETHODIMP QueryInterface(REFIID, LPVOID *);
  STDMETHODIMP_(ULONG) AddRef(void);
  STDMETHODIMP_(ULONG) Release(void);

  STDMETHODIMP SaveObject(void);
  STDMETHODIMP GetMoniker(DWORD, DWORD, LPMONIKER *);
  STDMETHODIMP GetContainer(LPOLECONTAINER *);
  STDMETHODIMP ShowObject(void);
  STDMETHODIMP OnShowWindow(BOOL);
  STDMETHODIMP RequestNewObjectLayout(void);

protected:
	tOleHandler * handler;
  ULONG m_cRef;
  //class CTenant      *m_pTen;
  LPUNKNOWN m_pUnkOuter;
};

#endif // !defined(AFX_TOLECLIENTSITE_H__49B97C0A_8DEB_11D3_A906_0004ACE655F9__INCLUDED_)
