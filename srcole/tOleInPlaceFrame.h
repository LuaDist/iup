// tOleInPlaceFrame.h: interface for the tOleInPlaceFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOLEINPLACEFRAME_H__76ABD328_9D90_11D3_A906_0004ACE655F9__INCLUDED_)
#define AFX_TOLEINPLACEFRAME_H__76ABD328_9D90_11D3_A906_0004ACE655F9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "tLegacy.h"

/*
 * UINT value such that adding one produces zero.  Portable to Win32.
 * This is used to represent a non-existent zero-based UINT value
 */
#define NOVALUE                     ((UINT)-1)


//PATRON.CPP:  Frame object that creates a main window

class tOleInPlaceFrame : public IOleInPlaceFrame
    {
    private:
        BOOL            m_fInitialized;     //OleInitialize worked

        DWORD           m_cRef;
        HWND            m_hWnd;          //Object window

        BOOL            m_fInContextHelp;   //In context help mode?

        LPOLEINPLACEACTIVEOBJECT    m_pIOleIPActiveObject;

    //This is for access from IOleInPlaceSite::GetWindowContext
    public:
        HACCEL          m_hAccelIP;         //Accelerators for in-place

    protected:

    public:
        tOleInPlaceFrame(HWND);
        virtual ~tOleInPlaceFrame(void);

        //IOleInPlaceFrame implementation
        STDMETHODIMP         QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP         GetWindow(HWND *);
        STDMETHODIMP         ContextSensitiveHelp(BOOL);
        STDMETHODIMP         GetBorder(LPRECT);
        STDMETHODIMP         RequestBorderSpace(LPCBORDERWIDTHS);
        STDMETHODIMP         SetBorderSpace(LPCBORDERWIDTHS);
        STDMETHODIMP         SetActiveObject(LPOLEINPLACEACTIVEOBJECT
                                 , LPCOLESTR);
        STDMETHODIMP         InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS);
        STDMETHODIMP         SetMenu(HMENU, HOLEMENU, HWND);
        STDMETHODIMP         RemoveMenus(HMENU);
        STDMETHODIMP         SetStatusText(LPCOLESTR);
        STDMETHODIMP         EnableModeless(BOOL);
        STDMETHODIMP         TranslateAccelerator(LPMSG, WORD);
    };



#endif // !defined(AFX_TOLEINPLACEFRAME_H__76ABD328_9D90_11D3_A906_0004ACE655F9__INCLUDED_)
