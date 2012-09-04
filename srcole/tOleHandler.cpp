// tOleHandler.cpp: implementation of the tOleHandler class.
//
//////////////////////////////////////////////////////////////////////

#include <ole2.h>
#include <assert.h>

#ifndef _OLE2_H_
#define _OLE2_H_
#endif

#include "tOleHandler.h"
#include "tDispatch.h"

/*
 * tOleHandler::tOleHandler
 * tOleHandler::~tOleHandler
 *
 */

tOleHandler::tOleHandler()
{
  natural_width  = 0;
  natural_height = 0;

  m_hWnd=NULL;

  m_fInitialized=0;
  m_cOpens=0;

  m_pObj=NULL;
  m_clsID=CLSID_NULL;
  m_fSetExtent=FALSE;

  m_cRef=0;
  m_pIOleObject=NULL;
  m_pIViewObject2=NULL;
  m_grfMisc=0;

  m_rcl.right = m_rcl.left = 0;
  m_rcl.top = m_rcl.bottom = 0;

  m_pImpIOleClientSite=NULL;

  m_fRepaintEnabled=TRUE;


  m_pImpIOleIPSite=NULL;
  m_pIOleIPObject=NULL;
  m_rcPos.left=-1;
  m_fInRectSet=FALSE;

  //CHAPTER24MOD
  m_pImpIOleControlSite=NULL;
  m_pImpIDispatch=NULL;

  m_pIOleControl=NULL;

  m_fHaveControlInfo=FALSE;
  m_cLockInPlace=0;
  m_fPendingDeactivate=FALSE;
  //End CHAPTER24MOD

  return;
}


tOleHandler::~tOleHandler(void)
    {

    //Object pointers cleaned up in Close.

    //CHAPTER24MOD

    DeleteInterfaceImp(m_pImpIOleControlSite);
    DeleteInterfaceImp(m_pImpIDispatch);

    //End CHAPTER24MOD

    DeleteInterfaceImp(m_pImpIOleIPSite);
    DeleteInterfaceImp(m_pImpIOleClientSite);
    return;
    }




/*
 * tOleHandler::QueryInterface
 * tOleHandler::AddRef
 * tOleHandler::Release
 *
 * Purpose:
 *  IUnknown members for tOleHandler object.
 */

STDMETHODIMP tOleHandler::QueryInterface(REFIID riid, LPVOID *ppv)
    {
    *ppv=NULL;

    if (IID_IUnknown==riid)
        *ppv=this;

    if (IID_IOleClientSite==riid)
        *ppv=m_pImpIOleClientSite;

    if (IID_IOleWindow==riid || IID_IOleInPlaceSite==riid)
        *ppv=m_pImpIOleIPSite;

    //CHAPTER24MOD
    if (IID_IOleControlSite==riid)
        *ppv=m_pImpIOleControlSite;

    //Queries for IDispatch return the ambient properties interface
    if (IID_IDispatch==riid)
        *ppv=m_pImpIDispatch;
    //End CHAPTER24MOD

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) tOleHandler::AddRef(void)
    {
    return ++m_cRef;
    }

STDMETHODIMP_(ULONG) tOleHandler::Release(void)
    {
    if (0!=--m_cRef)
        return m_cRef;

    delete this;
    return 0;
    }






/*
 * tOleHandler::Create
 *
 * Purpose:
 *  Creates a new tenant of the given CLSID, which can be either a
 *  static bitmap or metafile or any compound document object.
 *
 * Parameters:
 *  tType           TENANTTYPE to create, either a static metafile,
 *                  bitmap, or some kind of compound document object
 *                  This determines which OleCreate* call we use.
 *  pvType          LPVOID providing the relevant pointer from which
 *                  to create the tenant, depending on iType.
 *  pFE             LPFORMATETC specifying the type of renderings
 *                  to use.
 *  pptl            PPOINTL in which we store offset coordinates.
 *  pszl            LPSIZEL where this object should store its
 *                  lometric extents.
 *  pIStorage       LPSTORAGE of the page we live in.  We have to
 *                  create another storage in this for the tenant.
 *  ppo             PPATRONOBJECT containing placement data.
 *  dwData          DWORD with extra data, sensitive to iType.
 *
 * Return Value:
 *  UINT            A CREATE_* value depending on what we
 *                  actually do.
 */

UINT tOleHandler::Create(LPVOID pvType)
    {
    HRESULT             hr;
    LPUNKNOWN           pObj;
    UINT                uRet=CREATE_GRAPHICONLY;
    DWORD       dwMode=STGM_READWRITE
                    | STGM_SHARE_EXCLUSIVE
                    | STGM_DELETEONRELEASE;


    IPersistStorage *persist_storage = NULL;

    StgCreateDocfile(NULL, dwMode, 0, &m_pIStorage);

    if(m_pIStorage == NULL)
      return CREATE_FAILED;

    if (NULL==pvType)
        return CREATE_FAILED;

    hr=ResultFromScode(E_FAIL);

    Open(NULL);

    //CHAPTER24MOD
    /*
     * The OLE Control specifications mention that a
     * a control might implement IPersistStream[Init]
     * instead of IPersistStorage.  In that case you
     * cannot use OleCreate on a control but must rather
     * use CoCreateInstance since OleCreate assumes
     * that IPersistStorage is available.  With a control,
     * you would have to create the object first, then
     * check if OLEMISC_SETCLIENTSITEFIRST is set, then
     * send it your IOleClientSite first.  Then you check
     * for IPersistStorage and failing that, try
     * IPersistStream[Init].
     *
     * For simplicity we'll assume storage-based
     * controls in this sample.
     */
    //End CHAPTER24MOD

    hr = CoCreateInstance(*((LPCLSID)pvType), NULL,
      CLSCTX_ALL, IID_IUnknown, (LPVOID *)&pObj);

    if(FAILED(hr))
      return CREATE_FAILED;

    if(pObj->QueryInterface(IID_IPersistStorage, (void **) &persist_storage) != S_OK)
      return CREATE_FAILED;

    //We need an IOleObject most of the time, so get one here.
    m_pIOleObject=NULL;
    hr = pObj->QueryInterface(IID_IOleObject, (LPVOID*)&m_pIOleObject);

    if(FAILED(hr))
      return CREATE_FAILED;

    // seta o client site
    m_pIOleObject->SetClientSite(m_pImpIOleClientSite);

    // inicializa o objeto
    hr = persist_storage->InitNew(m_pIStorage);

    if(FAILED(hr))
      return CREATE_FAILED;


    //We don't get the size if PatronObject data was seen already.
    if (!ObjectInitialize(pObj))
        {
        return CREATE_FAILED;
        }

    SIZEL   szl;

    hr=ResultFromScode(E_FAIL);

    CalcNaturalSize();

    //CHAPTER24MOD
    //Make sure this happens
    /*if ((OLEMISC_ACTIVATEWHENVISIBLE & m_grfMisc))
        Activate(OLEIVERB_INPLACEACTIVATE, NULL);*/
    //End CHAPTER24MOD

    return uRet;
    }







/*
 * tOleHandler::ObjectInitialize
 * (Protected)
 *
 * Purpose:
 *  Performs operations necessary after creating an object or
 *  reloading one from storage.
 *
 * Parameters:
 *  pObj            LPUNKNOWN of the object in this tenant.
 *  pFE             LPFORMATETC describing the graphic here.
 *  dwData          DWORD extra data.  If pFE->dwAspect==
 *                  DVASPECT_ICON then this is the iconic metafile.
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise.
 */

BOOL tOleHandler::ObjectInitialize(LPUNKNOWN pObj)
    {
    HRESULT         hr;
    FORMATETC fe;

    SETDefFormatEtc(fe, 0, TYMED_NULL);
    LPFORMATETC pFE = &fe;


    if (NULL==pObj || NULL==pFE)
        return FALSE;

    m_pObj=pObj;
    m_fe=*pFE;
    m_fe.ptd=NULL;
    m_dwState=TENANTSTATE_DEFAULT;

    m_pIViewObject2=NULL;
    hr=pObj->QueryInterface(IID_IViewObject2
        , (LPVOID*)&m_pIViewObject2);

    if (FAILED(hr))
        return FALSE;

    /*
     * Get the MiscStatus bits and check for OLEMISC_ONLYICONIC.
     * If set, force dwAspect in m_fe to DVASPECT_ICON so we
     * remember to draw it properly and do extents right.
     */
    m_pIOleObject->GetMiscStatus(m_fe.dwAspect, &m_grfMisc);


    //CHAPTER24MOD
    //Run the object if it says to do so
    if (OLEMISC_ALWAYSRUN & m_grfMisc)
        OleRun(pObj);
    //End CHAPTER24MOD



    //CHAPTER24MOD
    //Go try initializing control-related things.
    ControlInitialize();
    //End CHAPTER24MOD

    return TRUE;
    }




/*
 * tOleHandler::Open
 *
 * Purpose:
 *  Retrieves the IStorage associated with this tenant.  The
 *  IStorage is owned by the tenant and thus the tenant always
 *  holds a reference count.
 *
 *  If the storage is already open for this tenant, then this
 *  function will AddRef it; therefore the caller must always
 *  match an Open with a Close.
 *
 * Parameters:
 *  pIStorage       LPSTORAGE above this tenant (which has its
 *                  own storage).
 *
 * Return Value:
 *  BOOL            TRUE if opening succeeds, FALSE otherwise.
 */

BOOL tOleHandler::Open(LPSTORAGE pIStorage)
    {
    HRESULT     hr=NOERROR;
    DWORD       dwMode=STGM_TRANSACTED | STGM_READWRITE
                    | STGM_SHARE_EXCLUSIVE;


    //Create these if we don't have them already.
    if (NULL==m_pImpIOleClientSite)
        {
        m_pImpIOleClientSite=new tOleClientSite(this, this);
        m_pImpIOleIPSite=new tOleInPlaceSite(this, this);

        //CHAPTER24MOD
        m_pImpIOleControlSite=new tOleControlSite(this, this);
        m_pImpIDispatch=new tDispatch(this, this);

        if (NULL==m_pImpIOleClientSite 
            || NULL==m_pImpIOleIPSite || NULL==m_pImpIOleControlSite
            || NULL==m_pImpIDispatch)
            return FALSE;
        //End CHAPTER24MOD
        }

    return TRUE;
    }




/*
 * tOleHandler::Close
 *
 * Purpose:
 *  Possibly commits the storage, then releases it reversing the
 *  reference count from Open.  If the reference on the storage
 *  goes to zero, the storage is forgotten.  However, the object we
 *  contain is still held and as long as it's active the storage
 *  remains alive.
 *
 * Parameters:
 *  fCommit         BOOL indicating if we're to commit.
 *
 * Return Value:
 *  None
 */

void tOleHandler::Close(BOOL fCommit)
    {
        /*
         * We can't use a zero reference count to know when to NULL
         * this since other things might have AddRef'd the storage.
         */
            //OnInPlaceDeactivate releases this pointer.
            if (NULL!=m_pIOleIPObject)
                m_pIOleIPObject->InPlaceDeactivate();

            //Close the object saving if necessary
            if (NULL!=m_pIOleObject)
                {
                m_pIOleObject->Close(OLECLOSE_SAVEIFDIRTY);
                ReleaseInterface(m_pIOleObject);
                }

            //Release all other held pointers
            //CHAPTER24MOD
            ReleaseInterface(m_pIOleControl);

            //End CHAPTER24MOD

            //Release all other held pointers
            if (NULL!=m_pIViewObject2)
                {
                m_pIViewObject2->SetAdvise(m_fe.dwAspect, 0, NULL);
                ReleaseInterface(m_pIViewObject2);
                }

            //We know we only hold one ref from Create or Load
            ReleaseInterface(m_pObj);

    return;
    }


/*
 * tOleHandler::Activate
 *
 * Purpose:
 *  Activates a verb on the object living in the tenant.  Does
 *  nothing for static objects.
 *
 * Parameters:
 *  iVerb           LONG of the verb to execute.
 *  pMSG            LPMSG to the message causing the invocation.
 *
 * Return Value:
 *  BOOL            TRUE if the object changed due to this verb
 *                  execution.
 */

BOOL tOleHandler::Activate(LONG iVerb, LPMSG pMSG)
    {
    RECT        rc, rcH;
    SIZEL       szl;

    //Can't activate statics.
/*    if (TENANTTYPE_STATIC==m_tType || NULL==m_pIOleObject)
        {
        MessageBeep(0);
        return FALSE;
        }*/

    RECTFROMRECTL(rc, m_rcl);
    RectConvertMappings(&rc, NULL, TRUE);
    XformRectInPixelsToHimetric(NULL, &rc, &rcH);

    //Get the server running first, then do a SetExtent, then show it
    OleRun(m_pIOleObject);

    if (m_fSetExtent)
        {
        SETSIZEL(szl, rcH.right-rcH.left, rcH.top-rcH.bottom);
        m_pIOleObject->SetExtent(m_fe.dwAspect, &szl);
        m_fSetExtent=FALSE;
        }

    //CHAPTER24MOD
    /*
     * If we have a pending deactivation, but we're activating
     * again, clear the pending flag.
     */
    if (OLEIVERB_UIACTIVATE==iVerb
        || OLEIVERB_INPLACEACTIVATE==iVerb)
        m_fPendingDeactivate=FALSE;
    //End CHAPTER24MOD

    m_pIOleObject->DoVerb(iVerb, pMSG, m_pImpIOleClientSite, 0
        , m_hWnd, &rcH);

    //If object changes, IAdviseSink::OnViewChange will see it.
    return FALSE;
    }






/*
 * tOleHandler::ObjectGet
 *
 * Purpose:
 *  Retrieves the LPUNKNOWN of the object in use by this tenant
 *
 * Parameters:
 *  ppUnk           LPUNKNOWN * in which to return the object
 *                  pointer.
 *
 * Return Value:
 *  None
 */

void tOleHandler::ObjectGet(LPUNKNOWN *ppUnk)
    {
    if (NULL!=ppUnk)
        {
        *ppUnk=m_pObj;
        m_pObj->AddRef();
        }

    return;
    }





/*
 * tOleHandler::SizeGet
 * tOleHandler::SizeSet
 * tOleHandler::RectGet
 * tOleHandler::RectSet
 *
 * Purpose:
 *  Returns or sets the size/position of the object contained here.
 *
 * Parameters:
 *  pszl/prcl       LPSIZEL (Size) or LPRECTL (Rect) with the
 *                  extents of interest.  In Get situations,
 *                  this will receive the extents; in Set it
 *                  contains the extents.
 *  fDevice         BOOL indicating that pszl/prcl is expressed
 *                  in device units.  Otherwise it's LOMETRIC.
 *  fInformObj      (Set Only) BOOL indicating if we need to inform
 *                  the object all.
 *
 * Return Value:
 *  None
 */

void tOleHandler::SizeGet(LPSIZEL pszl, BOOL fDevice)
    {
    if (!fDevice)
        {
        pszl->cx=m_rcl.right-m_rcl.left;
        pszl->cy=m_rcl.bottom-m_rcl.top;
        }
    else
        {
        RECT        rc;

        SetRect(&rc, (int)(m_rcl.right-m_rcl.left)
            , (int)(m_rcl.bottom-m_rcl.top), 0, 0);

        RectConvertMappings(&rc, NULL, TRUE);

        pszl->cx=(long)rc.left;
        pszl->cy=(long)rc.top;
        }

    return;
    }


void tOleHandler::SizeSet(LPSIZEL pszl, BOOL fDevice, BOOL fInformObj)
    {
    SIZEL           szl;

    if (!fDevice)
        {
        szl=*pszl;
        m_rcl.right =pszl->cx+m_rcl.left;
        m_rcl.bottom=pszl->cy+m_rcl.top;
        }
    else
        {
        RECT        rc;

        SetRect(&rc, (int)pszl->cx, (int)pszl->cy, 0, 0);
        RectConvertMappings(&rc, NULL, FALSE);

        m_rcl.right =(long)rc.left+m_rcl.left;
        m_rcl.bottom=(long)rc.top+m_rcl.top;

        SETSIZEL(szl, (long)rc.left, (long)rc.top);
        }

    //Tell OLE that this object was resized.
    if (NULL!=m_pIOleObject && fInformObj)
        {
        HRESULT     hr;
        BOOL        fRun=FALSE;

        //Convert our LOMETRIC into HIMETRIC by *=10
        szl.cx*=10;
        szl.cy*=-10;    //Our size is stored negative.

        /*
         * If the MiscStatus bit of OLEMISC_RECOMPOSEONRESIZE
         * is set, then we need to run the object before calling
         * SetExtent to make sure it has a real chance to
         * re-render the object.  We have to update and close
         * the object as well after this happens.
         */

        if (OLEMISC_RECOMPOSEONRESIZE & m_grfMisc)
            {
            if (!OleIsRunning(m_pIOleObject))
                {
                OleRun(m_pIOleObject);
                fRun=TRUE;
                }
            }

        hr=m_pIOleObject->SetExtent(m_fe.dwAspect, &szl);

        /*
         * If the object is not running and it does not have
         * RECOMPOSEONRESIZE, then SetExtent fails.  Make
         * sure that we call SetExtent again (by just calling
         * SizeSet here again) when we next run the object.
         */
        if (SUCCEEDED(hr))
            {
            m_fSetExtent=FALSE;

            if (fRun)
                {
                m_pIOleObject->Update();
                m_pIOleObject->Close(OLECLOSE_SAVEIFDIRTY);
                }
            }
        else
            {
            if (OLE_E_NOTRUNNING==GetScode(hr))
                m_fSetExtent=TRUE;
            }
        }

    return;
    }


void tOleHandler::RectGet(LPRECTL prcl, BOOL fDevice)
    {
    if (!fDevice)
        *prcl=m_rcl;
    else
        {
        RECT        rc;

        RECTFROMRECTL(rc, m_rcl);
        RectConvertMappings(&rc, NULL, TRUE);
        RECTLFROMRECT(*prcl, rc);
        }

    return;
    }


void tOleHandler::RectSet(LPRECTL prcl, BOOL fDevice, BOOL fInformObj)
    {
    SIZEL   szl;
    LONG    cx, cy;

    /*
     * Prevent reentrant calls that may come from calling
     * UpdateInPlaceObjectRects here and elsewhere.
     */
    if (m_fInRectSet)
        return;

    m_fInRectSet=TRUE;

    cx=m_rcl.right-m_rcl.left;
    cy=m_rcl.bottom-m_rcl.top;

    if (!fDevice)
        m_rcl=*prcl;
    else
        {
        RECT        rc;

        RECTFROMRECTL(rc, *prcl);
        RectConvertMappings(&rc, NULL, FALSE);
        RECTLFROMRECT(m_rcl, rc);
        }

    /*
     * Tell ourselves that the size changed, if it did.  SizeSet
     * will call IOleObject::SetExtent for us.
     */
    if ((m_rcl.right-m_rcl.left)!=cx || (m_rcl.bottom-m_rcl.top)!=cy)
        {
        SETSIZEL(szl, m_rcl.right-m_rcl.left, m_rcl.bottom-m_rcl.top);
        SizeSet(&szl, FALSE, fInformObj);
        }

    //Tell an in-place active object it moved too
    UpdateInPlaceObjectRects(NULL, TRUE);
    m_fInRectSet=FALSE;
    return;
    }








/*
 * tOleHandler::DeactivateInPlaceObject
 *
 * Purpose:
 *  Deactivates an in-place object if there is one in this tenant.
 *
 * Parameters:
 *  fFull           BOOL indicating full deactivation of UI
 *                  deactivate only.
 *
 * Return Value:
 *  None
 */

void tOleHandler::DeactivateInPlaceObject(BOOL fFull)
    {
    if (NULL!=m_pIOleIPObject)
        {
        /*
         * Activate-when-visible objects only UI deactivate
         * unless we're fully deactivating on purpose.
         */
        if ((OLEMISC_ACTIVATEWHENVISIBLE & m_grfMisc) && !fFull)
            m_pIOleIPObject->UIDeactivate();
        else
            {
            //CHAPTER24MOD
            /*
             * Only deactivate when there's no locks.  If there
             * is a lock, then remember that we need to deactivate
             * when all the locks go away.
             */
            if (0==m_cLockInPlace)
                m_pIOleIPObject->InPlaceDeactivate();
            else
                m_fPendingDeactivate=TRUE;
            //End CHAPTER24MOD
            }
        }

    return;
    }



/*
 * tOleHandler::UpdateInPlaceObjectRects
 *
 * Purpose:
 *  Generates a call to IOleInPlaceObject::SetObjectRects to allow
 *  it to show it's shading and its object adornments properly.
 *  This function deals in HIMETRIC units.
 *
 * Parameters:
 *  prcPos          LPCRECT to the size the object wants.  Ignored
 *                  if NULL in which case we use the size of the
 *                  tenant.  This rect is in client coordinates of
 *                  the pages window.
 *  fUseTenantRect  BOOL indicating if we need to use the tenant
 *                  rectangle offset for scrolling regardless.
 *
 * Return Value:
 *  None
 */

void tOleHandler::UpdateInPlaceObjectRects(LPCRECT prcPos
    , BOOL fUseTenantRect)
    {
    RECTL       rcl;
    RECT        rc;
    RECT        rcClip;
    BOOL        fResizeTenant=TRUE;

    //We don't clip special anywhere in our window.
    SetRect(&rcClip, 0, 0, 32767, 32767);

    /*
     * Note that if the object here is activate-when-visible
     * we'll always have this pointer.
     */
    if (NULL!=m_pIOleIPObject)
        {
        /*
         * This uses the last position rectangle from
         * IOleInPlaceSite::OnPosRectChange if it's been
         * initialized
         */
        if (NULL==prcPos && -1!=m_rcPos.left && !fUseTenantRect)
            prcPos=&m_rcPos;

        //This code is normally called from OnPosRectChange direct.
        if (NULL!=prcPos && !fUseTenantRect)
            {
            rc=*prcPos;

            //Calculate the boundaries of the full page
            //m_pPG->CalcBoundingRect(&rcClip, FALSE);

            //Make sure we limit object to page boundaries.
            IntersectRect(&rc, &rc, &rcClip);
            }
        else
            {
            /*
             * We have no rectangle of the object on which to
             * base the position, so just use the tenant rectangle.
             * This code is also used when scrolling objects.
             */
            RectGet(&rcl, TRUE);
            RECTFROMRECTL(rc, rcl);

            //Account for scrolling
//            OffsetRect(&rc, -(int)m_pPG->m_xPos, -(int)m_pPG->m_yPos);
            fResizeTenant=FALSE;
            }


        /*
         * NOTE:  The rectangles to SetObjectRects is in client
         * coordinates of the pages window.
         */
        if (NULL!=m_pIOleIPObject)
            m_pIOleIPObject->SetObjectRects(&rc, &rcClip);

        if (fResizeTenant)
            {
            //Need to tell the tenant to change position too
            RECTLFROMRECT(rcl, rc);
            RectSet(&rcl, TRUE, FALSE);
            }
        }

    return;
    }


/*
 * tOleHandler::ObjectWindow
 *
 * Purpose:
 *  Returns the window handle of the in-place object.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HWND            Handle of the object window.
 */

HWND tOleHandler::ObjectWindow(void)
    {
    HWND    hWnd=NULL;

    if (NULL!=m_pIOleIPObject)
        m_pIOleIPObject->GetWindow(&hWnd);

    return hWnd;
    }




/*
 * tOleHandler::ControlInitialize
 *
 * Purpose:
 *  Initializes the control if that's the type of object we have
 *  in the site.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if initialization worked, FALSE otherwise.
 */

BOOL tOleHandler::ControlInitialize(void)
    {
    HRESULT         hr;
    BOOL            fEvents;

    if (NULL==m_pObj)
        return FALSE;

    hr=m_pObj->QueryInterface(IID_IOleControl
        , (PPVOID)&m_pIOleControl);

    //Failure means not a control.
    if (FAILED(hr))
        return FALSE;

    m_ambientProp.setControl(m_pIOleControl);

    m_dwState |= TENANTSTATE_CONTROL;

    if (OLEMISC_ACTSLIKEBUTTON & m_grfMisc)
        m_dwState |= TENANTSTATE_BUTTON;

    //We don't use this, but might as well store it.
    if (OLEMISC_ACTSLIKELABEL & m_grfMisc)
        m_dwState |= TENANTSTATE_LABEL;

    /*
     * Call IOleControl::GetControlInfo to retrieve the keyboard
     * information for this control.  We have to reload this
     * information in IOleControlSite::OnControlInfoChanged.
     */
    /*m_fHaveControlInfo=SUCCEEDED(m_pIOleControl
        ->GetControlInfo(&m_ctrlInfo));*/ //!!!


    /*
     * If you wanted to receive IPropertyNotifySink notifications
     * for a control, establish that advisory connection here
     * through the object's IConnectionPointContainer and
     * IConnectionPoint.
     */

    return TRUE;
    }



/*
 * tOleHandler::GetControlFlags
 *
 * Purpose:
 *  Requests flags describing the control inside this tenant
 *  if there is, in fact a control.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  DWORD           Flags describing the control from the
 *                  TENANT
 */

DWORD tOleHandler::GetControlFlags(void)
    {
    return m_dwState & STATEMASK_CONTROLS;
    }





/*
 * tOleHandler::TryMnemonic
 *
 * Purpose:
 *  Asks the tenant to check the given keyboard message against
 *  one that its control might want, passing it to
 *  IOleControl::OnMnemonic if there is a match.
 *
 * Parameters:
 *  pMsg            LPMSG containing the message to check.
 *
 * Return Value:
 *  BOOL            TRUE if the mnemonic was a match,
 *                  FALSE otherwise.
 */

BOOL tOleHandler::TryMnemonic(LPMSG pMsg)
    {
    UINT        i;
    BOOL        fRet=FALSE;
    LPACCEL     pACC;
    BYTE        fVirt=FVIRTKEY;

    if (!m_fHaveControlInfo)    //False for non-controls
        return FALSE;

    if (0==m_ctrlInfo.cAccel)
        return FALSE;

    pACC=(LPACCEL)GlobalLock(m_ctrlInfo.hAccel);

    if (NULL==pACC)
        return FALSE;

    /*
     * We'll come here on WM_KEYDOWN messages and WM_SYSKEYDOWN
     * messages.  The control's accelerator table will contain
     * entries that each have the desired key and the various
     * modifier flags.  We the create the current modifier flags
     * then look for entries that match those flags and the key
     * that is in the message itself.
     */

    fVirt |= (WM_SYSKEYDOWN==pMsg->message) ? FALT : 0;

    //GetKeyState works on the last message
    fVirt |= (0x8000 & GetKeyState(VK_CONTROL)) ? FCONTROL : 0;
    fVirt |= (0x8000 & GetKeyState(VK_SHIFT)) ? FSHIFT : 0;

    for (i=0; i < m_ctrlInfo.cAccel; i++)
        {
        if (pACC[i].key==pMsg->wParam && pACC[i].fVirt==fVirt)
            {
            m_pIOleControl->OnMnemonic(pMsg);
            fRet=TRUE;
            break;
            }
        }

    GlobalUnlock(m_ctrlInfo.hAccel);
    return fRet;
    }





/*
 * tOleHandler::AmbientChange
 *
 * Purpose:
 *  Notifes a control that an ambient property has changed in
 *  the control site.
 *
 * Parameters:
 *  dispID          DISPID of the property that changed.
 *
 * Return Value:
 *  None
 */

void tOleHandler::AmbientChange(DISPID dispID)
    {
    if (NULL!=m_pIOleControl)
        m_pIOleControl->OnAmbientPropertyChange(dispID);

    return;
    }

//End CHAPTER24MOD

void tOleHandler::OnShow()
{
  // Se objeto ja' tive sido ativado, ignora chamada
  // Alguns controles reagem mal quando ativados
  // mais de uma vez
  if(m_pIOleIPObject != NULL)
    return;

  if ((OLEMISC_ACTIVATEWHENVISIBLE & m_grfMisc))
    Activate(OLEIVERB_INPLACEACTIVATE, NULL);
}


void tOleHandler::GetNaturalSize(long *pWidth, long *pHeight)
{
  assert(pWidth && pHeight);
  if(!pWidth || !pHeight)
    return;

  *pWidth = natural_width;
  *pHeight = natural_height;
}

/*
 * CalcNaturalSize
 * Obtem o tamanho desejado pelo objeto
 */

void tOleHandler::CalcNaturalSize()
{
  HRESULT hr;
  SIZEL sizel;
  RECTL rcl;

  assert(m_pIViewObject2 != NULL);

  if(!m_pIViewObject2)
    return;

  hr = m_pIViewObject2->GetExtent(m_fe.dwAspect, -1, NULL, &sizel);

  if(FAILED(hr))
    return;

  SETRECTL(rcl, 0, 0, sizel.cx/10, -sizel.cy/10);
  RectSet(&rcl, FALSE, TRUE);

  // Obtem medidas ja' convertidas para pixels
  SizeGet(&sizel, TRUE);

  natural_width = sizel.cx;
  natural_height = sizel.cy;
}

