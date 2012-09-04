// tOleInPlaceFrame.cpp: implementation of the tOleInPlaceFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "tOleInPlaceFrame.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
 * tOleInPlaceFrame::tOleInPlaceFrame
 * tOleInPlaceFrame::~tOleInPlaceFrame
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE from WinMain
 *  hInstPrev       HINSTANCE from WinMain
 *  pszCmdLine      LPSTR from WinMain
 *  nCmdShow        int from WInMain
 */

tOleInPlaceFrame::tOleInPlaceFrame(HWND hwnd)
    {
    m_cRef=0;
    m_hAccelIP=NULL;
    m_fInContextHelp=FALSE;
    m_pIOleIPActiveObject=NULL;
    m_hWnd = hwnd;

    return;
    }


tOleInPlaceFrame::~tOleInPlaceFrame(void)
    {
    }


/*
 * tOleInPlaceFrame::QueryInterface
 * tOleInPlaceFrame::AddRef
 * tOleInPlaceFrame::Release
 */

STDMETHODIMP tOleInPlaceFrame::QueryInterface(REFIID riid, PPVOID ppv)
    {
    //We only know IUnknown and IOleInPlaceFrame
    *ppv=NULL;

    if (IID_IUnknown==riid || IID_IOleInPlaceUIWindow==riid
        || IID_IOleWindow==riid || IID_IOleInPlaceFrame==riid)
        *ppv=(LPOLEINPLACEFRAME)this;

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) tOleInPlaceFrame::AddRef(void)
    {
    return ++m_cRef;
    }

STDMETHODIMP_(ULONG) tOleInPlaceFrame::Release(void)
    {
    /*
     * We don't do anything with this since we're not controlled
     * by a reference count as far as in-place stuff is concerned.
     */
    return --m_cRef;
    }




/*
 * tOleInPlaceFrame::GetWindow
 *
 * Purpose:
 *  Retrieves the handle of the window associated with the object
 *  on which this interface is implemented.
 *
 * Parameters:
 *  phWnd           HWND * in which to store the window handle.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, E_FAIL if there is no
 *                  window.
 */

STDMETHODIMP tOleInPlaceFrame::GetWindow(HWND *phWnd)
    {
    *phWnd=m_hWnd;
    return NOERROR;
    }




/*
 * tOleInPlaceFrame::ContextSensitiveHelp
 *
 * Purpose:
 *  Instructs the object on which this interface is implemented to
 *  enter or leave a context-sensitive help mode.
 *
 * Parameters:
 *  fEnterMode      BOOL TRUE to enter the mode, FALSE otherwise.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceFrame::ContextSensitiveHelp(BOOL fEnterMode)
    {
    /*
     * Don't bother if there is no active object since we don't do
     * context help on our own.
     */
    if (NULL==m_pIOleIPActiveObject)
        return NOERROR;

    /*
     * If the state changes, an MDI frame should call the same
     * function in all documents.  An SDI frame should just call
     * the active object, if it has one.
     */

    if (m_fInContextHelp!=fEnterMode)
        {
        m_fInContextHelp=fEnterMode;

        m_pIOleIPActiveObject->ContextSensitiveHelp(fEnterMode);

        }

    return NOERROR;
    }




/*
 * tOleInPlaceFrame::GetBorder
 *
 * Purpose:
 *  Returns the rectangle in which the container is willing to
 *  negotiate about an object's adornments.
 *
 * Parameters:
 *  prcBorder       LPRECT in which to store the rectangle.
 *
 * Return Value:
 *  HRESULT         NOERROR if all is well, INPLACE_E_NOTOOLSPACE
 *                  if there is no negotiable space.
 */

STDMETHODIMP tOleInPlaceFrame::GetBorder(LPRECT prcBorder)
    {
    if (NULL==prcBorder)
        return ResultFromScode(E_INVALIDARG);

    /*
     * We return all the client area space sans the StatStrip,
     * which we control
     */
    GetClientRect(m_hWnd, prcBorder);

    return NOERROR;
    }




/*
 * tOleInPlaceFrame::RequestBorderSpace
 *
 * Purpose:
 *  Asks the container if it can surrender the amount of space
 *  in pBW that the object would like for it's adornments.  The
 *  container does nothing but validate the spaces on this call.
 *
 * Parameters:
 *  pBW             LPCBORDERWIDTHS containing the requested space.
 *                  The values are the amount of space requested
 *                  from each side of the relevant window.
 *
 * Return Value:
 *  HRESULT         NOERROR if we can give up space,
 *                  INPLACE_E_NOTOOLSPACE otherwise.
 */

STDMETHODIMP tOleInPlaceFrame::RequestBorderSpace(LPCBORDERWIDTHS pBW)
    {
    //Everything is fine with us, so always return an OK.
    return NOERROR;
    }




/*
 * tOleInPlaceFrame::SetBorderSpace
 *
 * Purpose:
 *  Called when the object now officially requests that the
 *  container surrender border space it previously allowed
 *  in RequestBorderSpace.  The container should resize windows
 *  appropriately to surrender this space.
 *
 * Parameters:
 *  pBW             LPCBORDERWIDTHS containing the amount of space
 *                  from each side of the relevant window that the
 *                  object is now reserving.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceFrame::SetBorderSpace(LPCBORDERWIDTHS pBW)
    {
    return NOERROR;
    }




/*
 * tOleInPlaceFrame::SetActiveObject
 *
 * Purpose:
 *  Provides the container with the object's IOleInPlaceActiveObject
 *  pointer
 *
 * Parameters:
 *  pIIPActiveObj   LPOLEINPLACEACTIVEOBJECT of interest.
 *  pszObj          LPCOLESTR naming the object.  Not used.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceFrame::SetActiveObject
    (LPOLEINPLACEACTIVEOBJECT pIIPActiveObj, LPCOLESTR pszObj)
    {
    if (NULL!=m_pIOleIPActiveObject)
        m_pIOleIPActiveObject->Release();

    //NULLs m_pIOleIPActiveObject if pIIPActiveObj is NULL
    m_pIOleIPActiveObject=pIIPActiveObj;

    if (NULL!=m_pIOleIPActiveObject)
        m_pIOleIPActiveObject->AddRef();

    return NOERROR;
    }




/*
 * tOleInPlaceFrame::InsertMenus
 *
 * Purpose:
 *  Instructs the container to place its in-place menu items where
 *  necessary in the given menu and to fill in elements 0, 2, and 4
 *  of the OLEMENUGROUPWIDTHS array to indicate how many top-level
 *  items are in each group.
 *
 * Parameters:
 *  hMenu           HMENU in which to add popups.
 *  pMGW            LPOLEMENUGROUPWIDTHS in which to store the
 *                  width of each container menu group.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceFrame::InsertMenus(HMENU hMenu
    , LPOLEMENUGROUPWIDTHS pMGW)
    {
    return NOERROR;
    }




/*
 * tOleInPlaceFrame::SetMenu
 *
 * Purpose:
 *  Instructs the container to replace whatever menu it's currently
 *  using with the given menu and to call OleSetMenuDescritor so OLE
 *  knows to whom to dispatch messages.
 *
 * Parameters:
 *  hMenu           HMENU to show.
 *  hOLEMenu        HOLEMENU to the menu descriptor.
 *  hWndObj         HWND of the active object to which messages are
 *                  dispatched.
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceFrame::SetMenu(HMENU hMenu
    , HOLEMENU hOLEMenu, HWND hWndObj)
    {
    HRESULT         hr;

    /*
     * Our responsibilities here are to put the menu on the frame
     * window and call OleSetMenuDescriptor.
     * CPatronClient::SetMenu which we call here takes care of
     * MDI/SDI differences.
     *
     * We also want to save the object's hWnd for use in WM_SETFOCUS
     * processing.
     */

    return NOERROR;
    }




/*
 * tOleInPlaceFrame::RemoveMenus
 *
 * Purpose:
 *  Asks the container to remove any menus it put into hMenu in
 *  InsertMenus.
 *
 * Parameters:
 *  hMenu           HMENU from which to remove the container's
 *                  items.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceFrame::RemoveMenus(HMENU hMenu)
    {
    return NOERROR;
    }




/*
 * tOleInPlaceFrame::SetStatusText
 *
 * Purpose:
 *  Asks the container to place some text in a status line, if one
 *  exists.  If the container does not have a status line it
 *  should return E_FAIL here in which case the object could
 *  display its own.
 *
 * Parameters:
 *  pszText         LPCOLESTR to display.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_TRUNCATED if not all
 *                  of the text could be displayed, or E_FAIL if
 *                  the container has no status line.
 */

STDMETHODIMP tOleInPlaceFrame::SetStatusText(LPCOLESTR pszText)
    {
    return NOERROR;
    }




/*
 * tOleInPlaceFrame::EnableModeless
 *
 * Purpose:
 *  Instructs the container to show or hide any modeless popup
 *  windows that it may be using.
 *
 * Parameters:
 *  fEnable         BOOL indicating to enable/show the windows
 *                  (TRUE) or to hide them (FALSE).
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceFrame::EnableModeless(BOOL fEnable)
    {
    return NOERROR;
    }




/*
 * tOleInPlaceFrame::TranslateAccelerator
 *
 * Purpose:
 *  When dealing with an in-place object from an EXE server, this
 *  is called to give the container a chance to process accelerators
 *  after the server has looked at the message.
 *
 * Parameters:
 *  pMSG            LPMSG for the container to examine.
 *  wID             WORD the identifier in the container's
 *                  accelerator table (from IOleInPlaceSite
 *                  ::GetWindowContext) for this message (OLE does
 *                  some translation before calling).
 *
 * Return Value:
 *  HRESULT         NOERROR if the keystroke was used,
 *                  S_FALSE otherwise.
 */

STDMETHODIMP tOleInPlaceFrame::TranslateAccelerator(LPMSG pMSG, WORD wID)
    {
    SCODE       sc;

        sc=S_OK;
    return ResultFromScode(sc);
    }
