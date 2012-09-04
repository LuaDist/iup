/** \file
 * \brief Motif Driver keyboard mapping
 *
 * See Copyright Notice in "iup.h"
 */

#include <Xm/Xm.h>
#include <X11/keysym.h>

#include <stdlib.h>      
#include <stdio.h>      
#include <ctype.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_key.h"
#include "iup_str.h"

#include "iup_drv.h"
#include "iupmot_drv.h"


typedef struct Imot2iupkey
{
  KeySym motcode;
  int iupcode;
  int s_iupcode;
  int c_iupcode;
  int m_iupcode;
  int y_iupcode;
} Imot2iupkey;

static Imot2iupkey motkey_map[] = {

{ XK_Escape,    K_ESC,   K_sESC,    K_cESC,   K_mESC   ,K_yESC   },
{ XK_Pause,     K_PAUSE, K_sPAUSE,  K_cPAUSE, K_mPAUSE ,K_yPAUSE },
{ XK_Print,     K_Print, K_sPrint,  K_cPrint, K_mPrint ,K_yPrint },
{ XK_Menu,      K_Menu,  K_sMenu,   K_cMenu,  K_mMenu  ,K_yMenu  },
                                                       
{ XK_Home,      K_HOME,  K_sHOME,   K_cHOME,  K_mHOME  ,K_yHOME  },
{ XK_Up,        K_UP,    K_sUP,     K_cUP,    K_mUP    ,K_yUP    },
{ XK_Prior,     K_PGUP,  K_sPGUP,   K_cPGUP,  K_mPGUP  ,K_yPGUP  },
{ XK_Left,      K_LEFT,  K_sLEFT,   K_cLEFT,  K_mLEFT  ,K_yLEFT  },
{ XK_Begin,     K_MIDDLE,K_sMIDDLE, K_cMIDDLE,K_mMIDDLE,K_yMIDDLE},
{ XK_Right,     K_RIGHT, K_sRIGHT,  K_cRIGHT, K_mRIGHT ,K_yRIGHT },
{ XK_End,       K_END,   K_sEND,    K_cEND,   K_mEND   ,K_yEND   },
{ XK_Down,      K_DOWN,  K_sDOWN,   K_cDOWN,  K_mDOWN  ,K_yDOWN  },
{ XK_Next,      K_PGDN,  K_sPGDN,   K_cPGDN,  K_mPGDN  ,K_yPGDN  },
{ XK_Insert,    K_INS,   K_sINS,    K_cINS,   K_mINS   ,K_yINS   },
{ XK_Delete,    K_DEL,   K_sDEL,    K_cDEL,   K_mDEL   ,K_yDEL   },
{ XK_space,     K_SP,    K_sSP,     K_cSP,    K_mSP    ,K_ySP    },
{ XK_Tab,       K_TAB,   K_sTAB,    K_cTAB,   K_mTAB   ,K_yTAB   },
{ XK_Return,    K_CR,    K_sCR,     K_cCR,    K_mCR    ,K_yCR    },
{ XK_BackSpace, K_BS,    K_sBS,     K_cBS,    K_mBS    ,K_yBS    },

{ XK_1,   K_1, K_exclam,      K_c1, K_m1, K_y1 },
{ XK_2,   K_2, K_at,          K_c2, K_m2, K_y2 },
{ XK_3,   K_3, K_numbersign,  K_c3, K_m3, K_y3 },
{ XK_4,   K_4, K_dollar,      K_c4, K_m4, K_y4 },
{ XK_5,   K_5, K_percent,     K_c5, K_m5, K_y5 },
{ XK_6,   K_6, K_circum,      K_c6, K_m6, K_y6 },
{ XK_7,   K_7, K_ampersand,   K_c7, K_m7, K_y7 },
{ XK_8,   K_8, K_asterisk,    K_c8, K_m8, K_y8 },
{ XK_9,   K_9, K_parentleft,  K_c9, K_m9, K_y9 },
{ XK_0,   K_0, K_parentright, K_c0, K_m0, K_y0 },

{ XK_a,   K_a, K_A, K_cA, K_mA, K_yA },
{ XK_b,   K_b, K_B, K_cB, K_mB, K_yB },
{ XK_c,   K_c, K_C, K_cC, K_mC, K_yC },
{ XK_d,   K_d, K_D, K_cD, K_mD, K_yD },
{ XK_e,   K_e, K_E, K_cE, K_mE, K_yE },
{ XK_f,   K_f, K_F, K_cF, K_mF, K_yF },
{ XK_g,   K_g, K_G, K_cG, K_mG, K_yG },
{ XK_h,   K_h, K_H, K_cH, K_mH, K_yH },
{ XK_i,   K_i, K_I, K_cI, K_mI, K_yI },
{ XK_j,   K_j, K_J, K_cJ, K_mJ, K_yJ },
{ XK_k,   K_k, K_K, K_cK, K_mK, K_yK },
{ XK_l,   K_l, K_L, K_cL, K_mL, K_yL },
{ XK_m,   K_m, K_M, K_cM, K_mM, K_yM },
{ XK_n,   K_n, K_N, K_cN, K_mN, K_yN },
{ XK_o,   K_o, K_O, K_cO, K_mO, K_yO },
{ XK_p,   K_p, K_P, K_cP, K_mP, K_yP },
{ XK_q,   K_q, K_Q, K_cQ, K_mQ, K_yQ },
{ XK_r,   K_r, K_R, K_cR, K_mR, K_yR },
{ XK_s,   K_s, K_S, K_cS, K_mS, K_yS },
{ XK_t,   K_t, K_T, K_cT, K_mT, K_yT },
{ XK_u,   K_u, K_U, K_cU, K_mU, K_yU },
{ XK_v,   K_v, K_V, K_cV, K_mV, K_yV },
{ XK_w,   K_w, K_W, K_cW, K_mW, K_yW },
{ XK_x,   K_x, K_X, K_cX, K_mX, K_yX },
{ XK_y,   K_y, K_Y, K_cY, K_mY, K_yY },
{ XK_z,   K_z, K_Z, K_cZ, K_mZ, K_yZ },

{ XK_F1,  K_F1,  K_sF1,  K_cF1,  K_mF1,  K_yF1 },
{ XK_F2,  K_F2,  K_sF2,  K_cF2,  K_mF2,  K_yF2 },
{ XK_F3,  K_F3,  K_sF3,  K_cF3,  K_mF3,  K_yF3 },
{ XK_F4,  K_F4,  K_sF4,  K_cF4,  K_mF4,  K_yF4 },
{ XK_F5,  K_F5,  K_sF5,  K_cF5,  K_mF5,  K_yF5 },
{ XK_F6,  K_F6,  K_sF6,  K_cF6,  K_mF6,  K_yF6 },
{ XK_F7,  K_F7,  K_sF7,  K_cF7,  K_mF7,  K_yF7 },
{ XK_F8,  K_F8,  K_sF8,  K_cF8,  K_mF8,  K_yF8 },
{ XK_F9,  K_F9,  K_sF9,  K_cF9,  K_mF9,  K_yF9 },
{ XK_F10, K_F10, K_sF10, K_cF10, K_mF10, K_yF10 },
{ XK_F11, K_F11, K_sF11, K_cF11, K_mF11, K_yF11 },
{ XK_F12, K_F12, K_sF12, K_cF12, K_mF12, K_yF12 },

{ XK_semicolon,   K_semicolon,    K_colon,      K_cSemicolon, K_mSemicolon, K_ySemicolon },
{ XK_equal,       K_equal,        K_plus,       K_cEqual,     K_mEqual,  K_yEqual },
{ XK_comma,       K_comma,        K_less,       K_cComma,     K_mComma,  K_yComma },
{ XK_minus,       K_minus,        K_underscore, K_cMinus,     K_mMinus,  K_yMinus },
{ XK_period,      K_period,       K_greater,    K_cPeriod,    K_mPeriod, K_yPeriod },
{ XK_slash,       K_slash,        K_question,   K_cSlash,     K_mSlash,  K_ySlash },
{ XK_grave,       K_grave,        K_tilde,      0, 0, 0 },
{ XK_bracketleft, K_bracketleft,  K_braceleft,  K_cBracketleft, K_mBracketleft, K_yBracketleft },
{ XK_backslash,   K_backslash,    K_bar,        K_cBackslash,   K_mBackslash,   K_yBackslash },
{ XK_bracketright,K_bracketright, K_braceright, K_cBracketright,K_mBracketright,K_yBracketright },
{ XK_apostrophe,  K_apostrophe,   K_quotedbl,   0, 0, 0 },

{ XK_KP_0,   K_0, K_0,  K_c0, K_m0, K_y0 },
{ XK_KP_1,   K_1, K_1,  K_c1, K_m1, K_y1 },
{ XK_KP_2,   K_2, K_2,  K_c2, K_m2, K_y2 },
{ XK_KP_3,   K_3, K_3,  K_c3, K_m3, K_y3 },
{ XK_KP_4,   K_4, K_4,  K_c4, K_m4, K_y4 },
{ XK_KP_5,   K_5, K_5,  K_c5, K_m5, K_y5 },
{ XK_KP_6,   K_6, K_6,  K_c6, K_m6, K_y6 },
{ XK_KP_7,   K_7, K_7,  K_c7, K_m7, K_y7 },
{ XK_KP_8,   K_8, K_8,  K_c8, K_m8, K_y8 },
{ XK_KP_9,   K_9, K_9,  K_c9, K_m9, K_y9 },
{ XK_KP_Multiply,  K_asterisk, K_sAsterisk, K_cAsterisk, K_mAsterisk, K_yAsterisk },
{ XK_KP_Add,       K_plus,     K_sPlus,     K_cPlus,     K_mPlus,     K_yPlus },
{ XK_KP_Subtract,  K_minus,    K_sMinus,    K_cMinus,    K_mMinus,    K_yMinus },
{ XK_KP_Decimal,   K_period,   K_sPeriod,   K_cPeriod,   K_mPeriod,   K_yPeriod },
{ XK_KP_Divide,    K_slash,    K_sSlash,    K_cSlash,    K_mSlash,    K_ySlash },
{ XK_KP_Separator, K_comma,    K_sComma,    K_cComma,    K_mComma,    K_yComma },

{ XK_ccedilla,     K_ccedilla, K_Ccedilla, K_cCcedilla, K_mCcedilla, K_yCcedilla },

{ XK_dead_tilde,   K_tilde, K_circum, 0, 0, 0 },
{ XK_dead_acute,   K_acute, K_grave,  0, 0, 0 },
{ XK_dead_grave,   K_grave, K_tilde,  0, 0, 0 },
{ XK_dead_circumflex, K_tilde, K_circum, 0, 0, 0 },

{ XK_KP_F1,  K_F1,  K_sF1,  K_cF1,  K_mF1,  K_yF1 },
{ XK_KP_F2,  K_F2,  K_sF2,  K_cF2,  K_mF2,  K_yF2 },
{ XK_KP_F3,  K_F3,  K_sF3,  K_cF3,  K_mF3,  K_yF3 },
{ XK_KP_F4,  K_F4,  K_sF4,  K_cF4,  K_mF4,  K_yF4 },
{ XK_KP_Space,     K_SP,    K_sSP,     K_cSP,    K_mSP    ,K_ySP    },
{ XK_KP_Tab,       K_TAB,   K_sTAB,    K_cTAB,   K_mTAB   ,K_yTAB   },
{ XK_KP_Equal,     K_equal,  0,       K_cEqual,     K_mEqual,  K_yEqual },

{ XK_KP_Enter,     K_CR,    K_sCR,     K_cCR,    K_mCR,    K_yCR    },
{ XK_KP_Home,      K_HOME,  K_sHOME,   K_cHOME,  K_mHOME,  K_yHOME  },
{ XK_KP_Up,        K_UP,    K_sUP,     K_cUP,    K_mUP,    K_yUP    },
{ XK_KP_Page_Up,   K_PGUP,  K_sPGUP,   K_cPGUP,  K_mPGUP,  K_yPGUP  },
{ XK_KP_Left,      K_LEFT,  K_sLEFT,   K_cLEFT,  K_mLEFT,  K_yLEFT  },
{ XK_KP_Begin,     K_MIDDLE,K_sMIDDLE, K_cMIDDLE,K_mMIDDLE,K_yMIDDLE},
{ XK_KP_Right,     K_RIGHT, K_sRIGHT,  K_cRIGHT, K_mRIGHT, K_yRIGHT },
{ XK_KP_End,       K_END,   K_sEND,    K_cEND,   K_mEND,   K_yEND   },
{ XK_KP_Down,      K_DOWN,  K_sDOWN,   K_cDOWN,  K_mDOWN,  K_yDOWN  },
{ XK_KP_Page_Down, K_PGDN,  K_sPGDN,   K_cPGDN,  K_mPGDN,  K_yPGDN  },
{ XK_KP_Insert,    K_INS,   K_sINS,    K_cINS,   K_mINS,   K_yINS   },
{ XK_KP_Delete,    K_DEL,   K_sDEL,    K_cDEL,   K_mDEL,   K_yDEL   }

};

void iupdrvKeyEncode(int key, unsigned int *keyval, unsigned int *state)
{
  int i, iupcode = key & 0xFF; /* 0-255 interval */
  int count = sizeof(motkey_map)/sizeof(motkey_map[0]);
  for (i = 0; i < count; i++)
  {
    Imot2iupkey* key_map = &(motkey_map[i]);
    if (key_map->iupcode == iupcode)
    {
      *keyval = XKeysymToKeycode(iupmot_display, key_map->motcode);
      *state = 0;

      if (iupcode != key)
      {
        if (key_map->c_iupcode == key)
          *state = ControlMask;
        else if (key_map->m_iupcode == key)
          *state = Mod1Mask;
        else if (key_map->y_iupcode == key)
          *state = Mod4Mask;
        else if (key_map->s_iupcode == key)
          *state = ShiftMask;
      }
      return;
    }
    else if (key_map->s_iupcode == key)   /* There are Shift keys below 256 */
    {
      *keyval = XKeysymToKeycode(iupmot_display, key_map->motcode);
      *state = ShiftMask;
      return;
    }
  }
}

static int motKeyMap2Iup(unsigned int state, int i)
{
  int code = 0;
  if (state & ControlMask)   /* Ctrl */
    code = motkey_map[i].c_iupcode;
  else if (state & Mod1Mask || 
           state & Mod5Mask) /* Alt */
    code = motkey_map[i].m_iupcode;
  else if (state & Mod4Mask) /* Apple/Win */
    code = motkey_map[i].y_iupcode;
  else if (state & LockMask) /* CapsLock */
  {
    if ((state & ShiftMask) || !iupKeyCanCaps(motkey_map[i].iupcode))
      return motkey_map[i].iupcode;
    else
      code = motkey_map[i].s_iupcode;
  }
  else if (state & ShiftMask) /* Shift */
    code = motkey_map[i].s_iupcode;
  else
    return motkey_map[i].iupcode;

  if (!code)
    code = motkey_map[i].iupcode;

  return code;
}

int iupmotKeyDecode(XKeyEvent *evt)
{
  int i;
  KeySym motcode = XKeycodeToKeysym(iupmot_display, evt->keycode, 0);
  int count = sizeof(motkey_map)/sizeof(motkey_map[0]);

  if ((evt->state & Mod2Mask) && /* NumLock */
      (motcode >= XK_KP_Home) &&
      (motcode <= XK_KP_Delete))
  {
    /* remap to numeric keys */
    KeySym remap_numkey[] = {XK_KP_7, XK_KP_4, XK_KP_8, XK_KP_6, XK_KP_2, XK_KP_9, XK_KP_3, XK_KP_1, XK_KP_5, XK_KP_0, XK_KP_Decimal};
    motcode = remap_numkey[motcode-XK_KP_Home];
  }

  for (i = 0; i < count; i++)
  {
    if (motkey_map[i].motcode == motcode)
      return motKeyMap2Iup(evt->state, i);
  }

  return 0;
}

KeySym iupmotKeyCharToKeySym(char c)
{
  int i;
  int count = sizeof(motkey_map)/sizeof(motkey_map[0]);

  for (i = 0; i < count; i++)
  {
    if (motkey_map[i].iupcode == c)
      return motkey_map[i].motcode;
    if (motkey_map[i].s_iupcode == c)
    {
      if (motkey_map[i].motcode >= XK_a &&
          motkey_map[i].motcode <= XK_z)
        return motkey_map[i].motcode - (XK_a-XK_A);
    }
  }

  return 0;
}

/* Discards keyrepeat by removing the keypress event from the queue.
 * The pair keyrelease/keypress is always put together in the queue,
 * by removing the keypress, we only worry about keyrelease. In case
 * of a keyrelease, we ignore it if the next event is a keypress (which
 * means repetition. Otherwise it is a real keyrelease.
 *
 * Returns 1 if the keypress is found in the queue and 0 otherwise.
 */
static int motKeyDiscardKeypressRepeat(XEvent *evt)
{
  XEvent ahead;
  if (XEventsQueued(iupmot_display, QueuedAfterReading))
  {
    XPeekEvent(iupmot_display, &ahead);
    if (ahead.type == KeyPress && ahead.xkey.window == evt->xkey.window
        && ahead.xkey.keycode == evt->xkey.keycode && ahead.xkey.time == evt->xkey.time)
    {
      /* Pop off the repeated KeyPress and ignore */
      XNextEvent(iupmot_display, evt);
      /* Ignore the auto repeated KeyRelease/KeyPress pair */
      return 1;
    }
  }
  /* No KeyPress found */
  return 0;
}

/* this is called only for canvas */
void iupmotCanvasKeyReleaseEvent(Widget w, Ihandle *ih, XEvent *evt, Boolean *cont)
{
  if (motKeyDiscardKeypressRepeat(evt))
  {
    /* call key_press because it was removed from the queue */
    iupmotKeyPressEvent(w, ih, evt, cont);
  }
  else
  {
    int result;
    int code = iupmotKeyDecode((XKeyEvent*)evt);
    if (code == 0) 
      return;
    result = iupKeyCallKeyPressCb(ih, code, 0);
    if (result == IUP_CLOSE)
    {
      IupExitLoop();
      return;
    }
    if (result == IUP_IGNORE)
    {
      *cont = False;
      return;
    }
  }
}

void iupmotKeyPressEvent(Widget w, Ihandle *ih, XEvent *evt, Boolean *cont)
{
  int result;
  int code = iupmotKeyDecode((XKeyEvent*)evt);
  if (code == 0) 
      return;

  if ((((XKeyEvent*)evt)->state & Mod1Mask || ((XKeyEvent*)evt)->state & Mod5Mask))  /* Alt */
  {
    KeySym motcode = XKeycodeToKeysym(iupmot_display, ((XKeyEvent*)evt)->keycode, 0);
    if (motcode < 128)
    {
      IFni cb;
      Ihandle* dialog = IupGetDialog(ih);
      char attrib[22] = "_IUPMOT_MNEMONIC_ _CB";
      attrib[17] = (char)toupper(motcode);
      cb = (IFni)IupGetCallback(dialog, attrib);
      if (cb) 
      {
        cb(dialog, attrib[17]);
        return;
      }
    }
  }

  result = iupKeyCallKeyCb(ih, code);
  if (result == IUP_CLOSE)
  {
    IupExitLoop();
    return;
  }
  if (result == IUP_IGNORE)
  {
    *cont = False;
    return;
  }

  /* in the previous callback the dialog could be destroyed */
  if (iupObjectCheck(ih))
  {
    /* this is called only for canvas */
    if (ih->iclass->nativetype==IUP_TYPECANVAS) 
    {
      result = iupKeyCallKeyPressCb(ih, code, 1);
      if (result == IUP_CLOSE)
      {
        IupExitLoop();
        return;
      }
      if (result == IUP_IGNORE)
      {
        *cont = False;
        return;
      }
    }

    if (!iupKeyProcessNavigation(ih, code, ((XKeyEvent*)evt)->state & ShiftMask))
    {
      *cont = False;
      return;
    }
  }

  (void)w;
}

void iupmotButtonKeySetStatus(unsigned int state, unsigned int but, char* status, int doubleclick)
{
  if (state & ShiftMask)
    iupKEY_SETSHIFT(status);

  if (state & ControlMask)
    iupKEY_SETCONTROL(status); 

  if ((state & Button1Mask) || but==Button1)
    iupKEY_SETBUTTON1(status);

  if ((state & Button2Mask) || but==Button2)
    iupKEY_SETBUTTON2(status);

  if ((state & Button3Mask) || but==Button3)
    iupKEY_SETBUTTON3(status);

  if ((state & Button4Mask) || but==Button4)
    iupKEY_SETBUTTON4(status);

  if ((state & Button5Mask) || but==Button5)
    iupKEY_SETBUTTON5(status);

  if (state & Mod1Mask || state & Mod5Mask) /* Alt */
    iupKEY_SETALT(status);

  if (state & Mod4Mask) /* Apple/Win */
    iupKEY_SETSYS(status);

  if (doubleclick)
    iupKEY_SETDOUBLE(status);
}

