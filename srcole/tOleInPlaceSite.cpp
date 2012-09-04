// tOleInPlaceSite.cpp: implementation of the tOleInPlaceSite class.
//
//////////////////////////////////////////////////////////////////////

#include "tOleInPlaceSite.h"
#include "tOleHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL        g_fSwitchingActive=FALSE;

/*
 * tOleInPlaceSite::tOleInPlaceSite
 * tOleInPlaceSite::~tOleInPlaceSite
 *
 * Parameters (Constructor):
 *  pTen            PCTenant of the tenant we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

tOleInPlaceSite::tOleInPlaceSite(class tOleHandler *pTen
    , LPUNKNOWN pUnkOuter)
    {
    m_cRef=0;
    m_pTen=pTen;
    m_pUnkOuter=pUnkOuter;
    m_oleinplaceframe = new tOleInPlaceFrame(m_pTen->m_hWnd);
    return;
    }

tOleInPlaceSite::~tOleInPlaceSite(void)
    {
    return;
    }



/*
 * tOleInPlaceSite::QueryInterface
 * tOleInPlaceSite::AddRef
 * tOleInPlaceSite::Release
 *
 * Purpose:
 *  IUnknown members for tOleInPlaceSite object.
 */

STDMETHODIMP tOleInPlaceSite::QueryInterface(REFIID riid
    , LPVOID *ppv)
    {
    return m_pUnkOuter->QueryInterface(riid, ppv);
    }


STDMETHODIMP_(ULONG) tOleInPlaceSite::AddRef(void)
    {
    ++m_cRef;
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) tOleInPlaceSite::Release(void)
    {
    --m_cRef;
    return m_pUnkOuter->Release();
    }




/*
 * tOleInPlaceActiveObject::GetWindow
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

STDMETHODIMP tOleInPlaceSite::GetWindow(HWND *phWnd)
    {
    *phWnd=m_pTen->m_hWnd;
    return NOERROR;
    }




/*
 * tOleInPlaceActiveObject::ContextSensitiveHelp
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

STDMETHODIMP tOleInPlaceSite::ContextSensitiveHelp
    (BOOL fEnterMode)
    {
    return E_NOTIMPL;
    }




/*
 * tOleInPlaceSite::CanInPlaceActivate
 *
 * Purpose:
 *  Answers the server whether or not we can currently in-place
 *  activate its object.  By implementing this interface we say
 *  that we support in-place activation, but through this function
 *  we indicate whether the object can currently be activated
 *  in-place.  Iconic aspects, for example, cannot, meaning we
 *  return S_FALSE.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR if we can in-place activate the object
 *                  in this site, S_FALSE if not.
 */

STDMETHODIMP tOleInPlaceSite::CanInPlaceActivate(void)
    {
    if (DVASPECT_CONTENT!=m_pTen->m_fe.dwAspect)
        return ResultFromScode(S_FALSE);

    return NOERROR;
    }




/*
 * tOleInPlaceSite::OnInPlaceActivate
 *
 * Purpose:
 *  Informs the container that an object is being activated in-place
 *  such that the container can prepare appropriately.  The
 *  container does not, however, make any user interface changes at
 *  this point.  See OnUIActivate.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR or an appropriate error code.
 */

STDMETHODIMP tOleInPlaceSite::OnInPlaceActivate(void)
    {
    //CHAPTER24MOD
    m_pTen->m_fPendingDeactivate=FALSE;
    //End CHAPTER24MOD

    //m_pIOleIPObject is our in-place flag.
    m_pTen->m_pObj->QueryInterface(IID_IOleInPlaceObject
        , (PPVOID)&m_pTen->m_pIOleIPObject);

    return NOERROR;
    }




/*
 * tOleInPlaceSite::OnInPlaceDeactivate
 *
 * Purpose:
 *  Notifies the container that the object has deactivated itself
 *  from an in-place state.  Opposite of OnInPlaceActivate.  The
 *  container does not change any UI at this point.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR or an appropriate error code.
 */

STDMETHODIMP tOleInPlaceSite::OnInPlaceDeactivate(void)
    {
    /*
     * Since we don't have an Undo command, we can tell the object
     * right away to discard its Undo state.
     */
    m_pTen->Activate(OLEIVERB_DISCARDUNDOSTATE, NULL);
    ReleaseInterface(m_pTen->m_pIOleIPObject);
    return NOERROR;
    }




/*
 * tOleInPlaceSite::OnUIActivate
 *
 * Purpose:
 *  Informs the container that the object is going to start munging
 *  around with user interface, like replacing the menu.  The
 *  container should remove any relevant UI in preparation.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR or an appropriate error code.
 */

STDMETHODIMP tOleInPlaceSite::OnUIActivate(void)
    {
    //CHAPTER24MOD
    m_pTen->m_fPendingDeactivate=FALSE;
    //End CHAPTER24MOD

    /*
     * Change the currently selected tenant in the page.  This
     * will UIDeactivate the currently UI Active tenant.
     */
    g_fSwitchingActive=TRUE;
    //m_pTen->m_pPG->m_pPageCur->SwitchActiveTenant(m_pTen);
    g_fSwitchingActive=FALSE;

    return NOERROR;
    }




/*
 * tOleInPlaceSite::OnUIDeactivate
 *
 * Purpose:
 *  Informs the container that the object is deactivating its
 *  in-place user interface at which time the container may
 *  reinstate its own.  Opposite of OnUIActivate.
 *
 * Parameters:
 *  fUndoable       BOOL indicating if the object will actually
 *                  perform an Undo if the container calls
 *                  ReactivateAndUndo.
 *
 * Return Value:
 *  HRESULT         NOERROR or an appropriate error code.
 */

STDMETHODIMP tOleInPlaceSite::OnUIDeactivate(BOOL fUndoable)
    {
    MSG         msg;

    /*
     * Ignore this notification if we're switching between
     * multiple active objects.
     */
    if (g_fSwitchingActive)
        return NOERROR;

    //If in shutdown (NULL storage), don't check messages.
/*    if (NULL==m_pTen->m_pIStorage)
        {
        g_pFR->ReinstateUI();
        return NOERROR;
        }*/

    //If there's a pending double-click, delay showing our UI
/*    if (!PeekMessage(&msg, pDoc->Window(), WM_LBUTTONDBLCLK
        , WM_LBUTTONDBLCLK, PM_NOREMOVE | PM_NOYIELD))
        {
        //Turn everything back on.
        g_pFR->ReinstateUI();
        }
    else*/

    return NOERROR;
    }




/*
 * tOleInPlaceSite::DeactivateAndUndo
 *
 * Purpose:
 *  If immediately after activation the object does an Undo, the
 *  action being undone is the activation itself, and this call
 *  informs the container that this is, in fact, what happened.
 *  The container should call IOleInPlaceObject::UIDeactivate.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR or an appropriate error code.
 */

STDMETHODIMP tOleInPlaceSite::DeactivateAndUndo(void)
    {
    //CHAPTER24MOD
    /*
     * Note that we don't pay attention to the locking
     * from IOleControlSite::LockInPlaceActive since only
     * the object calls this function and should know
     * that it's going to be deactivated.
     */
    //End CHAPTER24MOD

    m_pTen->m_pIOleIPObject->InPlaceDeactivate();
    return NOERROR;
    }




/*
 * tOleInPlaceSite::DiscardUndoState
 *
 * Purpose:
 *  Informs the container that something happened in the object
 *  that means the container should discard any undo information
 *  it currently maintains for the object.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR or an appropriate error code.
 */

STDMETHODIMP tOleInPlaceSite::DiscardUndoState(void)
    {
    return ResultFromScode(E_NOTIMPL);
    }




/*
 * tOleInPlaceSite::GetWindowContext
 *
 * Purpose:
 *  Provides an in-place object with pointers to the frame and
 *  document level in-place interfaces (IOleInPlaceFrame and
 *  IOleInPlaceUIWindow) such that the object can do border
 *  negotiation and so forth.  Also requests the position and
 *  clipping rectangles of the object in the container and a
 *  pointer to an OLEINPLACEFRAME info structure which contains
 *  accelerator information.
 *
 *  Note that the two interfaces this call returns are not
 *  available through QueryInterface on IOleInPlaceSite since they
 *  live with the frame and document, but not the site.
 *
 * Parameters:
 *  ppIIPFrame      LPOLEINPLACEFRAME * in which to return the
 *                  AddRef'd pointer to the container's
 *                  IOleInPlaceFrame.
 *  ppIIPUIWindow   LPOLEINPLACEUIWINDOW * in which to return
 *                  the AddRef'd pointer to the container document's
 *                  IOleInPlaceUIWindow.
 *  prcPos          LPRECT in which to store the object's position.
 *  prcClip         LPRECT in which to store the object's visible
 *                  region.
 *  pFI             LPOLEINPLACEFRAMEINFO to fill with accelerator
 *                  stuff.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceSite::GetWindowContext
    (LPOLEINPLACEFRAME *ppIIPFrame, LPOLEINPLACEUIWINDOW
    *ppIIPUIWindow, LPRECT prcPos, LPRECT prcClip
    , LPOLEINPLACEFRAMEINFO pFI)
    {
    RECTL           rcl;

    *ppIIPUIWindow=NULL;
    *ppIIPFrame=m_oleinplaceframe;
    m_oleinplaceframe->AddRef();

    GetClientRect(m_pTen->m_hWnd, prcPos);
    GetClientRect(m_pTen->m_hWnd, prcClip);

/*    *ppIIPFrame=(LPOLEINPLACEFRAME)g_pFR;
    g_pFR->AddRef();*/

/*    if (NULL!=pDoc)
        {
        pDoc->QueryInterface(IID_IOleInPlaceUIWindow
            , (PPVOID)ppIIPUIWindow);
        }*/

    //Now get the rectangles and frame information.
    /*m_pTen->RectGet(&rcl, TRUE);
    RECTFROMRECTL(*prcPos, rcl);
                                  0
    //Include scroll position here.
    OffsetRect(prcPos, -(int)m_pTen->m_pPG->m_xPos
        , -(int)m_pTen->m_pPG->m_yPos);

    SetRect(prcClip, 0, 0, 32767, 32767);
*/
    pFI->cb=sizeof(OLEINPLACEFRAMEINFO);
    pFI->fMDIApp=FALSE;

    pFI->hwndFrame=m_pTen->m_hWnd;

    pFI->haccel=NULL;
    pFI->cAccelEntries=0;

    return NOERROR;
    }




/*
 * tOleInPlaceSite::Scroll
 *
 * Purpose:
 *  Asks the container to scroll the document, and thus the object,
 *  by the given amounts in the sz parameter.
 *
 * Parameters:
 *  sz              SIZE containing signed horizontal and vertical
 *                  extents by which the container should scroll.
 *                  These are in device units.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceSite::Scroll(SIZE sz)
    {
    /*int         x, y;

    x=m_pTen->m_pPG->m_xPos+sz.cx;
    y=m_pTen->m_pPG->m_yPos+sz.cy;

    SendScrollPosition(m_pTen->m_hWnd, WM_HSCROLL, x);
    SendScrollPosition(m_pTen->m_hWnd, WM_VSCROLL, y);*/
    return NOERROR;
    }




/*
 * tOleInPlaceSite::OnPosRectChange
 *
 * Purpose:
 *  Informs the container that the in-place object was resized.
 *  The container must call IOleInPlaceObject::SetObjectRects.
 *  This does not change the site's rectangle in any case.
 *
 * Parameters:
 *  prcPos          LPCRECT containing the new size of the object.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP tOleInPlaceSite::OnPosRectChange(LPCRECT prcPos)
    {
    if (NULL!=prcPos)
        {
        // This change was necessary because some controls were not working, and being positioned in a wrong place.
        // Contribution by Kommit
        LPRECT pPos = (LPRECT)prcPos; // convert the const pointer to non-const pointer to modify it's member values.
        pPos->right -= pPos->left;
        pPos->bottom -= pPos->top;
        pPos->left = 0;
        pPos->top = 0;

        m_pTen->m_rcPos=*prcPos;
        }

    m_pTen->UpdateInPlaceObjectRects(prcPos, FALSE);
    return NOERROR;
    }

