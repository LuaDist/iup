// tDispatch.h: interface for the tDispatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TDISPATCH_H__37A5ADED_91FB_11D3_A906_0004AC252223__INCLUDED_)
#define AFX_TDISPATCH_H__37A5ADED_91FB_11D3_A906_0004AC252223__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>

class tDispatch : public IDispatch  
{
public:
    protected:
        ULONG           m_cRef;
        class tOleHandler  *m_pTen;
        LPUNKNOWN       m_pUnkOuter;

    public:
        tDispatch(class tOleHandler *, LPUNKNOWN);
        ~tDispatch(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP GetTypeInfoCount(UINT *);
        STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo **);
        STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR **, UINT
            , LCID, DISPID *);
        STDMETHODIMP Invoke(DISPID, REFIID, LCID, USHORT
            , DISPPARAMS *, VARIANT *, EXCEPINFO *, UINT *);


};

#endif // !defined(AFX_TDISPATCH_H__37A5ADED_91FB_11D3_A906_0004AC252223__INCLUDED_)
