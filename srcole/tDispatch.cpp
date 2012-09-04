// tDispatch.cpp: implementation of the tDispatch class.
//
//////////////////////////////////////////////////////////////////////

#include "tDispatch.h"
#include "tOleHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/*
 * tDispatch::tDispatch
 * tDispatch::~tDispatch
 *
 * Parameters (Constructor):
 *  pTen            PCTenant of the tenant we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

tDispatch::tDispatch(tOleHandler *pTen, LPUNKNOWN pUnkOuter)
    {
    m_cRef=0;
    m_pTen=pTen;
    m_pUnkOuter=pUnkOuter;
    return;
    }

tDispatch::~tDispatch(void)
    {
    return;
    }




/*
 * tDispatch::QueryInterface
 * tDispatch::AddRef
 * tDispatch::Release
 */

STDMETHODIMP tDispatch::QueryInterface(REFIID riid, PPVOID ppv)
    {
    return m_pUnkOuter->QueryInterface(riid, ppv);
    }

STDMETHODIMP_(ULONG) tDispatch::AddRef(void)
    {
    ++m_cRef;
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) tDispatch::Release(void)
    {
    m_cRef--;
    return m_pUnkOuter->Release();
    }




/*
 * tDispatch::GetTypeInfoCount
 * tDispatch::GetTypeInfo
 * tDispatch::GetIDsOfNames
 *
 * Unimplemented members, not needed for ambient properties.
 */

STDMETHODIMP tDispatch::GetTypeInfoCount(UINT *pctInfo)
    {
    *pctInfo=0;
    return NOERROR;
    }

STDMETHODIMP tDispatch::GetTypeInfo(UINT itinfo
    , LCID lcid, ITypeInfo **pptInfo)
    {
    *pptInfo=NULL;
    return ResultFromScode(E_NOTIMPL);
    }

STDMETHODIMP tDispatch::GetIDsOfNames(REFIID riid
    , OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispID)
    {
    *rgszNames=NULL;
    *rgDispID=NULL;
    return ResultFromScode(E_NOTIMPL);
    }




/*
 * tDispatch::Invoke
 *
 * Purpose:
 *  Calls a method in the dispatch interface or manipulates a
 *  property.
 *
 * Parameters:
 *  dispIDMember    DISPID of the method or property of interest.
 *  riid            REFIID reserved, must be NULL.
 *  lcid            LCID of the locale.
 *  wFlags          USHORT describing the context of the invocation.
 *  pDispParams     DISPPARAMS * to the array of arguments.
 *  pVarResult      VARIANT * in which to store the result.  Is
 *                  NULL if the caller is not interested.
 *  pExcepInfo      EXCEPINFO * to exception information.
 *  puArgErr        UINT * in which to store the index of an
 *                  invalid parameter if DISP_E_TYPEMISMATCH
 *                  is returned.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error code.
 */


STDMETHODIMP tDispatch::Invoke(DISPID dispIDMember, REFIID riid
    , LCID lcid, unsigned short wFlags, DISPPARAMS *pDispParams
    , VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
    {
    HRESULT     hr;
    VARIANT     varResult;

    if (IID_NULL!=riid)
        return ResultFromScode(E_INVALIDARG);

    /*
     * We ignore lcid in this function.  A multilingual application
     * might use it to determine the meaning of certain parameters
     * or perhaps as an indication of how to format data like
     * time, date, and currency or any other language or locale-
     * sensitive data.
     */

    /*
     * Variable handling:  since ambient properties is just a
     * collection of singular read-only values, we don't have to
     * worry about input parameters.
     */

    /*
     * If the caller is not interested in the return value, then
     * pVarResult is NULL.  But since we're dealing with ambient
     * properties, there should always be an interest.  In any case,
     * if we're given a NULL, we'll point it to a dummy structure so
     * the rest of the code can assume that pVarResult is non-NULL.
     */
    if(NULL==pVarResult)
      pVarResult=&varResult;

    VariantInit(pVarResult);

    //The most common case is boolean, use as an initial type
    V_VT(pVarResult)=VT_BOOL;

    /*
     * Process the requested ambient property.  Anything but a
     * request for a property is invalid, so we can check that
     * before looking at the specific ID.  We can only get away
     * with this because all properties are read-only.
     */

    if (!(DISPATCH_PROPERTYGET & wFlags))
        return ResultFromScode(DISP_E_MEMBERNOTFOUND);

    hr=NOERROR;

    switch (dispIDMember)
        {
        case DISPID_AMBIENT_BACKCOLOR:
            V_I4(pVarResult)=m_pTen->m_ambientProp.getBackColor();
            V_VT(pVarResult)=VT_I4;
            break;

        case DISPID_AMBIENT_FORECOLOR:
            V_I4(pVarResult)=m_pTen->m_ambientProp.getForeColor();
            V_VT(pVarResult)=VT_I4;
            break;

        case DISPID_AMBIENT_FONT:
            /*
             * If we failed to create the font, act like we
             * don't support it.
             */
            if (!m_pTen->m_ambientProp.has_font())
                return ResultFromScode(DISP_E_MEMBERNOTFOUND);

            //The correct type is an IFontDisp pointer
            V_DISPATCH(pVarResult)=m_pTen->m_ambientProp.getFontRef();
            V_VT(pVarResult)=VT_FONT;
            break;

        case DISPID_AMBIENT_LOCALEID:
            V_I4(pVarResult)=m_pTen->m_ambientProp.getLCID();
            V_VT(pVarResult)=VT_I4;
            break;

        case DISPID_AMBIENT_USERMODE:
          V_BOOL(pVarResult)=!m_pTen->m_ambientProp.getDesignMode();
          V_VT(pVarResult) = VT_BOOL;
            break;

        case DISPID_AMBIENT_UIDEAD:
            //V_BOOL(pVarResult)=m_pTen->m_ambientProp.getUIDead();
            V_BOOL(pVarResult)=FALSE;
            V_VT(pVarResult) = VT_BOOL;

            break;

        case DISPID_AMBIENT_SUPPORTSMNEMONICS:
            V_BOOL(pVarResult)=TRUE;
            break;

        case DISPID_AMBIENT_SHOWGRABHANDLES:
            //V_BOOL(pVarResult)=m_pTen->m_ambientProp.m_fHatchHandles;
          V_BOOL(pVarResult)=FALSE;
          V_VT(pVarResult) = VT_BOOL;
            break;

        case DISPID_AMBIENT_SHOWHATCHING:
//            V_BOOL(pVarResult)=m_pTen->m_ambientProp.m_fHatchHandles;
          V_BOOL(pVarResult)=FALSE;
          V_VT(pVarResult) = VT_BOOL;
          break;

        default:
            hr=ResultFromScode(DISP_E_MEMBERNOTFOUND);
            break;
        }

    return hr;
    }
