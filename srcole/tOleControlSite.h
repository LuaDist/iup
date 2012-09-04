// tOleControlSite.h: interface for the tOleControlSite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOLECONTROLSITE_H__37A5ADEC_91FB_11D3_A906_0004AC252223__INCLUDED_)
#define AFX_TOLECONTROLSITE_H__37A5ADEC_91FB_11D3_A906_0004AC252223__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <olectl.h>

class tOleControlSite : public IOleControlSite  
{
    protected:
        ULONG           m_cRef;
        class tOleHandler *m_pTen;
        LPUNKNOWN       m_pUnkOuter;

    public:
        tOleControlSite(class tOleHandler *, LPUNKNOWN);
        ~tOleControlSite(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP OnControlInfoChanged(void);
        STDMETHODIMP LockInPlaceActive(BOOL);
        STDMETHODIMP GetExtendedControl(LPDISPATCH *);
        STDMETHODIMP TransformCoords(POINTL *, POINTF *, DWORD);
        STDMETHODIMP TranslateAccelerator(LPMSG, DWORD);
        STDMETHODIMP OnFocus(BOOL);
        STDMETHODIMP ShowPropertyFrame(void);
};

#endif // !defined(AFX_TOLECONTROLSITE_H__37A5ADEC_91FB_11D3_A906_0004AC252223__INCLUDED_)
