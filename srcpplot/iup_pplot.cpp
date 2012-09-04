/*
 * IupPPlot component
 *
 * Description : A component, derived from PPlot and IUP canvas
 *      Remark : Depend on libs IUP, CD, IUPCD
 */


#ifdef _MSC_VER
#pragma warning(disable: 4100)
#pragma warning(disable: 4512)
#endif

//#define USE_OPENGL 1
// Not fully working, needs improvements in CD_GL

#ifdef USE_OPENGL
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"
#include "iup_pplot.h"
#include "iupkey.h"
#ifdef USE_OPENGL
#include "iupgl.h"
#endif

#include <cd.h>
#ifdef USE_OPENGL
#include <cdgl.h>
#else
#include <cdiup.h>
#include <cddbuf.h>
#include <cdirgb.h>
#endif

#include "iup_class.h"
#include "iup_register.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_assert.h"

#include "iupPPlot.h"
#include "iupPPlotInteraction.h"
#include "iuppplot.hpp"


#ifndef M_E
#define M_E 2.71828182846
#endif

struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */
  PPainterIup* plt;
};


static int iPPlotGetCDFontStyle(const char* value);


/* PPlot function pointer typedefs. */
typedef int (*IFnC)(Ihandle*, cdCanvas*); /* postdraw_cb, predraw_cb */
typedef int (*IFniiff)(Ihandle*, int, int, float, float); /* delete_cb */
typedef int (*IFniiffi)(Ihandle*, int, int, float, float, int); /* select_cb */
typedef int (*IFniiffff)(Ihandle*, int, int, float, float, float*, float*); /* edit_cb */


/* callback: forward redraw request to PPlot object */
static int iPPlotRedraw_CB(Ihandle* ih)
{
  ih->data->plt->Draw(0, 1);  /* full redraw only if nothing changed */
  return IUP_DEFAULT;
}

/* callback: forward resize request to PPlot object */
static int iPPlotResize_CB(Ihandle* ih, int w, int h)
{
  ih->data->plt->Resize(w, h);
  return IUP_DEFAULT;
}

/* callback: forward mouse button events to PPlot object */
static int iPPlotMouseButton_CB(Ihandle* ih, int btn, int stat, int x, int y, char* r)
{
  ih->data->plt->MouseButton(btn, stat, x, y, r);
  return IUP_DEFAULT;
}

/* callback: forward mouse button events to PPlot object */
static int iPPlotMouseMove_CB(Ihandle* ih, int x, int y)
{
  ih->data->plt->MouseMove(x, y);
  return IUP_DEFAULT;
}

/* callback: forward keyboard events to PPlot object */
static int iPPlotKeyPress_CB(Ihandle* ih, int c, int press)
{
  ih->data->plt->KeyPress(c, press);
  return IUP_DEFAULT;
} 

/* user level call: add dataset to plot */
void IupPPlotBegin(Ihandle* ih, int strXdata)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* inXData = (PlotDataBase*)iupAttribGet(ih, "_IUP_PPLOT_XDATA");
  PlotDataBase* inYData = (PlotDataBase*)iupAttribGet(ih, "_IUP_PPLOT_YDATA");

  if (inXData) delete inXData;
  if (inYData) delete inYData;

  if (strXdata)
    inXData = (PlotDataBase*)(new StringPlotData());
  else
    inXData = (PlotDataBase*)(new PlotData());

  inYData = (PlotDataBase*)new PlotData();

  iupAttribSetStr(ih, "_IUP_PPLOT_XDATA",    (char*)inXData);
  iupAttribSetStr(ih, "_IUP_PPLOT_YDATA",    (char*)inYData);
}

void IupPPlotAdd(Ihandle* ih, float x, float y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotData* inXData = (PlotData*)iupAttribGet(ih, "_IUP_PPLOT_XDATA");
  PlotData* inYData = (PlotData*)iupAttribGet(ih, "_IUP_PPLOT_YDATA");

  if (!inYData || !inXData || inXData->IsString())
    return;

  inXData->push_back(x);
  inYData->push_back(y);
}

void IupPPlotAddStr(Ihandle* ih, const char* x, float y)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  StringPlotData *inXData = (StringPlotData*)iupAttribGet(ih, "_IUP_PPLOT_XDATA");
  PlotData *inYData = (PlotData*)iupAttribGet(ih, "_IUP_PPLOT_YDATA");

  if (!inYData || !inXData || !inXData->IsString())
    return;

  inXData->AddItem(x);
  inYData->push_back(y);
}

void IupPPlotInsertStr(Ihandle* ih, int inIndex, int inSampleIndex, const char* inX, float inY)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  StringPlotData *theXData = (StringPlotData*)theXDataBase;
  PlotData *theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || !theXData->IsString())
    return;

  theXData->InsertItem(inSampleIndex, inX);
  theYData->insert(theYData->begin()+inSampleIndex, inY);
}

void IupPPlotInsert(Ihandle* ih, int inIndex, int inSampleIndex, float inX, float inY)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  PlotData* theXData = (PlotData*)theXDataBase;
  PlotData* theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || theXData->IsString())
    return;

  theXData->insert(theXData->begin()+inSampleIndex, inX);
  theYData->insert(theYData->begin()+inSampleIndex, inY);
}

void IupPPlotAddPoints(Ihandle* ih, int inIndex, float *x, float *y, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  PlotData* inXData = (PlotData*)theXDataBase;
  PlotData* inYData = (PlotData*)theYDataBase;

  if (!inYData || !inXData || inXData->IsString())
    return;

  for (int i=0; i<count; i++)
  {
    inXData->push_back(x[i]);
    inYData->push_back(y[i]);
  }
}

void IupPPlotAddStrPoints(Ihandle* ih, int inIndex, const char** x, float* y, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  StringPlotData *inXData = (StringPlotData*)theXDataBase;
  PlotData   *inYData = (PlotData*)theYDataBase;

  if (!inYData || !inXData || !inXData->IsString())
    return;

  for (int i=0; i<count; i++)
  {
    inXData->AddItem(x[i]);
    inYData->push_back(y[i]);
  }
}

void IupPPlotInsertStrPoints(Ihandle* ih, int inIndex, int inSampleIndex, const char** inX, float* inY, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  StringPlotData *theXData = (StringPlotData*)theXDataBase;
  PlotData   *theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || !theXData->IsString())
    return;

  for (int i=0; i<count; i++)
  {
    theXData->InsertItem(inSampleIndex+i, inX[i]);
    theYData->insert(theYData->begin()+(inSampleIndex+i), inY[i]);
  }
}

void IupPPlotInsertPoints(Ihandle* ih, int inIndex, int inSampleIndex, float *inX, float *inY, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  PlotDataBase* theXDataBase = ih->data->plt->_plot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = ih->data->plt->_plot.mPlotDataContainer.GetYData(inIndex);
  PlotData* theXData = (PlotData*)theXDataBase;
  PlotData* theYData = (PlotData*)theYDataBase;

  if (!theYData || !theXData || theXData->IsString())
    return;

  for (int i=0; i<count; i++)
  {
    theXData->insert(theXData->begin()+(inSampleIndex+i), inX[i]);
    theYData->insert(theYData->begin()+(inSampleIndex+i), inY[i]);
  }
}

int IupPPlotEnd(Ihandle* ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return -1;

  PlotDataBase* inXData = (PlotDataBase*)iupAttribGet(ih, "_IUP_PPLOT_XDATA");
  PlotDataBase* inYData = (PlotDataBase*)iupAttribGet(ih, "_IUP_PPLOT_YDATA");
  if (!inYData || !inXData)
    return -1;

  /* add to plot */
  ih->data->plt->_currentDataSetIndex = ih->data->plt->_plot.mPlotDataContainer.AddXYPlot(inXData, inYData);

  LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
  legend->mStyle.mFontStyle = iPPlotGetCDFontStyle(IupGetAttribute(ih, "LEGENDFONTSTYLE"));
  legend->mStyle.mFontSize  = IupGetInt(ih, "LEGENDFONTSIZE");

  iupAttribSetStr(ih, "_IUP_PPLOT_XDATA", NULL);
  iupAttribSetStr(ih, "_IUP_PPLOT_YDATA", NULL);

  ih->data->plt->_redraw = 1;
  return ih->data->plt->_currentDataSetIndex;
}

void IupPPlotTransform(Ihandle* ih, float x, float y, int *ix, int *iy)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  if (ix) *ix = ih->data->plt->_plot.Round(ih->data->plt->_plot.mXTrafo->Transform(x));
  if (iy) *iy = ih->data->plt->_plot.Round(ih->data->plt->_plot.mYTrafo->Transform(y));
}

/* user level call: plot on the given device */
void IupPPlotPaintTo(Ihandle* ih, void* _cnv)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "pplot"))
    return;

  ih->data->plt->DrawTo((cdCanvas *)_cnv);
}

/* --------------------------------------------------------------------
                      class implementation
   -------------------------------------------------------------------- */

PostPainterCallbackIup::PostPainterCallbackIup (PPlot &inPPlot, Ihandle* inHandle):
  _ih(inHandle)
{
  inPPlot.mPostDrawerList.push_back (this);
}

bool PostPainterCallbackIup::Draw(Painter &inPainter)
{
  IFnC cb = (IFnC)IupGetCallback(_ih, "POSTDRAW_CB");

  if (cb)
  {
    PPainterIup* iupPainter = (PPainterIup*)(&inPainter);
    cb(_ih, iupPainter->_cddbuffer);
  }

  return true;
}

PrePainterCallbackIup::PrePainterCallbackIup (PPlot &inPPlot, Ihandle* inHandle):
  _ih(inHandle)
{
  inPPlot.mPreDrawerList.push_back (this);
}

bool PrePainterCallbackIup::Draw(Painter &inPainter)
{
  IFnC cb = (IFnC)IupGetCallback(_ih, "PREDRAW_CB");
  if (cb)
  {
    PPainterIup* iupPainter = (PPainterIup*)(&inPainter);
    cb(_ih, iupPainter->_cddbuffer);
  }

  return true;
}

bool PDeleteInteractionIup::DeleteNotify(int inIndex, int inSampleIndex, PlotDataBase* inXData, PlotDataBase* inYData)
{
  IFniiff cb = (IFniiff)IupGetCallback(_ih, "DELETE_CB");
  if (cb)
  {
    if (inIndex == -1)
    {
      Icallback cbb = IupGetCallback(_ih, "DELETEBEGIN_CB");
      if (cbb && cbb(_ih) == IUP_IGNORE)
        return false;
    }
    else if (inIndex == -2)
    {
      Icallback cbb = IupGetCallback(_ih, "DELETEEND_CB");
      if (cbb)
        cbb(_ih);
    }
    else
    {
      float theX = inXData->GetValue(inSampleIndex);
      float theY = inYData->GetValue(inSampleIndex);
      int ret = cb(_ih, inIndex, inSampleIndex, theX, theY);
      if (ret == IUP_IGNORE)
        return false;
    }
  }

  return true;
}

bool PSelectionInteractionIup::SelectNotify(int inIndex, int inSampleIndex, PlotDataBase* inXData, PlotDataBase* inYData, bool inSelect)
{
  IFniiffi cb = (IFniiffi)IupGetCallback(_ih, "SELECT_CB");
  if (cb)
  {
    if (inIndex == -1)
    {
      Icallback cbb = IupGetCallback(_ih, "SELECTBEGIN_CB");
      if (cbb && cbb(_ih) == IUP_IGNORE)
        return false;
    }
    else if (inIndex == -2)
    {
      Icallback cbb = IupGetCallback(_ih, "SELECTEND_CB");
      if (cbb)
        cbb(_ih);
    }
    else
    {
      float theX = inXData->GetValue(inSampleIndex);
      float theY = inYData->GetValue(inSampleIndex);
      int ret = cb(_ih, inIndex, inSampleIndex, theX, theY, (int)inSelect);
      if (ret == IUP_IGNORE)
        return false;
    }
  }

  return true;
}

bool PEditInteractionIup::Impl_HandleKeyEvent (const PKeyEvent &inEvent)
{
  if (inEvent.IsArrowDown () || inEvent.IsArrowUp () ||
      inEvent.IsArrowLeft () || inEvent.IsArrowRight ())
    return true;

  return false;
};

bool PEditInteractionIup::Impl_Calculate (Painter &inPainter, PPlot& inPPlot)
{
  PlotDataContainer &theContainer = inPPlot.mPlotDataContainer;
  long thePlotCount = theContainer.GetPlotCount();

  if (!EditNotify(-1, 0, 0, 0, NULL, NULL))
    return false;

  for (long theI=0;theI<thePlotCount;theI++)
  {
    PlotDataBase* theXData = theContainer.GetXData (theI);
    PlotDataBase* theYData = theContainer.GetYData (theI);
    PlotDataSelection *thePlotDataSelection = theContainer.GetPlotDataSelection (theI);

    if (mKeyEvent.IsArrowDown () || mKeyEvent.IsArrowUp () ||
        mKeyEvent.IsArrowLeft () || mKeyEvent.IsArrowRight ())
      HandleCursorKey (thePlotDataSelection, theXData, theYData, theI);
  }

  EditNotify(-2, 0, 0, 0, NULL, NULL);

  return true;
}

void PEditInteractionIup::HandleCursorKey (const PlotDataSelection *inPlotDataSelection, PlotDataBase* inXData, PlotDataBase* inYData, int inIndex)
{
  float theXDelta = 0; /* pixels */
  if (mKeyEvent.IsArrowLeft () || mKeyEvent.IsArrowRight ())
  {
    theXDelta = 1;

    if (mKeyEvent.IsArrowLeft ())
      theXDelta *= -1;

    if (mKeyEvent.IsOnlyControlKeyDown ())
      theXDelta *= 10;
  }

  float theYDelta = 0; /* pixels */
  if (mKeyEvent.IsArrowDown () || mKeyEvent.IsArrowUp ())
  {
    theYDelta = 1;

    if (mKeyEvent.IsArrowDown ())
      theYDelta *= -1;

    if (mKeyEvent.IsOnlyControlKeyDown ())
      theYDelta *= 10;
  }

  for (int theI=0;theI<inYData->GetSize ();theI++)
  {
    if (inPlotDataSelection->IsSelected (theI))
    {
      float theX = inXData->GetValue(theI);
      float newX = theX;

      if (theXDelta)
      {
        float theXPixels = mPPlot.mXTrafo->Transform(theX);
        theXPixels += theXDelta;
        newX = mPPlot.mXTrafo->TransformBack(theXPixels);
      }

      float theY = inYData->GetValue(theI);
      float newY = theY;
      if (theYDelta)
      {
        float theYPixels = mPPlot.mYTrafo->Transform(theY);
        theYPixels -= theYDelta;  /* in pixels Y is descending */
        newY = mPPlot.mYTrafo->TransformBack(theYPixels);
      }

      if (!EditNotify(inIndex, theI, theX, theY, &newX, &newY))
        return;

      if (inXData->IsString())
      {
        StringPlotData *theXData = (StringPlotData*)(inXData);
        PlotData* theYData = (PlotData*)(inYData);
        theXData->mRealPlotData[theI] = newX;
        (*theYData)[theI] = newY;
      }
      else
      {
        PlotData* theXData = (PlotData*)(inXData);
        PlotData* theYData = (PlotData*)(inYData);
        (*theXData)[theI] = newX;
        (*theYData)[theI] = newY;
      }
    }
  }
}

bool PEditInteractionIup::EditNotify(int inIndex, int inSampleIndex, float inX, float inY, float *inNewX, float *inNewY)
{
  IFniiffff cb = (IFniiffff)IupGetCallback(_ih, "EDIT_CB");
  if (cb)
  {
    if (inIndex == -1)
    {
      Icallback cbb = IupGetCallback(_ih, "EDITBEGIN_CB");
      if (cbb && cbb(_ih) == IUP_IGNORE)
        return false;
    }
    else if (inIndex == -2)
    {
      Icallback cbb = IupGetCallback(_ih, "EDITEND_CB");
      if (cbb)
        cbb(_ih);
    }
    else
    {
      int ret = cb(_ih, inIndex, inSampleIndex, inX, inY, inNewX, inNewY);
      if (ret == IUP_IGNORE)
        return false;
    }
  }

  return true;
}

InteractionContainerIup::InteractionContainerIup(PPlot &inPPlot, Ihandle* inHandle):
  mZoomInteraction (inPPlot),
  mSelectionInteraction (inPPlot, inHandle),
  mEditInteraction (inPPlot, inHandle),
  mDeleteInteraction (inPPlot, inHandle),
  mCrosshairInteraction (inPPlot),
  mPostPainterCallback(inPPlot, inHandle),
  mPrePainterCallback(inPPlot, inHandle)
{
  AddInteraction (mZoomInteraction);
  AddInteraction (mSelectionInteraction);
  AddInteraction (mEditInteraction);
  AddInteraction (mDeleteInteraction);
  AddInteraction (mCrosshairInteraction);
}

PPainterIup::PPainterIup(Ihandle *ih) : 
  Painter(),
  _ih(ih),
#ifndef USE_OPENGL
  _cdcanvas(NULL),
#endif
  _cddbuffer(NULL),
  _mouseDown(0),
  _currentDataSetIndex(-1),
  _redraw(1)
{
  _plot.mShowLegend = false; /* change default to hidden */
  _plot.mPlotBackground.mTransparent = false;  /* always draw the background */
  _plot.mMargins.mLeft = 15;
  _plot.mMargins.mBottom = 15;
  _plot.mMargins.mTop = 30;
  _plot.mMargins.mRight = 15;
  _plot.mXAxisSetup.mTickInfo.mTickDivision = 5;
  _plot.mYAxisSetup.mTickInfo.mTickDivision = 5;
  _plot.mXAxisSetup.mTickInfo.mMinorTickScreenSize = 5;
  _plot.mYAxisSetup.mTickInfo.mMinorTickScreenSize = 5;
  _plot.mXAxisSetup.mTickInfo.mMajorTickScreenSize = 8;
  _plot.mYAxisSetup.mTickInfo.mMajorTickScreenSize = 8;

  _InteractionContainer = new InteractionContainerIup(_plot, _ih);

} /* c-tor */


PPainterIup::~PPainterIup()
{
  delete _InteractionContainer;
} /* d-tor */

class MarkDataDrawer: public LineDataDrawer
{
 public:
   MarkDataDrawer (bool inDrawLine)
   {
     mDrawLine = inDrawLine;
     mDrawPoint = true;
     mMode = inDrawLine ? "MARKLINE" : "MARK";
   };
   virtual bool DrawPoint (int inScreenX, int inScreenY, const PRect &inRect, Painter &inPainter) const;
};

bool MarkDataDrawer::DrawPoint (int inScreenX, int inScreenY, const PRect &inRect, Painter &inPainter) const
{
  PPainterIup* painter = (PPainterIup*)&inPainter;
  cdCanvasMark(painter->_cddbuffer, inScreenX, cdCanvasInvertYAxis(painter->_cddbuffer, inScreenY));

  return true;
}

static void RemoveSample(PPlot& inPPlot, int inIndex, int inSampleIndex)
{
  PlotDataBase* theXDataBase = inPPlot.mPlotDataContainer.GetXData(inIndex);
  PlotDataBase* theYDataBase = inPPlot.mPlotDataContainer.GetYData(inIndex);

  if (theXDataBase->IsString())
  {
    StringPlotData *theXData = (StringPlotData *)theXDataBase;
    PlotData* theYData = (PlotData*)theYDataBase;
    theXData->mRealPlotData.erase(theXData->mRealPlotData.begin()+inSampleIndex);
    theXData->mStringPlotData.erase(theXData->mStringPlotData.begin()+inSampleIndex);
    theYData->erase(theYData->begin()+inSampleIndex);
  }
  else
  {
    PlotData* theXData = (PlotData*)theXDataBase;
    PlotData* theYData = (PlotData*)theYDataBase;
    theXData->erase(theXData->begin()+inSampleIndex);
    theYData->erase(theYData->begin()+inSampleIndex);
  }
}

/* --------------------------------------------------------------------
                      CD Gets - size and style
   -------------------------------------------------------------------- */

static int iPPlotGetCDFontStyle(const char* value)
{
  if (!value)
    return -1;
  if (iupStrEqualNoCase(value, "PLAIN"))
    return CD_PLAIN;
  if (iupStrEqualNoCase(value, "BOLD"))
    return CD_BOLD;
  if (iupStrEqualNoCase(value, "ITALIC"))
    return CD_ITALIC;
  if (iupStrEqualNoCase(value, "BOLDITALIC"))
    return CD_BOLD_ITALIC;
  return -1;
}

static char* iPPlotGetPlotFontSize(int size)
{
  if (size)
  {
    char* buffer = iupStrGetMemory(50);
    sprintf(buffer, "%d", size);
    return buffer;
  }
  else
    return NULL;
}

static char* iPPlotGetPlotFontStyle(int style)
{
  if (style >= CD_PLAIN && style <= CD_BOLD_ITALIC)
  {
    char* style_str[4] = {"PLAIN", "BOLD", "ITALIC", "BOLDITALIC"};
    return style_str[style];
  }
  else
    return NULL;
}

static char* iPPlotGetPlotPenStyle(int style)
{
  if (style >= CD_CONTINUOUS && style <= CD_DASH_DOT_DOT)
  {
    char* style_str[5] = {"CONTINUOUS", "DASHED", "DOTTED", "DASH_DOT", "DASH_DOT_DOT"};
    return style_str[style];
  }
  else
    return NULL;
}

static int iPPlotGetCDPenStyle(const char* value)
{
  if (!value || iupStrEqualNoCase(value, "CONTINUOUS"))
    return CD_CONTINUOUS;
  else if (iupStrEqualNoCase(value, "DASHED"))
    return CD_DASHED;
  else if (iupStrEqualNoCase(value, "DOTTED"))
    return CD_DOTTED;
  else if (iupStrEqualNoCase(value, "DASH_DOT"))
    return CD_DASH_DOT;
  else if (iupStrEqualNoCase(value, "DASH_DOT_DOT"))
    return CD_DASH_DOT_DOT;
  else
    return CD_CONTINUOUS;
}
 
static char* iPPlotGetPlotMarkStyle(int style)
{
  if (style >= CD_PLUS && style <= CD_HOLLOW_DIAMOND)
  {
    char* style_str[9] = {"PLUS", "STAR", "CIRCLE", "X", "BOX", "DIAMOND", "HOLLOW_CIRCLE", "HOLLOW_BOX", "HOLLOW_DIAMOND"};
    return style_str[style];
  }
  else
    return NULL;
}

static int iPPlotGetCDMarkStyle(const char* value)
{
  if (!value || iupStrEqualNoCase(value, "PLUS"))
    return CD_PLUS;
  else if (iupStrEqualNoCase(value, "STAR"))
    return CD_STAR;
  else if (iupStrEqualNoCase(value, "CIRCLE"))
    return CD_CIRCLE;
  else if (iupStrEqualNoCase(value, "X"))
    return CD_X;
  else if (iupStrEqualNoCase(value, "BOX"))
    return CD_BOX;
  else if (iupStrEqualNoCase(value, "DIAMOND"))
    return CD_DIAMOND;
  else if (iupStrEqualNoCase(value, "HOLLOW_CIRCLE"))
    return CD_HOLLOW_CIRCLE;
  else if (iupStrEqualNoCase(value, "HOLLOW_BOX"))
    return CD_HOLLOW_BOX;
  else if (iupStrEqualNoCase(value, "HOLLOW_DIAMOND"))
    return CD_HOLLOW_DIAMOND;
  else
    return CD_PLUS;
}

/*****************************************************************************/
/***** SET AND GET ATTRIBUTES ************************************************/
/*****************************************************************************/

/* refresh plot window (write only) */
static int iPPlotSetRedrawAttrib(Ihandle* ih, const char* value)
{
  (void)value;  /* not used */
  ih->data->plt->Draw(1, 1);   /* force a full redraw here */
  return 0;
}

/* total number of datasets (read only) */
static char* iPPlotGetCountAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", ih->data->plt->_plot.mPlotDataContainer.GetPlotCount());
  return buffer;
}

/* legend box visibility */
static int iPPlotSetLegendShowAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->plt->_plot.mShowLegend = true;
  else 
    ih->data->plt->_plot.mShowLegend = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetLegendShowAttrib(Ihandle* ih)
{
  if (ih->data->plt->_plot.mShowLegend)
    return "YES";
  else
    return "NO";
}

/* legend box visibility */
static int iPPlotSetLegendPosAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "TOPLEFT"))
    ih->data->plt->_plot.mLegendPos = PPLOT_TOPLEFT;
  else if (iupStrEqualNoCase(value, "BOTTOMLEFT"))
    ih->data->plt->_plot.mLegendPos = PPLOT_BOTTOMLEFT;
  else if (iupStrEqualNoCase(value, "BOTTOMRIGHT"))
    ih->data->plt->_plot.mLegendPos = PPLOT_BOTTOMRIGHT;
  else
    ih->data->plt->_plot.mLegendPos = PPLOT_TOPRIGHT;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetLegendPosAttrib(Ihandle* ih)
{
  char* legendpos_str[4] = {"TOPLEFT", "TOPRIGHT", "BOTTOMLEFT", "BOTTOMRIGHT"};

  return legendpos_str[ih->data->plt->_plot.mLegendPos];
}

/* background color */
static int iPPlotSetBGColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    ih->data->plt->_plot.mPlotBackground.mPlotRegionBackColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetBGColorAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d %d %d",
          ih->data->plt->_plot.mPlotBackground.mPlotRegionBackColor.mR,
          ih->data->plt->_plot.mPlotBackground.mPlotRegionBackColor.mG,
          ih->data->plt->_plot.mPlotBackground.mPlotRegionBackColor.mB);
  return buffer;
}


/* title color */
static int iPPlotSetFGColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    ih->data->plt->_plot.mPlotBackground.mTitleColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetFGColorAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d %d %d",
          ih->data->plt->_plot.mPlotBackground.mTitleColor.mR,
          ih->data->plt->_plot.mPlotBackground.mTitleColor.mG,
          ih->data->plt->_plot.mPlotBackground.mTitleColor.mB);
  return buffer;
}


/* plot title */
static int iPPlotSetTitleAttrib(Ihandle* ih, const char* value)
{
  if (value && value[0] != 0)
    ih->data->plt->_plot.mPlotBackground.mTitle = value;
  else
    ih->data->plt->_plot.mPlotBackground.mTitle.resize(0);

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetTitleAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(256);
  strncpy(buffer, ih->data->plt->_plot.mPlotBackground.mTitle.c_str(), 256);
  buffer[255]='\0';
  return buffer;
}


/* plot title font size */
static int iPPlotSetTitleFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mPlotBackground.mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetTitleFontSizeAttrib(Ihandle* ih)
{
  return iPPlotGetPlotFontSize(ih->data->plt->_plot.mPlotBackground.mStyle.mFontSize);
}


/* plot title font style */
static int iPPlotSetTitleFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    ih->data->plt->_plot.mPlotBackground.mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

/* legend font size */
static int iPPlotSetLegendFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii, xx;
  if (!iupStrToInt(value, &xx))
    return 0;

  for (ii = 0; ii < ih->data->plt->_plot.mPlotDataContainer.GetPlotCount(); ii++)
  {
    LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ii);
    legend->mStyle.mFontSize = xx;
  }

  ih->data->plt->_redraw = 1;
  return 1;
}

/* legend font style */
static int iPPlotSetLegendFontStyleAttrib(Ihandle* ih, const char* value)
{
  int ii;
  int style = iPPlotGetCDFontStyle(value);
  if (style == -1)
    return 0;

  for (ii = 0; ii < ih->data->plt->_plot.mPlotDataContainer.GetPlotCount(); ii++)
  {
    LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ii);
    legend->mStyle.mFontStyle = style;
  }

  ih->data->plt->_redraw = 1;
  return 1;
}

/* plot margins */
static int iPPlotSetMarginLeftAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mMargins.mLeft = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetMarginRightAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mMargins.mRight = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetMarginTopAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mMargins.mTop = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetMarginBottomAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mMargins.mBottom = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetMarginLeftAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", ih->data->plt->_plot.mMargins.mLeft);
  return buffer;
}

static char* iPPlotGetMarginRightAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", ih->data->plt->_plot.mMargins.mRight);
  return buffer;
}

static char* iPPlotGetMarginTopAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", ih->data->plt->_plot.mMargins.mTop);
  return buffer;
}

static char* iPPlotGetMarginBottomAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", ih->data->plt->_plot.mMargins.mBottom);
  return buffer;
}

/* plot grid color */
static int iPPlotSetGridColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    ih->data->plt->_plot.mGridInfo.mGridColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetGridColorAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d %d %d",
          ih->data->plt->_plot.mGridInfo.mGridColor.mR,
          ih->data->plt->_plot.mGridInfo.mGridColor.mG,
          ih->data->plt->_plot.mGridInfo.mGridColor.mB);
  return buffer;
}

/* plot grid line style */
static int iPPlotSetGridLineStyleAttrib(Ihandle* ih, const char* value)
{
  ih->data->plt->_plot.mGridInfo.mStyle.mPenStyle = iPPlotGetCDPenStyle(value);
  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetGridLineStyleAttrib(Ihandle* ih)
{
  return iPPlotGetPlotPenStyle(ih->data->plt->_plot.mGridInfo.mStyle.mPenStyle);
}

/* grid */
static int iPPlotSetGridAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "VERTICAL"))  /* vertical grid - X axis  */
  {
    ih->data->plt->_plot.mGridInfo.mXGridOn = true;
    ih->data->plt->_plot.mGridInfo.mYGridOn = false;
  }
  else if (iupStrEqualNoCase(value, "HORIZONTAL")) /* horizontal grid - Y axis */
  {
    ih->data->plt->_plot.mGridInfo.mYGridOn = true;
    ih->data->plt->_plot.mGridInfo.mXGridOn = false;
  }
  else if (iupStrEqualNoCase(value, "YES"))
  {
    ih->data->plt->_plot.mGridInfo.mXGridOn = true;
    ih->data->plt->_plot.mGridInfo.mYGridOn = true;
  }
  else
  {
    ih->data->plt->_plot.mGridInfo.mYGridOn = false;
    ih->data->plt->_plot.mGridInfo.mXGridOn = false;
  }

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetGridAttrib(Ihandle* ih)
{
  if (ih->data->plt->_plot.mGridInfo.mXGridOn && ih->data->plt->_plot.mGridInfo.mYGridOn)
    return "YES";
  else if (ih->data->plt->_plot.mGridInfo.mYGridOn)
    return "HORIZONTAL";
  else if (ih->data->plt->_plot.mGridInfo.mXGridOn)
    return "VERTICAL";
  else
    return "NO";
}

/* current dataset index */
static int iPPlotSetCurrentAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    int imax = ih->data->plt->_plot.mPlotDataContainer.GetPlotCount();
    ih->data->plt->_currentDataSetIndex = ( (ii>=0) && (ii<imax) ? ii : -1);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetCurrentAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", ih->data->plt->_currentDataSetIndex);
  return buffer;
}

/* remove a dataset */
static int iPPlotSetRemoveAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    ih->data->plt->_plot.mPlotDataContainer.RemoveElement(ii);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

/* clear all datasets */
static int iPPlotSetClearAttrib(Ihandle* ih, const char* value)
{
  ih->data->plt->_plot.mPlotDataContainer.ClearData();
  ih->data->plt->_redraw = 1;
  return 0;
}

/* =============================== */
/* current plot dataset attributes */
/* =============================== */

/* current plot line style */
static int iPPlotSetDSLineStyleAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  drawer->mStyle.mPenStyle = iPPlotGetCDPenStyle(value);

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSLineStyleAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);

  return iPPlotGetPlotPenStyle(drawer->mStyle.mPenStyle);
}

/* current plot line width */
static int iPPlotSetDSLineWidthAttrib(Ihandle* ih, const char* value)
{
  int ii;

  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;

  if (iupStrToInt(value, &ii))
  {
    DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
    drawer->mStyle.mPenWidth = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetDSLineWidthAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", drawer->mStyle.mPenWidth);
  return buffer;
}

/* current plot mark style */
static int iPPlotSetDSMarkStyleAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  drawer->mStyle.mMarkStyle = iPPlotGetCDMarkStyle(value);
  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSMarkStyleAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);

  return iPPlotGetPlotMarkStyle(drawer->mStyle.mMarkStyle);
}

/* current plot mark size */
static int iPPlotSetDSMarkSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;

  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  if (iupStrToInt(value, &ii))
  {
    DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
    drawer->mStyle.mMarkSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetDSMarkSizeAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", drawer->mStyle.mMarkSize);
  return buffer;
}

/* current dataset legend */
static int iPPlotSetDSLegendAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
    
  if (value)
    legend->mName = value;
  else
    legend->mName.resize(0);

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSLegendAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
  char* buffer = iupStrGetMemory(256);
  strncpy(buffer, legend->mName.c_str(), 255);
  buffer[255]='\0';
  return buffer;
}

/* current dataset line and legend color */
static int iPPlotSetDSColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;

  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;

  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
    legend->mColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetDSColorAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  LegendData* legend = ih->data->plt->_plot.mPlotDataContainer.GetLegendData(ih->data->plt->_currentDataSetIndex);
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d %d %d", legend->mColor.mR, legend->mColor.mG, legend->mColor.mB);
  return buffer;
}

/* show values */
static int iPPlotSetDSShowValuesAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
 
  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
    
  if (iupStrBoolean(value))
    drawer->mShowValues = true;
  else 
    drawer->mShowValues = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSShowValuesAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);
  if (drawer->mShowValues)
    return "YES";
  else
    return "NO";
}

/* current dataset drawing mode */
static int iPPlotSetDSModeAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  DataDrawerBase *theDataDrawer = NULL;
  ih->data->plt->_plot.mXAxisSetup.mDiscrete = false;
    
  if(iupStrEqualNoCase(value, "BAR"))
  {
    theDataDrawer = new BarDataDrawer();
    ih->data->plt->_plot.mXAxisSetup.mDiscrete = true;
  }
  else if(iupStrEqualNoCase(value, "MARK"))
    theDataDrawer = new MarkDataDrawer(0);
  else if(iupStrEqualNoCase(value, "MARKLINE"))
    theDataDrawer = new MarkDataDrawer(1);
  else  /* LINE */
    theDataDrawer = new LineDataDrawer();

  ih->data->plt->_plot.mPlotDataContainer.SetDataDrawer(ih->data->plt->_currentDataSetIndex, theDataDrawer);

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSModeAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  DataDrawerBase* drawer = ih->data->plt->_plot.mPlotDataContainer.GetDataDrawer(ih->data->plt->_currentDataSetIndex);

  return (char*)drawer->mMode;
}

/* allows selection and editing */
static int iPPlotSetDSEditAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  PlotDataSelection* dataselect = ih->data->plt->_plot.mPlotDataContainer.GetPlotDataSelection(ih->data->plt->_currentDataSetIndex);
    
  if (iupStrBoolean(value))
    dataselect->resize(ih->data->plt->_plot.mPlotDataContainer.GetConstYData(ih->data->plt->_currentDataSetIndex)->GetSize());
  else
    dataselect->clear();

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetDSEditAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  PlotDataSelection* dataselect = ih->data->plt->_plot.mPlotDataContainer.GetPlotDataSelection(ih->data->plt->_currentDataSetIndex);
  if (dataselect->empty())
    return "NO";
  else
    return "YES";
}

/* remove a sample */
static int iPPlotSetDSRemoveAttrib(Ihandle* ih, const char* value)
{
  int ii;

  if (ih->data->plt->_currentDataSetIndex <  0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return 0;
  
  if (iupStrToInt(value, &ii))
  {
    RemoveSample(ih->data->plt->_plot, ih->data->plt->_currentDataSetIndex, ii);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetDSCountAttrib(Ihandle* ih)
{
  if (ih->data->plt->_currentDataSetIndex < 0 ||
      ih->data->plt->_currentDataSetIndex >= ih->data->plt->_plot.mPlotDataContainer.GetPlotCount())
    return NULL;

  {
    char* buffer = iupStrGetMemory(30);
    int count = ih->data->plt->_plot.mPlotDataContainer.GetCount(ih->data->plt->_currentDataSetIndex);
    sprintf(buffer, "%d", count);
    return buffer;
  }
}

/* ========== */
/* axis props */
/* ========== */

/* ========== */
/* axis props */
/* ========== */

/* axis title */
static int iPPlotSetAxisXLabelAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (value)
    axis->mLabel = value;
  else
    axis->mLabel = "";

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYLabelAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (value)
    axis->mLabel = value;
  else
    axis->mLabel = "";

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXLabelAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(256);
  strncpy(buffer, axis->mLabel.c_str(), 255);
  buffer[255]='\0';
  return buffer;
}

static char* iPPlotGetAxisYLabelAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(256);
  strncpy(buffer, axis->mLabel.c_str(), 255);
  buffer[255]='\0';
  return buffer;
}

/* axis title position */
static int iPPlotSetAxisXLabelCenteredAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mLabelCentered = true;
  else 
   axis->mLabelCentered = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYLabelCenteredAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mLabelCentered = true;
  else 
   axis->mLabelCentered = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXLabelCenteredAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mLabelCentered)
    return "YES";
  else
    return "NO";
}

static char* iPPlotGetAxisYLabelCenteredAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mLabelCentered)
    return "YES";
  else
    return "NO";
}

/* axis, ticks and label color */
static int iPPlotSetAxisXColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char rr, gg, bb;
  if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mColor = PColor(rr, gg, bb);
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXColorAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d %d %d",
          axis->mColor.mR,
          axis->mColor.mG,
          axis->mColor.mB);
  return buffer;
}

static char* iPPlotGetAxisYColorAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d %d %d",
          axis->mColor.mR,
          axis->mColor.mG,
          axis->mColor.mB);
  return buffer;
}

/* autoscaling */
static int iPPlotSetAxisXAutoMinAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mAutoScaleMin = true;
  else 
    axis->mAutoScaleMin = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYAutoMinAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mAutoScaleMin = true;
  else 
    axis->mAutoScaleMin = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXAutoMinAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mAutoScaleMin)
    return "YES";
  else
    return "NO";
}

static char* iPPlotGetAxisYAutoMinAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mAutoScaleMin)
    return "YES";
  else
    return "NO";
}

/* autoscaling */
static int iPPlotSetAxisXAutoMaxAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mAutoScaleMax = true;
  else 
    axis->mAutoScaleMax = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYAutoMaxAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mAutoScaleMax = true;
  else 
    axis->mAutoScaleMax = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXAutoMaxAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mAutoScaleMax)
    return "YES";
  else
    return "NO";
}

static char* iPPlotGetAxisYAutoMaxAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mAutoScaleMax)
    return "YES";
  else
    return "NO";
}

/* min visible val */
static int iPPlotSetAxisXMinAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mMin = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYMinAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mMin = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXMinAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%g", axis->mMin);
  return buffer;
}

static char* iPPlotGetAxisYMinAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%g", axis->mMin);
  return buffer;
}

/* max visible val */
static int iPPlotSetAxisXMaxAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mMax = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYMaxAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mMax = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXMaxAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%g", axis->mMax);
  return buffer;
}

static char* iPPlotGetAxisYMaxAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%g", axis->mMax);
  return buffer;
}

/* values from left/top to right/bottom */
static int iPPlotSetAxisXReverseAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mAscending = false; /* inverted for X */
  else 
    axis->mAscending = true;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYReverseAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mAscending = true; /* NOT inverted for Y  */
  else 
    axis->mAscending = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXReverseAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mAscending)
    return "NO";    /* inverted for X */
  else
    return "YES";
}

static char* iPPlotGetAxisYReverseAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mAscending)
    return "YES";    /* NOT inverted for Y */
  else
    return "NO";
}

/* axis mode */
static int iPPlotSetAxisXCrossOriginAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mCrossOrigin = true;
  else 
    axis->mCrossOrigin = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYCrossOriginAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mCrossOrigin = true;
  else 
    axis->mCrossOrigin = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXCrossOriginAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mCrossOrigin)
    return "YES";
  else
    return "NO";
}

static char* iPPlotGetAxisYCrossOriginAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mCrossOrigin)
    return "YES";
  else
    return "NO";
}

/* log/lin scale */
static int iPPlotSetAxisXScaleAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if(iupStrEqualNoCase(value, "LIN"))
  {
    axis->mLogScale = false;
  }
  else if(iupStrEqualNoCase(value, "LOG10"))
  {
    axis->mLogScale = true;
    axis->mLogBase  = 10.0;
  }
  else if(iupStrEqualNoCase(value, "LOG2"))
  {
    axis->mLogScale = true;
    axis->mLogBase  = 2.0;
  }
  else
  {
    axis->mLogScale = true;
    axis->mLogBase  = (float)M_E;
  }

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYScaleAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if(iupStrEqualNoCase(value, "LIN"))
  {
    axis->mLogScale = false;
  }
  else if(iupStrEqualNoCase(value, "LOG10"))
  {
    axis->mLogScale = true;
    axis->mLogBase  = 10.0;
  }
  else if(iupStrEqualNoCase(value, "LOG2"))
  {
    axis->mLogScale = true;
    axis->mLogBase  = 2.0;
  }
  else
  {
    axis->mLogScale = true;
    axis->mLogBase  = (float)M_E;
  }

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXScaleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(30);

  if (axis->mLogScale)
  {
    if (axis->mLogBase == 10.0)
      strcpy(buffer, "LOG10");
    else if (axis->mLogBase == 2.0)
      strcpy(buffer, "LOG2");
    else
      strcpy(buffer, "LOGN");
  }
  else
    strcpy(buffer, "LIN");

  return buffer;
}

static char* iPPlotGetAxisYScaleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(30);

  if (axis->mLogScale)
  {
    if (axis->mLogBase == 10.0)
      strcpy(buffer, "LOG10");
    else if (axis->mLogBase == 2.0)
      strcpy(buffer, "LOG2");
    else
      strcpy(buffer, "LOGN");
  }
  else
    strcpy(buffer, "LIN");

  return buffer;
}

/* axis label font size */
static int iPPlotSetAxisXFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXFontSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iPPlotGetPlotFontSize(axis->mStyle.mFontSize);
}

static char* iPPlotGetAxisYFontSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iPPlotGetPlotFontSize(axis->mStyle.mFontSize);
}

/* axis label font style */
static int iPPlotSetAxisXFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXFontStyleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  return iPPlotGetPlotFontStyle(axis->mStyle.mFontStyle);
}

static char* iPPlotGetAxisYFontStyleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  return iPPlotGetPlotFontStyle(axis->mStyle.mFontStyle);
}

/* automatic tick size */
static int iPPlotSetAxisXAutoTickSizeAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mAutoTickSize = true;
  else 
    axis->mTickInfo.mAutoTickSize = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYAutoTickSizeAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mAutoTickSize = true;
  else 
    axis->mTickInfo.mAutoTickSize = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXAutoTickSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mTickInfo.mAutoTickSize)
    return "YES";
  else
    return "NO";
}

static char* iPPlotGetAxisYAutoTickSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mTickInfo.mAutoTickSize)
    return "YES";
  else
    return "NO";
}

/* size of ticks (in pixels) */
static int iPPlotSetAxisXTickSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mMinorTickScreenSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mMinorTickScreenSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", axis->mTickInfo.mMinorTickScreenSize);
  return buffer;
}

static char* iPPlotGetAxisYTickSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", axis->mTickInfo.mMinorTickScreenSize);
  return buffer;
}

/* size of major ticks (in pixels) */
static int iPPlotSetAxisXTickMajorSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mMajorTickScreenSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickMajorSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mMajorTickScreenSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickMajorSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", axis->mTickInfo.mMajorTickScreenSize);
  return buffer;
}

static char* iPPlotGetAxisYTickMajorSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", axis->mTickInfo.mMajorTickScreenSize);
  return buffer;
}

/* axis ticks font size */
static int iPPlotSetAxisXTickFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickFontSizeAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mStyle.mFontSize = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickFontSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iPPlotGetPlotFontSize(axis->mTickInfo.mStyle.mFontSize);
}

static char* iPPlotGetAxisYTickFontSizeAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iPPlotGetPlotFontSize(axis->mTickInfo.mStyle.mFontSize);
}

/* axis ticks number font style */
static int iPPlotSetAxisXTickFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickFontStyleAttrib(Ihandle* ih, const char* value)
{
  int style = iPPlotGetCDFontStyle(value);
  if (style != -1)
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mStyle.mFontStyle = style;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickFontStyleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  return iPPlotGetPlotFontSize(axis->mTickInfo.mStyle.mFontStyle);
}

static char* iPPlotGetAxisYTickFontStyleAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  return iPPlotGetPlotFontSize(axis->mTickInfo.mStyle.mFontStyle);
}

/* axis ticks number format */
static int iPPlotSetAxisXTickFormatAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (value && value[0]!=0)
    axis->mTickInfo.mFormatString = value;
  else
    axis->mTickInfo.mFormatString = "%.0f";

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYTickFormatAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (value && value[0]!=0)
    axis->mTickInfo.mFormatString = value;
  else
    axis->mTickInfo.mFormatString = "%.0f";

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXTickFormatAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(256);
  strncpy(buffer, axis->mTickInfo.mFormatString.c_str(), 255);
  buffer[255]='\0';
  return buffer;
}

static char* iPPlotGetAxisYTickFormatAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(256);
  strncpy(buffer, axis->mTickInfo.mFormatString.c_str(), 255);
  buffer[255]='\0';
  return buffer;
}

/* axis ticks */
static int iPPlotSetAxisXTickAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mTicksOn = true;
  else 
    axis->mTickInfo.mTicksOn = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYTickAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mTicksOn = true;
  else 
    axis->mTickInfo.mTicksOn = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXTickAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mTickInfo.mTicksOn)
    return "YES";
  else
    return "NO";
}

static char* iPPlotGetAxisYTickAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mTickInfo.mTicksOn)
    return "YES";
  else
    return "NO";
}

/* major tick spacing */
static int iPPlotSetAxisXTickMajorSpanAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mMajorTickSpan = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickMajorSpanAttrib(Ihandle* ih, const char* value)
{
  float xx;
  if (iupStrToFloat(value, &xx))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mMajorTickSpan = xx;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickMajorSpanAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%g", axis->mTickInfo.mMajorTickSpan);
  return buffer;
}

static char* iPPlotGetAxisYTickMajorSpanAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%g", axis->mTickInfo.mMajorTickSpan);
  return buffer;
}

/* number of ticks between major ticks */
static int iPPlotSetAxisXTickDivisionAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
    axis->mTickInfo.mTickDivision = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static int iPPlotSetAxisYTickDivisionAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
    axis->mTickInfo.mTickDivision = ii;
    ih->data->plt->_redraw = 1;
  }
  return 0;
}

static char* iPPlotGetAxisXTickDivisionAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", axis->mTickInfo.mTickDivision);
  return buffer;
}

static char* iPPlotGetAxisYTickDivisionAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", axis->mTickInfo.mTickDivision);
  return buffer;
}

/* auto tick spacing */
static int iPPlotSetAxisXAutoTickAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mAutoTick = true;
  else 
    axis->mTickInfo.mAutoTick = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static int iPPlotSetAxisYAutoTickAttrib(Ihandle* ih, const char* value)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (iupStrBoolean(value))
    axis->mTickInfo.mAutoTick = true;
  else 
    axis->mTickInfo.mAutoTick = false;

  ih->data->plt->_redraw = 1;
  return 0;
}

static char* iPPlotGetAxisXAutoTickAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mXAxisSetup;

  if (axis->mTickInfo.mAutoTick)
    return "YES";
  else
    return "NO";
}

static char* iPPlotGetAxisYAutoTickAttrib(Ihandle* ih)
{
  AxisSetup* axis = &ih->data->plt->_plot.mYAxisSetup;

  if (axis->mTickInfo.mAutoTick)
    return "YES";
  else
    return "NO";
}

/* MouseButton */
void PPainterIup::MouseButton(int btn, int stat, int x, int y, char *r)
{
  PMouseEvent theEvent;
  int theModifierKeys = 0;

  theEvent.mX = x;
  theEvent.mY = y;
  
  if(btn == IUP_BUTTON1)
  {
    theEvent.mType = ( stat!=0 ? (PMouseEvent::kDown) : (PMouseEvent::kUp) );
    _mouseDown = ( stat!=0 ? 1 : 0 );
  }
  else return;

  _mouse_ALT = 0;
  _mouse_SHIFT = 0;
  _mouse_CTRL = 0;

  if (iup_isalt(r))  /* signal Alt */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kAlt);
    _mouse_ALT = 1;
  }
  if (iup_iscontrol(r))  /* signal Ctrl */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kControl);
    _mouse_SHIFT = 1;
  }
  if (iup_isshift(r))  /* signal Shift */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kShift);
    _mouse_CTRL = 1;
  }
  theEvent.SetModifierKeys (theModifierKeys);

  if( _InteractionContainer->HandleMouseEvent(theEvent))
  {
    this->Draw(1, 1);
  } 
  else
  {
    /* ignore the event */
  }
}

/* MouseMove */
void PPainterIup::MouseMove(int x, int y)
{
  PMouseEvent theEvent;
  int theModifierKeys = 0;

  if(!_mouseDown ) return;

  theEvent.mX = x;
  theEvent.mY = y;

  theEvent.mType = PMouseEvent::kMove;
  if(_mouse_ALT)  /* signal Alt */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kAlt);
  }
  if(_mouse_SHIFT)  /* signal Shift */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kControl);
  }
  if(_mouse_CTRL)  /* signal Ctrl */
  {
    theModifierKeys = (theModifierKeys | PMouseEvent::kShift);
  }
  theEvent.SetModifierKeys (theModifierKeys);

  if(_InteractionContainer->HandleMouseEvent(theEvent))
  {
    this->Draw(1, 1);
  } 
  else
  {
    /* ignore the event */
  }
}

/* KeyPress */
void PPainterIup::KeyPress(int c, int press)
{
  int theModifierKeys = 0;
  int theRepeatCount = 0;
  PKeyEvent::EKey theKeyCode = PKeyEvent::kNone;
  char theChar = 0;

  if(!press) return;

  switch(c)
  {
    case K_cX:  /* CTRL + X */
      theModifierKeys = PMouseEvent::kControl;
      theKeyCode = PKeyEvent::kChar;
      theChar = 'x';
    break;
    case K_cY:  /* CTRL + Y */
      theModifierKeys = PMouseEvent::kControl;
      theKeyCode = PKeyEvent::kChar;
      theChar = 'y';
    break;
    case K_cR:  /* CTRL + R */
      theModifierKeys = PMouseEvent::kControl;
      theKeyCode = PKeyEvent::kChar;
      theChar = 'r';
    break;
    case K_cUP:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_UP:   /* Arrow */
      theKeyCode = PKeyEvent::kArrowUp;
    break;
    case K_cDOWN:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_DOWN:   /* Arrow */
      theKeyCode = PKeyEvent::kArrowDown;
    break;
    case K_cLEFT:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_LEFT:   /* Arrow */
      theKeyCode = PKeyEvent::kArrowLeft;
    break;
    case K_cRIGHT:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_RIGHT:   /* Arrow */
      theKeyCode = PKeyEvent::kArrowRight;
    break;
    case K_cDEL:  /* CTRL + Arrow */
      theModifierKeys = PMouseEvent::kControl;
    case K_DEL:   /* Arrow */
      theKeyCode = PKeyEvent::kDelete;
    break;
  }

  PKeyEvent theEvent (theKeyCode, theRepeatCount, theModifierKeys, theChar);

  if(_InteractionContainer->HandleKeyEvent(theEvent))
  {
    this->Draw(1, 1);
  } 
  else
  {
    /* ignore the event */
  }
}

/* Draw */
void PPainterIup::Draw(int force, int flush)
{
  if (!_cddbuffer)
    return;

#ifdef USE_OPENGL
  if (!IupGLIsCurrent(_ih))
    force = 1;
  IupGLMakeCurrent(_ih);
#endif
  cdCanvasActivate(_cddbuffer);

  if (force || _redraw)
  {
    cdCanvasClear(_cddbuffer);
    _plot.Draw(*this);
    _redraw = 0;
  }

  if (flush)
  {
    cdCanvasFlush(_cddbuffer);

#ifdef USE_OPENGL
    IupGLSwapBuffers(_ih);
#endif
  }
}

/* Resize */
void PPainterIup::Resize(int w, int h)
{
#ifndef USE_OPENGL
  if (!_cddbuffer)
  {
    /* update canvas size */
    cdCanvasActivate(_cdcanvas);

    /* this can fail if canvas size is zero */
    if (IupGetInt(_ih, "USE_IMAGERGB"))
      _cddbuffer = cdCreateCanvas(CD_DBUFFERRGB, _cdcanvas);
    else
      _cddbuffer = cdCreateCanvas(CD_DBUFFER, _cdcanvas);
  }
#endif

  if (!_cddbuffer)
    return;

#ifdef USE_OPENGL
  IupGLMakeCurrent(_ih);
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  char StrData[100];
  sprintf(StrData, "%dx%d", w, h);
  cdCanvasSetAttribute(_cddbuffer, "SIZE", StrData);
#endif

  /* update canvas size */
  cdCanvasActivate(_cddbuffer);

  _redraw = 1;

  return;
}

/* send plot to some other device */ 
void PPainterIup::DrawTo(cdCanvas *usrCnv)
{
  cdCanvas *old_cddbuffer  = _cddbuffer;
  _cddbuffer = usrCnv;

#ifndef USE_OPENGL
  cdCanvas *old_cdcanvas   = _cdcanvas;
  _cdcanvas = usrCnv;
#endif

  if (_cddbuffer)
    Draw(1, 0); /* no flush here */

  _cddbuffer = old_cddbuffer;
#ifndef USE_OPENGL
  _cdcanvas  = old_cdcanvas;
#endif
}

void PPainterIup::FillArrow(int inX1, int inY1, int inX2, int inY2, int inX3, int inY3)
{
  if (!_cddbuffer)
    return;

  cdCanvasBegin(_cddbuffer, CD_FILL);
  cdCanvasVertex(_cddbuffer, inX1, cdCanvasInvertYAxis(_cddbuffer, inY1));
  cdCanvasVertex(_cddbuffer, inX2, cdCanvasInvertYAxis(_cddbuffer, inY2));
  cdCanvasVertex(_cddbuffer, inX3, cdCanvasInvertYAxis(_cddbuffer, inY3));
  cdCanvasEnd(_cddbuffer);
}

/* DrawLine */
void PPainterIup::DrawLine(float inX1, float inY1, float inX2, float inY2)
{
  if (!_cddbuffer)
    return;

  cdfCanvasLine(_cddbuffer, inX1, cdfCanvasInvertYAxis(_cddbuffer, inY1),
                                           inX2, cdfCanvasInvertYAxis(_cddbuffer, inY2));
}

/* FillRect */
void PPainterIup::FillRect(int inX, int inY, int inW, int inH)
{
  if (!_cddbuffer)
    return;

  cdCanvasBox(_cddbuffer, inX, inX+inW, 
              cdCanvasInvertYAxis(_cddbuffer, inY),
              cdCanvasInvertYAxis(_cddbuffer, inY + inH - 1));
}

/* InvertRect */
void PPainterIup::InvertRect(int inX, int inY, int inW, int inH)
{
  long cprev;

  if (!_cddbuffer)
    return;

  cdCanvasWriteMode(_cddbuffer, CD_XOR);
  cprev = cdCanvasForeground(_cddbuffer, CD_WHITE);
  cdCanvasRect(_cddbuffer, inX, inX + inW - 1,
               cdCanvasInvertYAxis(_cddbuffer, inY),
               cdCanvasInvertYAxis(_cddbuffer, inY + inH - 1));
  cdCanvasWriteMode(_cddbuffer, CD_REPLACE);
  cdCanvasForeground(_cddbuffer, cprev);
}

/* SetClipRect */
void PPainterIup::SetClipRect(int inX, int inY, int inW, int inH)
{
  if (!_cddbuffer)
    return;

  cdCanvasClipArea(_cddbuffer, inX, inX + inW - 1,
                   cdCanvasInvertYAxis(_cddbuffer, inY),
                   cdCanvasInvertYAxis(_cddbuffer, inY + inH - 1));
  cdCanvasClip(_cddbuffer, CD_CLIPAREA);
}

/* GetWidth */
long PPainterIup::GetWidth() const
{
  int iret;

  if (!_cddbuffer)
    return IUP_DEFAULT;

  cdCanvasGetSize(_cddbuffer, &iret, NULL, NULL, NULL);

  return (long)iret;
}

/* GetHeight */
long PPainterIup::GetHeight() const
{
  int iret;

  if (!_cddbuffer)
    return IUP_NOERROR;

  cdCanvasGetSize(_cddbuffer, NULL, &iret, NULL, NULL);

  return (long)iret;
}

/* SetLineColor */
void PPainterIup::SetLineColor(int inR, int inG, int inB)
{
  if (!_cddbuffer)
    return;

  cdCanvasForeground(_cddbuffer, cdEncodeColor((unsigned char)inR,
                                               (unsigned char)inG,
                                               (unsigned char)inB));
}

/* SetFillColor */
void PPainterIup::SetFillColor(int inR, int inG, int inB)
{
  if (!_cddbuffer)
    return;

  cdCanvasForeground(_cddbuffer, cdEncodeColor((unsigned char)inR,
                                               (unsigned char)inG,
                                               (unsigned char)inB));
}

/* CalculateTextDrawSize */
long PPainterIup::CalculateTextDrawSize(const char *inString)
{
  int iw;

  if (!_cddbuffer)
    return IUP_NOERROR;

  cdCanvasGetTextSize(_cddbuffer, const_cast<char *>(inString), &iw, NULL);

  return iw;
}

/* GetFontHeight */
long PPainterIup::GetFontHeight() const
{
  int ih;

  if (!_cddbuffer)
    return IUP_NOERROR;

  cdCanvasGetFontDim(_cddbuffer, NULL, &ih, NULL, NULL);

  return ih;
}

/* DrawText */
/* this call leave all the hard job of alignment on painter side */
void PPainterIup::DrawText(int inX, int inY, short align, const char *inString)
{
  if (!_cddbuffer)
    return;

  cdCanvasTextAlignment(_cddbuffer, align);
  cdCanvasText(_cddbuffer, inX, cdCanvasInvertYAxis(_cddbuffer, inY), const_cast<char *>(inString));
}

/* DrawRotatedText */
void PPainterIup::DrawRotatedText(int inX, int inY, float inDegrees, short align, const char *inString)
{
  double aprev;

  if (!_cddbuffer)
    return;

  cdCanvasTextAlignment(_cddbuffer, align);
  aprev = cdCanvasTextOrientation(_cddbuffer, -inDegrees);
  cdCanvasText(_cddbuffer, inX, cdCanvasInvertYAxis(_cddbuffer, inY), const_cast<char *>(inString));
  cdCanvasTextOrientation(_cddbuffer, aprev);
}

void PPainterIup::SetStyle(const PStyle &inStyle)
{
  if (!_cddbuffer)
    return;

  cdCanvasLineWidth(_cddbuffer, inStyle.mPenWidth);
  cdCanvasLineStyle(_cddbuffer, inStyle.mPenStyle);

  cdCanvasNativeFont(_cddbuffer, IupGetAttribute(_ih, "FONT"));

  if (inStyle.mFontStyle != -1 || inStyle.mFontSize != 0)
    cdCanvasFont(_cddbuffer, NULL, inStyle.mFontStyle, inStyle.mFontSize);

  cdCanvasMarkType(_cddbuffer, inStyle.mMarkStyle);
  cdCanvasMarkSize(_cddbuffer, inStyle.mMarkSize);
}

static int iPPlotMapMethod(Ihandle* ih)
{
#ifdef USE_OPENGL
  char StrData[100];
  int w, h;
  IupGetIntInt(ih, "DRAWSIZE", &w, &h);
  sprintf(StrData, "%dx%d", w, h);

  ih->data->plt->_cddbuffer = cdCreateCanvas(CD_GL, StrData);
  if (!ih->data->plt->_cddbuffer)
    return IUP_ERROR;
#else
  int old_gdi = 0;

  if (IupGetInt(ih, "USE_GDI+"))
    old_gdi = cdUseContextPlus(1);

  ih->data->plt->_cdcanvas = cdCreateCanvas(CD_IUP, ih);
  if (ih->data->plt->_cdcanvas)
  {
    /* this can fail if canvas size is zero */
    if (IupGetInt(ih, "USE_IMAGERGB"))
      ih->data->plt->_cddbuffer = cdCreateCanvas(CD_DBUFFERRGB, ih->data->plt->_cdcanvas);
    else
      ih->data->plt->_cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->plt->_cdcanvas);

    if (IupGetInt(ih, "USE_GDI+"))
      cdUseContextPlus(old_gdi);
  }

  if (!ih->data->plt->_cdcanvas)
    return IUP_ERROR;
#endif

  ih->data->plt->_redraw = 1;

  return IUP_NOERROR;
}

static void iPPlotUnMapMethod(Ihandle* ih)
{
  if (ih->data->plt->_cddbuffer != NULL)
  {
    cdKillCanvas(ih->data->plt->_cddbuffer);
    ih->data->plt->_cddbuffer = NULL;
  }

#ifndef USE_OPENGL
  if (ih->data->plt->_cdcanvas != NULL)
  {
    cdKillCanvas(ih->data->plt->_cdcanvas);
    ih->data->plt->_cdcanvas = NULL;
  }
#endif
}

static void iPPlotDestroyMethod(Ihandle* ih)
{
  delete ih->data->plt;
}

static int iPPlotCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  /* free the data alocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* Initializing object with no cd canvases */
  ih->data->plt = new PPainterIup(ih);

  /* IupCanvas callbacks */
  IupSetCallback(ih, "ACTION",      (Icallback)iPPlotRedraw_CB);
  IupSetCallback(ih, "RESIZE_CB",   (Icallback)iPPlotResize_CB);
  IupSetCallback(ih, "BUTTON_CB",   (Icallback)iPPlotMouseButton_CB);
  IupSetCallback(ih, "MOTION_CB",   (Icallback)iPPlotMouseMove_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iPPlotKeyPress_CB);

#ifdef USE_OPENGL
  IupSetAttribute(ih, "BUFFER", "DOUBLE");
#endif

  return IUP_NOERROR;
}

static Iclass* iPPlotNewClass(void)
{
#ifdef USE_OPENGL
  Iclass* ic = iupClassNew(iupRegisterFindClass("glcanvas"));
#else
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));
#endif

  ic->name = "pplot";
  ic->format = NULL;  /* none */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iPPlotNewClass;
  ic->Create  = iPPlotCreateMethod;
  ic->Destroy = iPPlotDestroyMethod;
  ic->Map     = iPPlotMapMethod;
  ic->UnMap   = iPPlotUnMapMethod;

  /* IupPPlot Callbacks */
  iupClassRegisterCallback(ic, "POSTDRAW_CB", "v");
  iupClassRegisterCallback(ic, "PREDRAW_CB", "v");
  iupClassRegisterCallback(ic, "DELETE_CB", "iiff");
  iupClassRegisterCallback(ic, "DELETEBEGIN_CB", "");
  iupClassRegisterCallback(ic, "DELETEEND_CB", "");
  iupClassRegisterCallback(ic, "SELECT_CB", "iiffi");
  iupClassRegisterCallback(ic, "SELECTBEGIN_CB", "");
  iupClassRegisterCallback(ic, "SELECTEND_CB", "");
  iupClassRegisterCallback(ic, "EDIT_CB", "iiffvv");
  iupClassRegisterCallback(ic, "EDITBEGIN_CB", "");
  iupClassRegisterCallback(ic, "EDITEND_CB", "");

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", iPPlotGetBGColorAttrib, iPPlotSetBGColorAttrib, IUPAF_SAMEASSYSTEM, "255 255 255", IUPAF_NOT_MAPPED);   /* overwrite canvas implementation, set a system default to force a new default */
  iupClassRegisterAttribute(ic, "FGCOLOR", iPPlotGetFGColorAttrib, iPPlotSetFGColorAttrib, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NOT_MAPPED);

  /* IupPPlot only */

  iupClassRegisterAttribute(ic, "REDRAW", NULL, iPPlotSetRedrawAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLE", iPPlotGetTitleAttrib, iPPlotSetTitleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEFONTSIZE", iPPlotGetTitleFontSizeAttrib, iPPlotSetTitleFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEFONTSTYLE", NULL, iPPlotSetTitleFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDSHOW", iPPlotGetLegendShowAttrib, iPPlotSetLegendShowAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDPOS", iPPlotGetLegendPosAttrib, iPPlotSetLegendPosAttrib, IUPAF_SAMEASSYSTEM, "TOPRIGHT", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDFONTSIZE", NULL, iPPlotSetLegendFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDFONTSTYLE", NULL, iPPlotSetLegendFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARGINLEFT", iPPlotGetMarginLeftAttrib, iPPlotSetMarginLeftAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARGINRIGHT", iPPlotGetMarginRightAttrib, iPPlotSetMarginRightAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARGINTOP", iPPlotGetMarginTopAttrib, iPPlotSetMarginTopAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARGINBOTTOM", iPPlotGetMarginBottomAttrib, iPPlotSetMarginBottomAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRIDLINESTYLE", iPPlotGetGridLineStyleAttrib, iPPlotSetGridLineStyleAttrib, IUPAF_SAMEASSYSTEM, "CONTINUOUS", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRIDCOLOR", iPPlotGetGridColorAttrib, iPPlotSetGridColorAttrib, IUPAF_SAMEASSYSTEM, "200 200 200", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRID", iPPlotGetGridAttrib, iPPlotSetGridAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DS_LINESTYLE", iPPlotGetDSLineStyleAttrib, iPPlotSetDSLineStyleAttrib, IUPAF_SAMEASSYSTEM, "CONTINUOUS", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_LINEWIDTH", iPPlotGetDSLineWidthAttrib, iPPlotSetDSLineWidthAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MARKSTYLE", iPPlotGetDSMarkStyleAttrib, iPPlotSetDSMarkStyleAttrib, IUPAF_SAMEASSYSTEM, "X", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MARKSIZE", iPPlotGetDSMarkSizeAttrib, iPPlotSetDSMarkSizeAttrib, IUPAF_SAMEASSYSTEM, "7", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_LEGEND", iPPlotGetDSLegendAttrib, iPPlotSetDSLegendAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_COLOR", iPPlotGetDSColorAttrib, iPPlotSetDSColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_SHOWVALUES", iPPlotGetDSShowValuesAttrib, iPPlotSetDSShowValuesAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MODE", iPPlotGetDSModeAttrib, iPPlotSetDSModeAttrib, IUPAF_SAMEASSYSTEM, "LINE", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_EDIT", iPPlotGetDSEditAttrib, iPPlotSetDSEditAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_REMOVE", NULL, iPPlotSetDSRemoveAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_COUNT", iPPlotGetDSCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "AXS_XLABEL", iPPlotGetAxisXLabelAttrib, iPPlotSetAxisXLabelAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YLABEL", iPPlotGetAxisYLabelAttrib, iPPlotSetAxisYLabelAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XLABELCENTERED", iPPlotGetAxisXLabelCenteredAttrib, iPPlotSetAxisXLabelCenteredAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YLABELCENTERED", iPPlotGetAxisYLabelCenteredAttrib, iPPlotSetAxisYLabelCenteredAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XCOLOR", iPPlotGetAxisXColorAttrib, iPPlotSetAxisXColorAttrib, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YCOLOR", iPPlotGetAxisYColorAttrib, iPPlotSetAxisYColorAttrib, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOMIN", iPPlotGetAxisXAutoMinAttrib, iPPlotSetAxisXAutoMinAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOMIN", iPPlotGetAxisYAutoMinAttrib, iPPlotSetAxisYAutoMinAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOMAX", iPPlotGetAxisXAutoMaxAttrib, iPPlotSetAxisXAutoMaxAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOMAX", iPPlotGetAxisYAutoMaxAttrib, iPPlotSetAxisYAutoMaxAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XMIN", iPPlotGetAxisXMinAttrib, iPPlotSetAxisXMinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YMIN", iPPlotGetAxisYMinAttrib, iPPlotSetAxisYMinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XMAX", iPPlotGetAxisXMaxAttrib, iPPlotSetAxisXMaxAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YMAX", iPPlotGetAxisYMaxAttrib, iPPlotSetAxisYMaxAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XREVERSE", iPPlotGetAxisXReverseAttrib, iPPlotSetAxisXReverseAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YREVERSE", iPPlotGetAxisYReverseAttrib, iPPlotSetAxisYReverseAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XCROSSORIGIN", iPPlotGetAxisXCrossOriginAttrib, iPPlotSetAxisXCrossOriginAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YCROSSORIGIN", iPPlotGetAxisYCrossOriginAttrib, iPPlotSetAxisYCrossOriginAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XSCALE", iPPlotGetAxisXScaleAttrib, iPPlotSetAxisXScaleAttrib, IUPAF_SAMEASSYSTEM, "LIN", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YSCALE", iPPlotGetAxisYScaleAttrib, iPPlotSetAxisYScaleAttrib, IUPAF_SAMEASSYSTEM, "LIN", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XFONTSIZE", iPPlotGetAxisXFontSizeAttrib, iPPlotSetAxisXFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YFONTSIZE", iPPlotGetAxisYFontSizeAttrib, iPPlotSetAxisYFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XFONTSTYLE", iPPlotGetAxisXFontStyleAttrib, iPPlotSetAxisXFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YFONTSTYLE", iPPlotGetAxisYFontStyleAttrib, iPPlotSetAxisYFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICK", iPPlotGetAxisXTickAttrib, iPPlotSetAxisXTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICK", iPPlotGetAxisYTickAttrib, iPPlotSetAxisYTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKSIZE", iPPlotGetAxisXTickSizeAttrib, iPPlotSetAxisXTickSizeAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKSIZE", iPPlotGetAxisYTickSizeAttrib, iPPlotSetAxisYTickSizeAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFORMAT", iPPlotGetAxisXTickFormatAttrib, iPPlotSetAxisXTickFormatAttrib, IUPAF_SAMEASSYSTEM, "%.0f", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFORMAT", iPPlotGetAxisYTickFormatAttrib, iPPlotSetAxisYTickFormatAttrib, IUPAF_SAMEASSYSTEM, "%.0f", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFONTSIZE", iPPlotGetAxisXTickFontSizeAttrib, iPPlotSetAxisXTickFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFONTSIZE", iPPlotGetAxisYTickFontSizeAttrib, iPPlotSetAxisYTickFontSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFONTSTYLE", iPPlotGetAxisXTickFontStyleAttrib, iPPlotSetAxisXTickFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFONTSTYLE", iPPlotGetAxisYTickFontStyleAttrib, iPPlotSetAxisYTickFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOTICK", iPPlotGetAxisXAutoTickAttrib, iPPlotSetAxisXAutoTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOTICK", iPPlotGetAxisYAutoTickAttrib, iPPlotSetAxisYAutoTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOTICKSIZE", iPPlotGetAxisXAutoTickSizeAttrib, iPPlotSetAxisXAutoTickSizeAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOTICKSIZE", iPPlotGetAxisYAutoTickSizeAttrib, iPPlotSetAxisYAutoTickSizeAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKMAJORSPAN", iPPlotGetAxisXTickMajorSpanAttrib, iPPlotSetAxisXTickMajorSpanAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKMAJORSPAN", iPPlotGetAxisYTickMajorSpanAttrib, iPPlotSetAxisYTickMajorSpanAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKDIVISION", iPPlotGetAxisXTickDivisionAttrib, iPPlotSetAxisXTickDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKDIVISION", iPPlotGetAxisYTickDivisionAttrib, iPPlotSetAxisYTickDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOTICKSIZE", iPPlotGetAxisXAutoTickSizeAttrib, iPPlotSetAxisXAutoTickSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOTICKSIZE", iPPlotGetAxisYAutoTickSizeAttrib, iPPlotSetAxisYAutoTickSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKMAJORSIZE", iPPlotGetAxisXTickMajorSizeAttrib, iPPlotSetAxisXTickMajorSizeAttrib, IUPAF_SAMEASSYSTEM, "8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKMAJORSIZE", iPPlotGetAxisYTickMajorSizeAttrib, iPPlotSetAxisYTickMajorSizeAttrib, IUPAF_SAMEASSYSTEM, "8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "REMOVE", NULL, iPPlotSetRemoveAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLEAR", NULL, iPPlotSetClearAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", iPPlotGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CURRENT", iPPlotGetCurrentAttrib, iPPlotSetCurrentAttrib, IUPAF_SAMEASSYSTEM, "-1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}

/* user level call: create control */
Ihandle* IupPPlot(void)
{
  return IupCreate("pplot");
}

void IupPPlotOpen(void)
{
#ifdef USE_OPENGL
  IupGLCanvasOpen();
#endif

  if (!IupGetGlobal("_IUP_PPLOT_OPEN"))
  {
    iupRegisterClass(iPPlotNewClass());
    IupSetGlobal("_IUP_PPLOT_OPEN", "1");
  }
}
