/** \file
 * \brief IupTuioClient control
 *
 * See Copyright Notice in "iup.h"
 */

#include "TuioListener.h"
#include "TuioClient.h"

#include "iup.h"
#include "iuptuio.h"
#include "iupcbs.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup_object.h"
#include "iup_assert.h"
#include "iup_str.h"
#include "iup_register.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"


using namespace TUIO;

struct iTuioCursorEnvent{
  int id;
  float x, y;
  char state;

  iTuioCursorEnvent(int _id, float _x, float _y, char _state);
};

iTuioCursorEnvent::iTuioCursorEnvent(int _id, float _x, float _y, char _state)
:id(_id), x(_x), y(_y), state(_state)
{
}

class IupTuioListener : public TuioListener 
{
  int changed, locked;
  TuioClient* client;
  Ihandle* ih;
  std::list<iTuioCursorEnvent> cursor_events;

  void processCursor(TuioCursor *tcur, const char* state, const char* action);
  void initCursorInfo(int cursor_count, int* pid, int* pstate);
  void finishCursorInfo(int cursor_count, int* px, int* py, int* pstate, int w, int h, int use_client_coord, Ihandle* ih_canvas);
  void updateCursorInfo(int *cursor_count, int* pid, int* px, int* py, int* pstate, int id, int x, int y, int state);
  int GetMainCursor();

  static int timer_action_cb(Ihandle *timer);

  public:
    int debug;
    Ihandle* timer;

    IupTuioListener(Ihandle* _ih, TuioClient* _client);

    void addTuioObject(TuioObject *tobj);
    void updateTuioObject(TuioObject *tobj);
    void removeTuioObject(TuioObject *tobj);

    void addTuioCursor(TuioCursor *tcur);
    void updateTuioCursor(TuioCursor *tcur);
    void removeTuioCursor(TuioCursor *tcur);

    void refresh(TuioTime frameTime);
};

IupTuioListener::IupTuioListener(Ihandle* _ih, TuioClient* _client)
  :changed(0), locked(0), client(_client), ih(_ih), debug(0)
{
  timer = IupTimer();
  IupSetAttribute(timer, "TIME", "50");
  IupSetAttribute(timer, "_IUP_TUIOLISTENER", (char*)this);
  IupSetCallback(timer, "ACTION_CB", timer_action_cb);
}

void IupTuioListener::addTuioObject(TuioObject *tobj) 
{
  /* unused */
  (void)tobj;
}

void IupTuioListener::updateTuioObject(TuioObject *tobj) 
{
  /* unused */
  (void)tobj;
}

void IupTuioListener::removeTuioObject(TuioObject *tobj) 
{
  /* unused */
  (void)tobj;
}

void IupTuioListener::addTuioCursor(TuioCursor *tcur) 
{
  IupTuioListener::processCursor(tcur, "DOWN", "AddCursor"); 
}

void IupTuioListener::updateTuioCursor(TuioCursor *tcur) 
{
  IupTuioListener::processCursor(tcur, "MOVE", "UpdateCursor"); 
}

void IupTuioListener::removeTuioCursor(TuioCursor *tcur) 
{
  IupTuioListener::processCursor(tcur, "UP", "RemoveCursor"); 
}

void IupTuioListener::processCursor(TuioCursor *tcur, const char* state, const char* action) 
{
  float x = tcur->getX();
  float y = tcur->getY();
  int id = (int)tcur->getSessionID();
  cursor_events.push_back(iTuioCursorEnvent(id, x, y, state[0]));

  this->changed = 1;

  if (this->debug)
    printf("IupTuioClient-%s(id=%d sid=%d x=%d y=%d)\n", action, tcur->getCursorID(), (int)tcur->getSessionID(), (int)tcur->getX(), (int)tcur->getY());
}

void  IupTuioListener::refresh(TuioTime frameTime) 
{
  if (this->changed)
  {
    Ihandle* ih_canvas = IupGetAttributeHandle(this->ih, "TARGETCANVAS");
    if (!ih_canvas)
      ih_canvas = this->ih;

    this->changed = 0;

    IFniiis cb = (IFniiis)IupGetCallback(ih_canvas, "TOUCH_CB");
    IFniIIII mcb = (IFniIIII)IupGetCallback(ih_canvas, "MULTITOUCH_CB");
    if (cb || mcb)
    {
      this->client->lockCursorList();
      this->locked = 1;
    }

    if (this->debug)
      printf("IupTuioClient-RefreshChanged(time=%d)\n", (int)frameTime.getTotalMilliseconds());
  }
}

void IupTuioListener::initCursorInfo(int cursor_count, int* pid, int* pstate)
{
  std::list<TuioCursor*>& cursorList = this->client->getCursorList();
  std::list <TuioCursor*>::iterator iter;
  int i;

  for (i = 0, iter = cursorList.begin() ; i<cursor_count; iter++, i++) 
  {
    TuioCursor* tcur = (*iter);
    pid[i] = (int)tcur->getSessionID();
    pstate[i] = 0;  /* mark to be updated later */
  }
}

int IupTuioListener::GetMainCursor()
{
  std::list<TuioCursor*>& cursorList = this->client->getCursorList();
  std::list <TuioCursor*>::iterator iter;
  std::list <TuioCursor*>::iterator end = cursorList.end();
  int min_id = -1;

  for (iter = cursorList.begin(); iter!=end; iter++) 
  {
    TuioCursor* tcur = (*iter);
    int id = (int)tcur->getSessionID();
    if (min_id == -1 || id < min_id)
      min_id = id;
  }

  return min_id;
}

void IupTuioListener::finishCursorInfo(int cursor_count, int* px, int* py, int* pstate, int w, int h, int use_client_coord, Ihandle* ih_canvas)
{
  std::list<TuioCursor*>& cursorList = this->client->getCursorList();
  std::list <TuioCursor*>::iterator iter;
  std::list <TuioCursor*>::iterator end = cursorList.end();
  int i;

  for (i = 0, iter = cursorList.begin(); i<cursor_count && iter!=end; iter++, i++) 
  {
    if (pstate[i] == 0) /* if still 0, then it was not updated, must fill it here */
    {
      TuioCursor* tcur = (*iter);

      int x = tcur->getScreenX(w);
      int y = tcur->getScreenY(h);

      if (use_client_coord)
        iupdrvScreenToClient(ih_canvas, &x, &y);

      px[i] = x;
      py[i] = y;
      pstate[i] = 'M';  /* mark as MOVE */
    }
  }
}

void IupTuioListener::updateCursorInfo(int *cursor_count, int* pid, int* px, int* py, int* pstate, int id, int x, int y, int state)
{
  int i;

  for(i = 0; i < *cursor_count; i++)
  {
    if (pid[i] == id)
    {
      px[i] = x;
      py[i] = y;
      pstate[i] = state;
      return;
    }
  }

  if (state == 'U')  /* UP - not in the cursorList, add it */
  {
    pid[i] = id;
    px[i] = x;
    py[i] = y;
    pstate[i] = state;
    (*cursor_count)++;
  }
}

int IupTuioListener::timer_action_cb(Ihandle *timer)
{
  IupTuioListener* listener = reinterpret_cast<IupTuioListener*>(IupGetAttribute(timer, "_IUP_TUIOLISTENER"));

  if (!listener->locked)
    return IUP_DEFAULT;

  int events_count = (int)listener->cursor_events.size();
  int cursor_count = listener->client->CursorListCount();
  int total_count = events_count+cursor_count;
  if (!total_count)
  {
    listener->locked = 0;
    listener->client->unlockCursorList();
    return IUP_DEFAULT;
  }

  int use_client_coord = 0;
  Ihandle* ih_canvas = IupGetAttributeHandle(listener->ih, "TARGETCANVAS");
  if (ih_canvas)
    use_client_coord = 1;
  else
    ih_canvas = listener->ih;

  IFniiis cb = (IFniiis)IupGetCallback(ih_canvas, "TOUCH_CB");
  IFniIIII mcb = (IFniIIII)IupGetCallback(ih_canvas, "MULTITOUCH_CB");

  int w, h, x, y;
  iupdrvGetFullSize(&w, &h);

  int *px=NULL, *py=NULL, *pid=NULL, *pstate=NULL;
  if (mcb)
  {
    px = new int[total_count];
    py = new int[total_count];
    pid = new int[total_count];
    pstate = new int[total_count];

    listener->initCursorInfo(cursor_count, pid, pstate);
  }

  int min_id = -1;
  if (cb)
    min_id = listener->GetMainCursor();

  for (int i = 0; i<events_count; i++) 
  {
    const iTuioCursorEnvent& evt = listener->cursor_events.front();

    const char* state = (evt.state=='D')? "DOWN": ((evt.state=='U')? "UP": "MOVE");
    x = (int)floor(evt.x*w+0.5f);
    y = (int)floor(evt.y*h+0.5f);

    if (use_client_coord)
      iupdrvScreenToClient(ih_canvas, &x, &y);

    if (cb)
    {
      if (min_id == evt.id)
        state = (evt.state=='D')? "DOWN-PRIMARY": ((evt.state=='U')? "UP-PRIMARY": "MOVE-PRIMARY");

      if (cb(ih_canvas, evt.id, x, y, (char*)state)==IUP_CLOSE)
        IupExitLoop();
    }

    if (mcb)
      listener->updateCursorInfo(&cursor_count, pid, px, py, pstate, evt.id, x, y, state[0]);

    listener->cursor_events.pop_front();
  }

  if (mcb)
  {
    listener->finishCursorInfo(cursor_count, px, py, pstate, w, h, use_client_coord, ih_canvas);

    if (mcb(ih_canvas, cursor_count, pid, px, py, pstate)==IUP_CLOSE)
      IupExitLoop();
     
    delete[] px;
    delete[] py;
    delete[] pid;
    delete[] pstate;
  }

  listener->locked = 0;
  listener->client->unlockCursorList();
  return IUP_DEFAULT;
}

/**************************************************************************************/

struct _IcontrolData 
{
  IupTuioListener* listener;
  TuioClient* client;
};

static int iTuioSetConnectAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->client->isConnected())
  {
    IupSetAttribute(ih->data->listener->timer, "RUN", "NO");
    ih->data->client->disconnect();
  }

  if (iupStrEqualNoCase(value, "YES"))
  {
    ih->data->client->connect(false);
    IupSetAttribute(ih->data->listener->timer, "RUN", "YES");
  }
  else if (iupStrEqualNoCase(value, "LOCKED"))
  {
    ih->data->client->connect(true);
    IupSetAttribute(ih->data->listener->timer, "RUN", "YES");
  }
    
  return 0;
}

static char* iTuioGetConnectAttrib(Ihandle *ih)
{
  if (ih->data->client->isConnected())
    return (char*)"Yes";
  else
    return (char*)"No";
}

static int iTuioSetDebugAttrib(Ihandle* ih, const char* value)
{
  ih->data->listener->debug = iupStrBoolean(value);
  return 0;
}

static char* iTuioGetDebugAttrib(Ihandle *ih)
{
  if (ih->data->listener->debug)
    return (char*)"Yes";
  else
    return (char*)"No";
}

static int iTuioCreateMethod(Ihandle* ih, void** params)
{
  int port = 3333;
  if (params && params[0])
#if defined (WIN32) && defined (_M_X64)
    port = (int)(long long)(params[0]);
#else
    port = (int)(long)(params[0]); /* must cast to long first to avoid 64bit C++ compiler error */
#endif
  ih->data = iupALLOCCTRLDATA();
  
  ih->data->client = new TuioClient(port);
  ih->data->listener = new IupTuioListener(ih, ih->data->client);
  ih->data->client->addTuioListener(ih->data->listener);

  return IUP_NOERROR;
}

static void iTuioDestroyMethod(Ihandle* ih)
{
  IupDestroy(ih->data->listener->timer);
  delete ih->data->client;
  delete ih->data->listener;
}

Ihandle* IupTuioClient(int port)
{
  void *params[2];
  params[0] = (void*)port;
  params[1] = NULL;
  return IupCreatev("tuioclient", params);
}

static Iclass* iTuioNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = (char*)"tuioclient";
  ic->format = (char*)"i";  /* (int) */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;
  
  ic->New = iTuioNewClass;
  ic->Create = iTuioCreateMethod;
  ic->Destroy = iTuioDestroyMethod;

  iupClassRegisterCallback(ic, "TOUCH_CB", "iiis");
  iupClassRegisterCallback(ic, "MULTITOUCH_CB", "iIIII");

  iupClassRegisterAttribute(ic, "CONNECT", iTuioGetConnectAttrib, iTuioSetConnectAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DEBUG", iTuioGetDebugAttrib, iTuioSetDebugAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}

int IupTuioOpen(void)
{
  if (IupGetGlobal("_IUP_TUIO_OPEN"))
    return IUP_OPENED;

  iupRegisterClass(iTuioNewClass());

  IupSetGlobal("_IUP_TUIO_OPEN", "1");
  return IUP_NOERROR;
}
