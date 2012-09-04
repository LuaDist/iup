//////////////////////////////////////////////////////////////////////
//
// tOleClientSite.cpp: implementation of the tOleClientSite class.
//
//////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <assert.h>

#include "tOleClientSite.h"
#include "tOleHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

tOleClientSite::tOleClientSite(LPUNKNOWN pUnkOuter,
                               tOleHandler *olehandler)
{
  m_cRef=0;
  m_pUnkOuter = pUnkOuter;
  handler = olehandler;
  return;
}

tOleClientSite::~tOleClientSite()
{

}

STDMETHODIMP tOleClientSite::QueryInterface(REFIID riid,
                                            LPVOID *ppv)
{
  return m_pUnkOuter->QueryInterface(riid, ppv);
}


STDMETHODIMP_(ULONG) tOleClientSite::AddRef(void)
{
  ++m_cRef;
  return m_pUnkOuter->AddRef();
}

STDMETHODIMP_(ULONG) tOleClientSite::Release(void)
{
  --m_cRef;
  return m_pUnkOuter->Release();
}

STDMETHODIMP tOleClientSite::SaveObject(void)
{
  // persistencia ainda nao suportada
  return E_FAIL;
}


STDMETHODIMP tOleClientSite::GetMoniker(DWORD dwAssign,
                                        DWORD dwWhich,
                                        LPMONIKER *ppmk)
{
  // Linking nao suportado
  return E_FAIL;
}


STDMETHODIMP tOleClientSite::GetContainer(LPOLECONTAINER * ppContainer)
{

  *ppContainer=NULL;

  return E_NOINTERFACE;
}


/*
 * tOleClientSite::ShowObject
 *
 * Purpose:
 *  Tells the container to bring the object fully into view as much
 *  as possible, that is, scroll the document.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         Standard.
 */

STDMETHODIMP tOleClientSite::ShowObject(void)
{
  return NOERROR;
}


STDMETHODIMP tOleClientSite::OnShowWindow(BOOL fShow)
{
  return NOERROR;
}

/*
 * tOleClientSite::RequestNewObjectLayout
 *
 * Purpose:
 *  Called when the object would like to have its layout
 *  reinitialized.  This is used by OLE Controls.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         Standard.
 */

STDMETHODIMP tOleClientSite::RequestNewObjectLayout(void)
{
  RECT    rc, rcT;
  SIZEL   szl;
  HRESULT hr;

  /*
   * This function is called by a control that is not in-place
   * active or UI active and therefore doesn't have our
   * IOleInPlaceSite interface in which to call OnPosRectChange.
   * Therefore we do pretty much the same thing we do in that
   * function although we ask the control for the size it wants.
   */

  if (!handler->m_pIViewObject2)
    return E_FAIL;

  //Get the size from the control
  hr=handler->m_pIViewObject2->GetExtent(handler->m_fe.dwAspect, -1, NULL, &szl);
  if (FAILED(hr))
    return hr;

  //Add these extents to the existing tenant position.
  SetRect(&rcT, 0, 0, szl.cx*10, -szl.cy*10);
  RectConvertMappings(&rcT, NULL, TRUE);

  rc=handler->m_rcPos;
  rc.right=rc.left+rcT.right;
  rc.bottom=rc.top+rcT.bottom;

  handler->UpdateInPlaceObjectRects(&rc, FALSE);
  return NOERROR;
}

