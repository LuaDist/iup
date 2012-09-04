// legacy.cpp

#include <windows.h>
#include "tLegacy.h"


/*
 * RectConvertMappings
 *
 * Purpose:
 *  Converts the contents of a rectangle from device to logical
 *  coordinates where the hDC defines the logical coordinates.
 *
 * Parameters:
 *  pRect           LPRECT containing the rectangle to convert.
 *  hDC             HDC describing the logical coordinate system.
 *                  if NULL, uses a screen DC in MM_LOMETRIC.
 *  fToDevice       BOOL TRUE to convert from uConv to device,
 *                  FALSE to convert device to uConv.
 *
 * Return Value:
 *  None
 */


void RectConvertMappings(LPRECT pRect, HDC hDC, BOOL fToDevice)
    {
    POINT   rgpt[2];
    BOOL    fSysDC=FALSE;

    if (NULL==pRect)
        return;

    rgpt[0].x=pRect->left;
    rgpt[0].y=pRect->top;
    rgpt[1].x=pRect->right;
    rgpt[1].y=pRect->bottom;

    if (NULL==hDC)
        {
        hDC=GetDC(NULL);
        SetMapMode(hDC, MM_LOMETRIC);
        fSysDC=TRUE;
        }

    if (fToDevice)
        LPtoDP(hDC, rgpt, 2);
    else
        DPtoLP(hDC, rgpt, 2);

    if (fSysDC)
        ReleaseDC(NULL, hDC);

    pRect->left=rgpt[0].x;
    pRect->top=rgpt[0].y;
    pRect->right=rgpt[1].x;
    pRect->bottom=rgpt[1].y;

    return;
    }

STDAPI_(void) XformRectInHimetricToPixels(HDC hDC, LPRECT prcHiMetric
	, LPRECT prcPix)
	{
	int     iXppli;     //Pixels per logical inch along width
	int     iYppli;     //Pixels per logical inch along height
	int     iXextInHiMetric=(prcHiMetric->right-prcHiMetric->left);
	int     iYextInHiMetric=(prcHiMetric->bottom-prcHiMetric->top);
	BOOL    fSystemDC=FALSE;

	if (NULL==hDC || GetDeviceCaps(hDC, LOGPIXELSX) == 0)
		{
		hDC=GetDC(NULL);
		fSystemDC=TRUE;
		}

	iXppli=GetDeviceCaps (hDC, LOGPIXELSX);
	iYppli=GetDeviceCaps (hDC, LOGPIXELSY);

	//We got pixel units, convert them to logical HIMETRIC along the display
	prcPix->right=MAP_LOGHIM_TO_PIX(iXextInHiMetric, iXppli);
	prcPix->top  =MAP_LOGHIM_TO_PIX(iYextInHiMetric, iYppli);

	prcPix->left =0;
	prcPix->bottom= 0;

	if (fSystemDC)
		ReleaseDC(NULL, hDC);

	return;
	}
/*
 * INOLE_SetIconInCache
 *
 * Purpose:
 *  Stores an iconic presentation metafile in the cache.
 *
 * Parameters:
 *  pObj            IUnknown * of the object.
 *  hMetaPict       HGLOBAL containing the presentation.
 *
 * Return Value:
 *  HRESULT         From IOleCache::SetData.
 */

#if 0
STDAPI INOLE_SetIconInCache(IUnknown *pObj, HGLOBAL hMetaPict)
    {
    IOleCache      *pCache;
    FORMATETC       fe;
    STGMEDIUM       stm;
    HRESULT         hr;

    if (NULL==hMetaPict)
        return ResultFromScode(E_INVALIDARG);

    if (FAILED(pObj->QueryInterface(IID_IOleCache, (void **)&pCache)))
        return ResultFromScode(E_NOINTERFACE);

    SETFormatEtc(fe, CF_METAFILEPICT, DVASPECT_ICON, NULL
        , TYMED_MFPICT, -1);

    stm.tymed=TYMED_MFPICT;
    stm.hGlobal=hMetaPict;
    stm.pUnkForRelease=NULL;

    hr=pCache->SetData(&fe, &stm, FALSE);
    pCache->Release();

    return hr;
    }
#endif

  

/*
 * XformRectInPixelsToHimetric
 * XformRectInHimetricToPixels
 *
 * Purpose:
 *  Convert a rectangle between pixels of a given hDC and HIMETRIC units
 *  as manipulated in OLE.  If the hDC is NULL, then a screen DC is used
 *  and assumes the MM_TEXT mapping mode.
 *
 * Parameters:
 *  hDC             HDC providing reference to the pixel mapping.  If
 *                  NULL, a screen DC is used.
 *  prcPix          LPRECT containng the rectangles to convert.
 *  prcHiMetric
 *
 * Return Value:
 *  None
 *
 * NOTE:
 *  When displaying on the screen, Window apps display everything enlarged
 *  from its actual size so that it is easier to read. For example, if an
 *  app wants to display a 1in. horizontal line, that when printed is
 *  actually a 1in. line on the printed page, then it will display the line
 *  on the screen physically larger than 1in. This is described as a line
 *  that is "logically" 1in. along the display width. Windows maintains as
 *  part of the device-specific information about a given display device:
 *      LOGPIXELSX -- no. of pixels per logical in along the display width
 *      LOGPIXELSY -- no. of pixels per logical in along the display height
 *
 *  The following formula converts a distance in pixels into its equivalent
 *  logical HIMETRIC units:
 *
 *      DistInHiMetric=(HIMETRIC_PER_INCH * DistInPix)
 *                      -------------------------------
 *                            PIXELS_PER_LOGICAL_IN
 *
 * Rect in Pixels (MM_TEXT):
 *
 *              0---------- X
 *              |
 *              |       1) ------------------ ( 2   P1=(rc.left, rc.top)
 *              |       |                     |     P2=(rc.right, rc.top)
 *              |       |                     |     P3=(rc.left, rc.bottom)
 *              |       |                     |     P4=(rc.right, rc.bottom)
 *                      |                     |
 *              Y       |                     |
 *                      3) ------------------ ( 4
 *
 *              NOTE:   Origin  =(P1x, P1y)
 *                      X extent=P4x - P1x
 *                      Y extent=P4y - P1y
 *
 *
 * Rect in Himetric (MM_HIMETRIC):
 *
 *
 *                      1) ------------------ ( 2   P1=(rc.left, rc.top)
 *              Y       |                     |     P2=(rc.right, rc.top)
 *                      |                     |     P3=(rc.left, rc.bottom)
 *              |       |                     |     P4=(rc.right, rc.bottom)
 *              |       |                     |
 *              |       |                     |
 *              |       3) ------------------ ( 4
 *              |
 *              0---------- X
 *
 *              NOTE:   Origin  =(P3x, P3y)
 *                      X extent=P2x - P3x
 *                      Y extent=P2y - P3y
 *
 *
 */

STDAPI_(void) XformRectInPixelsToHimetric(HDC hDC, LPRECT prcPix
	, LPRECT prcHiMetric)
	{
	int     iXppli;     //Pixels per logical inch along width
	int     iYppli;     //Pixels per logical inch along height
	int     iXextInPix=(prcPix->right-prcPix->left);
	int     iYextInPix=(prcPix->bottom-prcPix->top);
	BOOL    fSystemDC=FALSE;

	if (NULL==hDC || GetDeviceCaps(hDC, LOGPIXELSX) == 0)
		{
		hDC=GetDC(NULL);
		fSystemDC=TRUE;
		}

	iXppli=GetDeviceCaps (hDC, LOGPIXELSX);
	iYppli=GetDeviceCaps (hDC, LOGPIXELSY);

	//We got pixel units, convert them to logical HIMETRIC along the display
	prcHiMetric->right=MAP_PIX_TO_LOGHIM(iXextInPix, iXppli);
	prcHiMetric->top  =MAP_PIX_TO_LOGHIM(iYextInPix, iYppli);

	prcHiMetric->left   =0;
	prcHiMetric->bottom =0;

	if (fSystemDC)
		ReleaseDC(NULL, hDC);

	return;
	}


