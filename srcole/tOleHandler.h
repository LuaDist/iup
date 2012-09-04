// tOleHandler.h: interface for the tOleHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOLEHANDLER_H__49B97C12_8DEB_11D3_A906_0004ACE655F9__INCLUDED_)
#define AFX_TOLEHANDLER_H__49B97C12_8DEB_11D3_A906_0004ACE655F9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <olectl.h>

#include "tOleClientSite.h"
#include "tOleInPlaceSite.h"
#include "tOleControlSite.h"
#include "tDispatch.h"
#include "tAmbientProperties.h"
#include "tLegacy.h"

#ifndef LPOLEINPLACEOBJECT
typedef interface IOleInPlaceObject *LPOLEINPLACEOBJECT;
#endif

class tOleClientSite;

class tOleHandler : public IUnknown  
{
public:
  friend class tOleClientSite;
  friend class tOleInPlaceSite;
  friend class tOleControlSite;
  friend class tDispatch;
	tOleHandler();
	virtual ~tOleHandler();
  tAmbientProperties m_ambientProp;

  HWND            m_hWnd;         
private:

  DWORD           m_cOpens;       //Count calls to Open

  BOOL            m_fInitialized; //Something here?
  LPUNKNOWN       m_pObj;         //The object here
  LPSTORAGE       m_pIStorage;    //Sub-storage for tenant

  FORMATETC       m_fe;           //Used to create the object
  DWORD           m_dwState;      //State flags
  RECTL           m_rcl;          //Space of this object
  CLSID           m_clsID;        //Object class (for statics)
  BOOL            m_fSetExtent;   //Call SetExtent on next run

  ULONG           m_cRef;             //We're an object now
  LPOLEOBJECT     m_pIOleObject;      //IOleObject on m_pObj
  LPVIEWOBJECT2   m_pIViewObject2;    //IViewObject2 on m_pObj
  ULONG           m_grfMisc;          //OLEMISC flags
  BOOL            m_fRepaintEnabled;  //No redundant paints

  //Our interfaces
  tOleClientSite  *m_pImpIOleClientSite;

  tOleInPlaceSite *m_pImpIOleIPSite; //Our interface
  LPOLEINPLACEOBJECT   m_pIOleIPObject;  //The object
  RECT                 m_rcPos;          //Last object rect
  BOOL                 m_fInRectSet;     //Prevent reentrancy

  //CHAPTER24MOD
  tOleControlSite    *m_pImpIOleControlSite; //Implemented
  tDispatch          *m_pImpIDispatch;       //Implemented

  //Control interfaces
  LPOLECONTROL        m_pIOleControl;      //Used

  //Other state information
  BOOL                m_fHaveControlInfo;
  CONTROLINFO         m_ctrlInfo;
  ULONG               m_cLockInPlace;
  BOOL                m_fPendingDeactivate;
  //End CHAPTER24MOD


/*
* This flag is used exculsively by the implementation of
* IOleUILinkContainer on the page we're in for the Links
* Dialog. Since we never use it ourselves, public here
* is no big deal.
*/
public:
  BOOL            m_fLinkAvail;

protected:
	void CalcNaturalSize(void);
	long natural_height;
	long natural_width;
  BOOL    ObjectInitialize(LPUNKNOWN);

  //CHAPTER24MOD
  BOOL    ControlInitialize(void);
  //End CHAPTER24MOD

public:
	void setDesignMode(bool value, bool notify);
	void GetNaturalSize(long *pWidth, long *pHeight);
	void OnShow(void);
  //Gotta have an IUnknown for delegation
  STDMETHODIMP QueryInterface(REFIID, LPVOID *);
  STDMETHODIMP_(ULONG) AddRef(void);
  STDMETHODIMP_(ULONG) Release(void);

  UINT        Create(LPVOID);
  BOOL        Open(LPSTORAGE);
  void        Close(BOOL);


  BOOL        Activate(LONG, LPMSG);

  void        EnableRepaint(BOOL);

  void        ObjectGet(LPUNKNOWN *);
  void        SizeGet(LPSIZEL, BOOL);
  void        SizeSet(LPSIZEL, BOOL, BOOL);
  void        RectGet(LPRECTL, BOOL);
  void        RectSet(LPRECTL, BOOL, BOOL);

  void        DeactivateInPlaceObject(BOOL);
  void        UpdateInPlaceObjectRects(LPCRECT, BOOL);
  HWND        ObjectWindow(void);

  //CHAPTER24MOD
  DWORD       GetControlFlags();
  BOOL        TryMnemonic(LPMSG);
  void        AmbientChange(DISPID);
  //End CHAPTER24MOD


};

#endif // !defined(AFX_TOLEHANDLER_H__49B97C12_8DEB_11D3_A906_0004ACE655F9__INCLUDED_)
