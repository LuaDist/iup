/*
 * INOLE.H
 *
 * Master header file for all Inside OLE samples.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Microsoft
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _INOLE_H_
#define _INOLE_H_

#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <ole2ver.h>

#ifdef INC_OLEUI
#ifdef WIN32
/*
 * Comment the next line and remove the comment from the
 * line after it to compile for Windows NT 3.5.
 */
#include <oledlg.h>
//#include <ole2ui.h>
#else
#include <ole2ui.h>
#endif
#endif

#ifdef INC_CONTROLS
#define INC_AUTOMATION
#endif

#ifdef INC_AUTOMATION
#ifndef WIN32
#include <dispatch.h>
#include <olenls.h>
#else
#include <oleauto.h>
#endif
#endif

#ifdef INC_CONTROLS
#include <olectl.h>
#ifndef INITGUIDS
#include <olectlid.h>
#endif
#endif

#ifdef WIN32
//#include <tchar.h>
#ifdef UNICODE
#include <wchar.h>
#endif
#endif

#ifndef WIN32
#include <shellapi.h>
#include <malloc.h>
#endif

#ifdef INC_CLASSLIB
extern "C"
    {
    #include <commdlg.h>
   #ifndef WIN32
    #include <print.h>
    #include <dlgs.h>
   #endif
    }

#include <classlib.h>
#endif


//Types that OLE2.H et. al. leave out

#ifndef PPVOID
typedef LPVOID * PPVOID;
#endif  //PPVOID


#ifdef _OLE2_H_   //May not include ole2.h at all times.

#ifndef PPOINTL
typedef POINTL * PPOINTL;
#endif  //PPOINTL


#ifndef _WIN32
#ifndef OLECHAR
typedef char OLECHAR;
typedef OLECHAR FAR* LPOLESTR;
typedef const OLECHAR FAR* LPCOLESTR;
#endif //OLECHAR
#endif //_WIN32


//Useful macros.
#define SETFormatEtc(fe, cf, asp, td, med, li)   \
    {\
    (fe).cfFormat=cf;\
    (fe).dwAspect=asp;\
    (fe).ptd=td;\
    (fe).tymed=med;\
    (fe).lindex=li;\
    }

#define SETDefFormatEtc(fe, cf, med)   \
    {\
    (fe).cfFormat=cf;\
    (fe).dwAspect=DVASPECT_CONTENT;\
    (fe).ptd=NULL;\
    (fe).tymed=med;\
    (fe).lindex=-1;\
    }


#define SETRECTL(rcl, l, t, r, b) \
    {\
    (rcl).left=l;\
    (rcl).top=t;\
    (rcl).right=r;\
    (rcl).bottom=b;\
    }

#define SETSIZEL(szl, h, v) \
    {\
    (szl).cx=h;\
    (szl).cy=v;\
    }


#define RECTLFROMRECT(rcl, rc)\
    {\
    (rcl).left=(long)(rc).left;\
    (rcl).top=(long)(rc).top;\
    (rcl).right=(long)(rc).right;\
    (rcl).bottom=(long)(rc).bottom;\
    }


#define RECTFROMRECTL(rc, rcl)\
    {\
    (rc).left=(int)(rcl).left;\
    (rc).top=(int)(rcl).top;\
    (rc).right=(int)(rcl).right;\
    (rc).bottom=(int)(rcl).bottom;\
    }


#define POINTLFROMPOINT(ptl, pt) \
    { \
    (ptl).x=(long)(pt).x; \
    (ptl).y=(long)(pt).y; \
    }


#define POINTFROMPOINTL(pt, ptl) \
    { \
    (pt).x=(int)(ptl).x; \
    (pt).y=(int)(ptl).y; \
    }

//Here's one that should be in windows.h
#define SETPOINT(pt, h, v) \
    {\
    (pt).x=h;\
    (pt).y=v;\
    }

#define SETPOINTL(ptl, h, v) \
    {\
    (ptl).x=h;\
    (ptl).y=v;\
    }

#endif  //_OLE2_H_

#ifdef INC_AUTOMATION

//Macros for setting DISPPARAMS structures
#define SETDISPPARAMS(dp, numArgs, pvArgs, numNamed, pNamed) \
    {\
    (dp).cArgs=numArgs;\
    (dp).rgvarg=pvArgs;\
    (dp).cNamedArgs=numNamed;\
    (dp).rgdispidNamedArgs=pNamed;\
    }

#define SETNOPARAMS(dp) SETDISPPARAMS(dp, 0, NULL, 0, NULL)

//Macros for setting EXCEPINFO structures
#define SETEXCEPINFO(ei, excode, src, desc, file, ctx, func, scd) \
    {\
    (ei).wCode=excode;\
    (ei).wReserved=0;\
    (ei).bstrSource=src;\
    (ei).bstrDescription=desc;\
    (ei).bstrHelpFile=file;\
    (ei).dwHelpContext=ctx;\
    (ei).pvReserved=NULL;\
    (ei).pfnDeferredFillIn=func;\
    (ei).scode=scd;\
    }


#define INITEXCEPINFO(ei) \
        SETEXCEPINFO(ei,0,NULL,NULL,NULL,0L,NULL,S_OK)

#endif


/*
 * State flags for IPersistStorage implementations.  These
 * are kept here to avoid repeating the code in all samples.
 */

typedef enum
    {
    PSSTATE_UNINIT,     //Uninitialized
    PSSTATE_SCRIBBLE,   //Scribble
    PSSTATE_ZOMBIE,     //No scribble
    PSSTATE_HANDSOFF    //Hand-off
    } PSSTATE;


/*
 * Identifers to describe which persistence model an object
 * is using, along with a union type that holds on the the
 * appropriate pointers that a client may need.
 */
typedef enum
    {
    PERSIST_UNKNOWN=0,
    PERSIST_STORAGE,
    PERSIST_STREAM,
    PERSIST_STREAMINIT,
    PERSIST_FILE
    } PERSIST_MODEL;

typedef struct
    {
    PERSIST_MODEL   psModel;
    union
        {
        IPersistStorage    *pIPersistStorage;
        IPersistStream     *pIPersistStream;
       #ifdef INC_CONTROLS
        IPersistStreamInit *pIPersistStreamInit;
       #endif
        IPersistFile       *pIPersistFile;
        } pIP;

    } PERSISTPOINTER, *PPERSISTPOINTER;


//To identify a storage in which to save, load, or create.
typedef struct
    {
    PERSIST_MODEL   psModel;
    union
        {
        IStorage    *pIStorage;
        IStream     *pIStream;
        } pIS;

    } STGPOINTER, *PSTGPOINTER;



//Type for an object-destroyed callback
typedef void (*PFNDESTROYED)(void);


//DeleteInterfaceImp calls 'delete' and NULLs the pointer
#define DeleteInterfaceImp(p)\
            {\
            if (NULL!=p)\
                {\
                delete p;\
                p=NULL;\
                }\
            }


//ReleaseInterface calls 'Release' and NULLs the pointer
#define ReleaseInterface(p)\
            {\
            IUnknown *pt=(IUnknown *)p;\
            p=NULL;\
            if (NULL!=pt)\
                pt->Release();\
            }


//OLE Documents Clipboard Formats

#define CFSTR_EMBEDSOURCE       TEXT("Embed Source")
#define CFSTR_EMBEDDEDOBJECT    TEXT("Embedded Object")
#define CFSTR_LINKSOURCE        TEXT("Link Source")
#define CFSTR_CUSTOMLINKSOURCE  TEXT("Custom Link Source")
#define CFSTR_OBJECTDESCRIPTOR  TEXT("Object Descriptor")
#define CFSTR_LINKSRCDESCRIPTOR TEXT("Link Source Descriptor")



//Functions in the helper DLL, INOLE.DLL

//UI Effects
STDAPI_(void) UIDrawHandles(LPRECT, HDC, DWORD, UINT, BOOL);
STDAPI_(void) UIDrawShading(LPRECT, HDC, DWORD, UINT);
STDAPI_(void) UIShowObject(LPCRECT, HDC, BOOL);

//For UIDrawHandles
#define UI_HANDLES_USEINVERSE    0x00000001L
#define UI_HANDLES_NOBORDER      0x00000002L
#define UI_HANDLES_INSIDE        0x00000004L
#define UI_HANDLES_OUTSIDE       0x00000008L

//For UIDrawShading
#define UI_SHADE_FULLRECT        1
#define UI_SHADE_BORDERIN        2
#define UI_SHADE_BORDEROUT       3

//Coordinate Munging
STDAPI_(int)  XformWidthInHimetricToPixels(HDC,  int);
STDAPI_(int)  XformWidthInPixelsToHimetric(HDC,  int);
STDAPI_(int)  XformHeightInHimetricToPixels(HDC, int);
STDAPI_(int)  XformHeightInPixelsToHimetric(HDC, int);

STDAPI_(void) XformRectInPixelsToHimetric(HDC, LPRECT,  LPRECT);
STDAPI_(void) XformRectInHimetricToPixels(HDC, LPRECT,  LPRECT);
STDAPI_(void) XformSizeInPixelsToHimetric(HDC, LPSIZEL, LPSIZEL);
STDAPI_(void) XformSizeInHimetricToPixels(HDC, LPSIZEL, LPSIZEL);


//Helpers
STDAPI_(void)     INOLE_MetafilePictIconFree(HGLOBAL);
STDAPI            INOLE_SetIconInCache(IUnknown *, HGLOBAL);
STDAPI_(UINT)     INOLE_GetUserTypeOfClass(REFCLSID, UINT, LPTSTR
                      , UINT);
STDAPI            INOLE_DoConvert(IStorage *, REFCLSID);
STDAPI_(LPTSTR)   INOLE_CopyString(LPTSTR);
STDAPI_(HGLOBAL)  INOLE_ObjectDescriptorFromOleObject(IOleObject *
                      , DWORD, POINTL, LPSIZEL);
STDAPI_(HGLOBAL)  INOLE_AllocObjectDescriptor(CLSID, DWORD
                      , SIZEL, POINTL, DWORD, LPTSTR, LPTSTR);
STDAPI_(IStorage *) INOLE_CreateStorageOnHGlobal(DWORD);
STDAPI            INOLE_GetLinkSourceData(IMoniker *, LPCLSID
                      , LPFORMATETC, LPSTGMEDIUM);
STDAPI_(void)     INOLE_RegisterAsRunning(IUnknown *, IMoniker *
                      , DWORD, LPDWORD);
STDAPI_(void)     INOLE_RevokeAsRunning(LPDWORD);
STDAPI_(void)     INOLE_NoteChangeTime(DWORD, FILETIME *, LPTSTR);



/*
 * These are for ANSI compilations on Win32.  Source code assumes
 * a Win32 Unicode environment (or Win16 Ansi).  These macros
 * route Win32 ANSI compiled functions to wrappers which do the
 * proper Unicode conversion.
 */


#ifdef WIN32ANSI
STDAPI INOLE_ConvertStringToANSI(LPCWSTR, LPSTR *);
STDAPI INOLE_StringFromCLSID(REFCLSID, LPSTR *);
STDAPI INOLE_StringFromGUID2(REFGUID, LPSTR, int);
STDAPI INOLE_ProgIDFromCLSID(REFCLSID, LPSTR *);

STDAPI INOLE_ReadFmtUserTypeStg(IStorage *, CLIPFORMAT *, LPSTR *);
STDAPI INOLE_WriteFmtUserTypeStg(IStorage *, CLIPFORMAT, LPSTR);

STDAPI INOLE_StgIsStorageFile(LPCSTR);
STDAPI INOLE_StgCreateDocfile(LPCSTR, DWORD, DWORD, IStorage **);
STDAPI INOLE_StgOpenStorage(LPCSTR, IStorage *, DWORD, SNB
           , DWORD, IStorage **);


STDAPI INOLE_CreateFileMoniker(LPCSTR, LPMONIKER *);
STDAPI INOLE_CreateItemMoniker(LPCSTR, LPCSTR, LPMONIKER *);
STDAPI INOLE_MkParseDisplayName(LPBC, LPCSTR, ULONG *, LPMONIKER *);

STDAPI INOLE_OleCreateLinkToFile(LPCSTR, REFIID, DWORD, LPFORMATETC
           , LPOLECLIENTSITE, LPSTORAGE, LPVOID *);
STDAPI INOLE_OleCreateFromFile(REFCLSID, LPCSTR, REFIID
           , DWORD, LPFORMATETC, LPOLECLIENTSITE, LPSTORAGE, LPVOID *);


#ifndef NOMACROREDIRECT
#undef StringFromCLSID
#define StringFromCLSID(c, pp) INOLE_StringFromCLSID(c, pp)

#undef StringFromGUID2
#define StringFromGUID2(r, p, i) INOLE_StringFromGUID2(r, p, i)

#undef ProgIDFromCLSID
#define ProgIDFromCLSID(c, pp) INOLE_ProgIDFromCLSID(c, pp)

#undef ReadFmtUserTypeStg
#define ReadFmtUserTypeStg(p, c, s) INOLE_ReadFmtUserTypeStg(p, c, s)

#undef WriteFmtUserTypeStg
#define WriteFmtUserTypeStg(p, c, s) INOLE_WriteFmtUserTypeStg(p, c, s)

#undef StgIsStorageFile
#define StgIsStorageFile(s) INOLE_StgIsStorageFile(s)

#undef StgCreateDocfile
#define StgCreateDocfile(a, b, c, d) INOLE_StgCreateDocfile(a, b, c, d)

#undef StgOpenStorage
#define StgOpenStorage(a,b,c,d,e,f) INOLE_StgOpenStorage(a,b,c,d,e,f)

#undef CreateFileMoniker
#define CreateFileMoniker(p, i) INOLE_CreateFileMoniker(p, i)

#undef CreateItemMoniker
#define CreateItemMoniker(p1, p2, i) INOLE_CreateItemMoniker(p1, p2, i)

#undef MkParseDisplayName
#define MkParseDisplayName(b, p, u, i) INOLE_MkParseDisplayName(b, p, u, i)

#undef OleCreateLinkToFile
#define OleCreateLinkToFile(s, i, d, fe, cs, st, pv) INOLE_OleCreateLinkToFile(s, i, d, fe, cs, st, pv)

#undef OleCreateFromFile
#define OleCreateFromFile(c, s, i, d, fe, cs, st, pv) INOLE_OleCreateFromFile(c, s, i, d, fe, cs, st, pv)


#endif

#endif

//Versioning.
#define VERSIONMAJOR                2
#define VERSIONMINOR                0
#define VERSIONCURRENT              0x00020000

#define HIMETRIC_PER_INCH           2540
#define LOMETRIC_PER_INCH           254
#define LOMETRIC_BORDER             60          //Border around page

#define CREATE_FAILED               0
#define CREATE_GRAPHICONLY          1

//State flags
//CHAPTER24MOD
//Switched #defines to an enum
enum
    {
    TENANTSTATE_DEFAULT =0x00000000,
    TENANTSTATE_SELECTED=0x00000001,
    TENANTSTATE_OPEN    =0x00000002,
    TENANTSTATE_SHOWTYPE=0x00000004,

    //New ones for controls.
    TENANTSTATE_CONTROL     =0x00000100,
    TENANTSTATE_EVENTS      =0x00000200,
    TENANTSTATE_BUTTON      =0x00000400,
    TENANTSTATE_LABEL       =0x00000800,
    TENANTSTATE_DEFBUTTON   =0x00001000,
    TENANTSTATE_CANCELBUTTON=0x00002000
    };

//To mask off non-control flags
#define STATEMASK_CONTROLS 0xFFFFFF00



/*
 * Structures to save with the document describing the device
 * configuration and pages that we have.  This is followed by
 * a list of DWORD IDs for the individual pages.
 */

typedef struct tagDEVICECONFIG
    {
    DWORD       cb;                         //Size of structure
    TCHAR       szDriver[CCHDEVICENAME];
    TCHAR       szDevice[CCHDEVICENAME];
    TCHAR       szPort[CCHDEVICENAME];
    DWORD       cbDevMode;                  //Size of actual DEVMODE
    DEVMODE     dm;                         //Variable
    } DEVICECONFIG, *PDEVICECONFIG;

//Offset to cbDevMode
#define CBSEEKOFFSETCBDEVMODE  (sizeof(DWORD)   \
                               +(3*CCHDEVICENAME*sizeof(TCHAR)))

//Combined OLE and Patron device structures.
typedef struct tagCOMBINEDEVICE
    {
    DVTARGETDEVICE  td;
    DEVICECONFIG    dc;
    } COMBINEBDEVICE, *PCOMBINEDEVICE;



void             RectConvertMappings(LPRECT, HDC, BOOL);


//CHAPTER24MOD


#ifdef WIN32
#define PROP_POINTER    TEXT("Pointer")
#else
#define PROP_SELECTOR   "Selector"
#define PROP_OFFSET     "Offset"
#endif

//End CHAPTER24MOD

BOOL ObjectEventsIID(LPUNKNOWN pObj, IID *piid);
BOOL InterfaceConnect(LPUNKNOWN pObj, REFIID riid
    , LPUNKNOWN pIUnknownSink, LPDWORD pdwConn);
BOOL InterfaceDisconnect(LPUNKNOWN pObj, REFIID riid
    , LPDWORD pdwConn);
BOOL ObjectTypeInfo(LPUNKNOWN pObj, LPTYPEINFO *ppITypeInfo);
BOOL ObjectTypeInfoEvents(LPUNKNOWN pObj, LPTYPEINFO *ppITypeInfo);
BOOL ObjectEventsIID(LPUNKNOWN pObj, IID *piid);

#define HIMETRIC_PER_INCH   2540    //Number HIMETRIC units per inch
#define PTS_PER_INCH        72      //Number points (font size) per inch

#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)

//Useful macros.
#define SETFormatEtc(fe, cf, asp, td, med, li)   \
    {\
    (fe).cfFormat=cf;\
    (fe).dwAspect=asp;\
    (fe).ptd=td;\
    (fe).tymed=med;\
    (fe).lindex=li;\
    }

STDAPI_(void) XformRectInHimetricToPixels(HDC hDC, LPRECT prcHiMetric
	, LPRECT prcPix);

#endif //_INOLE_H_
