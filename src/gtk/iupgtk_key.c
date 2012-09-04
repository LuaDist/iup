/** \file
 * \brief GTK Driver keyboard mapping
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <stdlib.h>      
#include <stdio.h>      

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_key.h"

#include "iup_drv.h"
#include "iupgtk_drv.h"


typedef struct _Igtk2iupkey
{
  guint gtkcode;
  int iupcode;
  int s_iupcode;
  int c_iupcode;
  int m_iupcode;
  int y_iupcode;
} Igtk2iupkey;

static Igtk2iupkey gtkkey_map[] = {

{ GDK_Escape,    K_ESC,   K_sESC,    K_cESC,   K_mESC   ,K_yESC   },
{ GDK_Pause,     K_PAUSE, K_sPAUSE,  K_cPAUSE, K_mPAUSE ,K_yPAUSE },
{ GDK_Print,     K_Print, K_sPrint,  K_cPrint, K_mPrint ,K_yPrint },
{ GDK_Menu,      K_Menu,  K_sMenu,   K_cMenu,  K_mMenu  ,K_yMenu  },
                                                       
{ GDK_Home,      K_HOME,  K_sHOME,   K_cHOME,  K_mHOME  ,K_yHOME  },
{ GDK_Up,        K_UP,    K_sUP,     K_cUP,    K_mUP    ,K_yUP    },
{ GDK_Prior,     K_PGUP,  K_sPGUP,   K_cPGUP,  K_mPGUP  ,K_yPGUP  },
{ GDK_Left,      K_LEFT,  K_sLEFT,   K_cLEFT,  K_mLEFT  ,K_yLEFT  },
{ GDK_Begin,     K_MIDDLE,K_sMIDDLE, K_cMIDDLE,K_mMIDDLE,K_yMIDDLE},
{ GDK_Right,     K_RIGHT, K_sRIGHT,  K_cRIGHT, K_mRIGHT ,K_yRIGHT },
{ GDK_End,       K_END,   K_sEND,    K_cEND,   K_mEND   ,K_yEND   },
{ GDK_Down,      K_DOWN,  K_sDOWN,   K_cDOWN,  K_mDOWN  ,K_yDOWN  },
{ GDK_Next,      K_PGDN,  K_sPGDN,   K_cPGDN,  K_mPGDN  ,K_yPGDN  },
{ GDK_Insert,    K_INS,   K_sINS,    K_cINS,   K_mINS   ,K_yINS   },
{ GDK_Delete,    K_DEL,   K_sDEL,    K_cDEL,   K_mDEL   ,K_yDEL   },
{ GDK_space,     K_SP,    K_sSP,     K_cSP,    K_mSP    ,K_ySP    },
{ GDK_Tab,       K_TAB,   K_sTAB,    K_cTAB,   K_mTAB   ,K_yTAB   },
{ GDK_Return,    K_CR,    K_sCR,     K_cCR,    K_mCR    ,K_yCR    },
{ GDK_BackSpace, K_BS,    K_sBS,     K_cBS,    K_mBS    ,K_yBS    },

{ GDK_1,   K_1, K_exclam,      K_c1, K_m1, K_y1 },
{ GDK_2,   K_2, K_at,          K_c2, K_m2, K_y2 },
{ GDK_3,   K_3, K_numbersign,  K_c3, K_m3, K_y3 },
{ GDK_4,   K_4, K_dollar,      K_c4, K_m4, K_y4 },
{ GDK_5,   K_5, K_percent,     K_c5, K_m5, K_y5 },
{ GDK_6,   K_6, K_circum,      K_c6, K_m6, K_y6 },
{ GDK_7,   K_7, K_ampersand,   K_c7, K_m7, K_y7 },
{ GDK_8,   K_8, K_asterisk,    K_c8, K_m8, K_y8 },
{ GDK_9,   K_9, K_parentleft,  K_c9, K_m9, K_y9 },
{ GDK_0,   K_0, K_parentright, K_c0, K_m0, K_y0 },

/* Shift will be flaged so s_iupcode will contain the right code */
{ GDK_exclam,     K_1, K_exclam,      K_c1, K_m1, K_y1 },
{ GDK_at,         K_2, K_at,          K_c2, K_m2, K_y2 },
{ GDK_numbersign, K_3, K_numbersign,  K_c3, K_m3, K_y3 },
{ GDK_dollar,     K_4, K_dollar,      K_c4, K_m4, K_y4 },
{ GDK_percent,    K_5, K_percent,     K_c5, K_m5, K_y5 },
{ GDK_dead_diaeresis, K_6, K_circum,  K_c6, K_m6, K_y6 },
{ GDK_ampersand,  K_7, K_ampersand,   K_c7, K_m7, K_y7 },
{ GDK_asterisk,   K_8, K_asterisk,    K_c8, K_m8, K_y8 },
{ GDK_parenleft,  K_9, K_parentleft,  K_c9, K_m9, K_y9 },
{ GDK_parenright, K_0, K_parentright, K_c0, K_m0, K_y0 },

{ GDK_a,   K_a, K_A, K_cA, K_mA, K_yA },
{ GDK_b,   K_b, K_B, K_cB, K_mB, K_yB },
{ GDK_c,   K_c, K_C, K_cC, K_mC, K_yC },
{ GDK_d,   K_d, K_D, K_cD, K_mD, K_yD },
{ GDK_e,   K_e, K_E, K_cE, K_mE, K_yE },
{ GDK_f,   K_f, K_F, K_cF, K_mF, K_yF },
{ GDK_g,   K_g, K_G, K_cG, K_mG, K_yG },
{ GDK_h,   K_h, K_H, K_cH, K_mH, K_yH },
{ GDK_i,   K_i, K_I, K_cI, K_mI, K_yI },
{ GDK_j,   K_j, K_J, K_cJ, K_mJ, K_yJ },
{ GDK_k,   K_k, K_K, K_cK, K_mK, K_yK },
{ GDK_l,   K_l, K_L, K_cL, K_mL, K_yL },
{ GDK_m,   K_m, K_M, K_cM, K_mM, K_yM },
{ GDK_n,   K_n, K_N, K_cN, K_mN, K_yN },
{ GDK_o,   K_o, K_O, K_cO, K_mO, K_yO },
{ GDK_p,   K_p, K_P, K_cP, K_mP, K_yP },
{ GDK_q,   K_q, K_Q, K_cQ, K_mQ, K_yQ },
{ GDK_r,   K_r, K_R, K_cR, K_mR, K_yR },
{ GDK_s,   K_s, K_S, K_cS, K_mS, K_yS },
{ GDK_t,   K_t, K_T, K_cT, K_mT, K_yT },
{ GDK_u,   K_u, K_U, K_cU, K_mU, K_yU },
{ GDK_v,   K_v, K_V, K_cV, K_mV, K_yV },
{ GDK_w,   K_w, K_W, K_cW, K_mW, K_yW },
{ GDK_x,   K_x, K_X, K_cX, K_mX, K_yX },
{ GDK_y,   K_y, K_Y, K_cY, K_mY, K_yY },
{ GDK_z,   K_z, K_Z, K_cZ, K_mZ, K_yZ },

/* Shift will be flaged so s_iupcode will contain the right code */
{ GDK_A,   K_a, K_A, K_cA, K_mA, K_yA },
{ GDK_B,   K_b, K_B, K_cB, K_mB, K_yB },
{ GDK_C,   K_c, K_C, K_cC, K_mC, K_yC },
{ GDK_D,   K_d, K_D, K_cD, K_mD, K_yD },
{ GDK_E,   K_e, K_E, K_cE, K_mE, K_yE },
{ GDK_F,   K_f, K_F, K_cF, K_mF, K_yF },
{ GDK_G,   K_g, K_G, K_cG, K_mG, K_yG },
{ GDK_H,   K_h, K_H, K_cH, K_mH, K_yH },
{ GDK_I,   K_i, K_I, K_cI, K_mI, K_yI },
{ GDK_J,   K_j, K_J, K_cJ, K_mJ, K_yJ },
{ GDK_K,   K_k, K_K, K_cK, K_mK, K_yK },
{ GDK_L,   K_l, K_L, K_cL, K_mL, K_yL },
{ GDK_M,   K_m, K_M, K_cM, K_mM, K_yM },
{ GDK_N,   K_n, K_N, K_cN, K_mN, K_yN },
{ GDK_O,   K_o, K_O, K_cO, K_mO, K_yO },
{ GDK_P,   K_p, K_P, K_cP, K_mP, K_yP },
{ GDK_Q,   K_q, K_Q, K_cQ, K_mQ, K_yQ },
{ GDK_R,   K_r, K_R, K_cR, K_mR, K_yR },
{ GDK_S,   K_s, K_S, K_cS, K_mS, K_yS },
{ GDK_T,   K_t, K_T, K_cT, K_mT, K_yT },
{ GDK_U,   K_u, K_U, K_cU, K_mU, K_yU },
{ GDK_V,   K_v, K_V, K_cV, K_mV, K_yV },
{ GDK_W,   K_w, K_W, K_cW, K_mW, K_yW },
{ GDK_X,   K_x, K_X, K_cX, K_mX, K_yX },
{ GDK_Y,   K_y, K_Y, K_cY, K_mY, K_yY },
{ GDK_Z,   K_z, K_Z, K_cZ, K_mZ, K_yZ },

{ GDK_F1,  K_F1,  K_sF1,  K_cF1,  K_mF1,  K_yF1 },
{ GDK_F2,  K_F2,  K_sF2,  K_cF2,  K_mF2,  K_yF2 },
{ GDK_F3,  K_F3,  K_sF3,  K_cF3,  K_mF3,  K_yF3 },
{ GDK_F4,  K_F4,  K_sF4,  K_cF4,  K_mF4,  K_yF4 },
{ GDK_F5,  K_F5,  K_sF5,  K_cF5,  K_mF5,  K_yF5 },
{ GDK_F6,  K_F6,  K_sF6,  K_cF6,  K_mF6,  K_yF6 },
{ GDK_F7,  K_F7,  K_sF7,  K_cF7,  K_mF7,  K_yF7 },
{ GDK_F8,  K_F8,  K_sF8,  K_cF8,  K_mF8,  K_yF8 },
{ GDK_F9,  K_F9,  K_sF9,  K_cF9,  K_mF9,  K_yF9 },
{ GDK_F10, K_F10, K_sF10, K_cF10, K_mF10, K_yF10 },
{ GDK_F11, K_F11, K_sF11, K_cF11, K_mF11, K_yF11 },
{ GDK_F12, K_F12, K_sF12, K_cF12, K_mF12, K_yF12 },

{ GDK_semicolon,   K_semicolon,    K_colon,      K_cSemicolon, K_mSemicolon, K_ySemicolon },
{ GDK_equal,       K_equal,        K_plus,       K_cEqual,     K_mEqual,  K_yEqual },
{ GDK_comma,       K_comma,        K_less,       K_cComma,     K_mComma,  K_yComma },
{ GDK_minus,       K_minus,        K_underscore, K_cMinus,     K_mMinus,  K_yMinus },
{ GDK_period,      K_period,       K_greater,    K_cPeriod,    K_mPeriod, K_yPeriod },
{ GDK_slash,       K_slash,        K_question,   K_cSlash,     K_mSlash,  K_ySlash },
{ GDK_grave,       K_grave,        K_tilde,      0, 0, 0 },
{ GDK_bracketleft, K_bracketleft,  K_braceleft,  K_cBracketleft, K_mBracketleft, K_yBracketleft },
{ GDK_backslash,   K_backslash,    K_bar,        K_cBackslash,   K_mBackslash,   K_yBackslash },
{ GDK_bracketright,K_bracketright, K_braceright, K_cBracketright,K_mBracketright,K_yBracketright },
{ GDK_apostrophe,  K_apostrophe,   K_quotedbl,   0, 0, 0 },

/* Shift will be flaged so s_iupcode will contain the right code */
{ GDK_colon,      K_semicolon,    K_colon,      K_cSemicolon, K_mSemicolon, K_ySemicolon },
{ GDK_plus,       K_equal,        K_plus,       K_cEqual,     K_mEqual,  K_yEqual },
{ GDK_less,       K_comma,        K_less,       K_cComma,     K_mComma,  K_yComma },
{ GDK_underscore, K_minus,        K_underscore, K_cMinus,     K_mMinus,  K_yMinus },
{ GDK_greater,    K_period,       K_greater,    K_cPeriod,    K_mPeriod, K_yPeriod },
{ GDK_question,   K_slash,        K_question,   K_cSlash,     K_mSlash,  K_ySlash },
{ GDK_braceleft,  K_bracketleft,  K_braceleft,  K_cBracketleft, K_mBracketleft, K_yBracketleft },
{ GDK_bar,        K_backslash,    K_bar,        K_cBackslash,   K_mBackslash,   K_yBackslash },
{ GDK_braceright, K_bracketright, K_braceright, K_cBracketright,K_mBracketright,K_yBracketright },
{ GDK_quotedbl,   K_apostrophe,   K_quotedbl,   0, 0, 0 },

{ GDK_KP_0,   K_0, K_0,  K_c0, K_m0, K_y0 },
{ GDK_KP_1,   K_1, K_1,  K_c1, K_m1, K_y1 },
{ GDK_KP_2,   K_2, K_2,  K_c2, K_m2, K_y2 },
{ GDK_KP_3,   K_3, K_3,  K_c3, K_m3, K_y3 },
{ GDK_KP_4,   K_4, K_4,  K_c4, K_m4, K_y4 },
{ GDK_KP_5,   K_5, K_5,  K_c5, K_m5, K_y5 },
{ GDK_KP_6,   K_6, K_6,  K_c6, K_m6, K_y6 },
{ GDK_KP_7,   K_7, K_7,  K_c7, K_m7, K_y7 },
{ GDK_KP_8,   K_8, K_8,  K_c8, K_m8, K_y8 },
{ GDK_KP_9,   K_9, K_9,  K_c9, K_m9, K_y9 },
{ GDK_KP_Multiply,  K_asterisk, K_sAsterisk, K_cAsterisk, K_mAsterisk, K_yAsterisk },
{ GDK_KP_Add,       K_plus,     K_sPlus,     K_cPlus,     K_mPlus,     K_yPlus },
{ GDK_KP_Subtract,  K_minus,    K_sMinus,    K_cMinus,    K_mMinus,    K_yMinus },
{ GDK_KP_Decimal,   K_period,   K_sPeriod,   K_cPeriod,   K_mPeriod,   K_yPeriod },
{ GDK_KP_Divide,    K_slash,    K_sSlash,    K_cSlash,    K_mSlash,    K_ySlash },
{ GDK_KP_Separator, K_comma,    K_sComma,    K_cComma,    K_mComma,    K_yComma },

{ GDK_ccedilla,     K_ccedilla, K_Ccedilla, K_cCcedilla, K_mCcedilla, K_yCcedilla },
{ GDK_Ccedilla,     K_ccedilla, K_Ccedilla, K_cCcedilla, K_mCcedilla, K_yCcedilla },

{ GDK_dead_tilde,      K_tilde, K_circum, 0, 0, 0 },
{ GDK_dead_acute,      K_acute, K_grave,  0, 0, 0 },
{ GDK_dead_grave,      K_grave, K_tilde,  0, 0, 0 },
{ GDK_dead_circumflex, K_tilde, K_circum, 0, 0, 0 },

{ GDK_KP_F1,        K_F1,    K_sF1,    K_cF1,    K_mF1,    K_yF1 },
{ GDK_KP_F2,        K_F2,    K_sF2,    K_cF2,    K_mF2,    K_yF2 },
{ GDK_KP_F3,        K_F3,    K_sF3,    K_cF3,    K_mF3,    K_yF3 },
{ GDK_KP_F4,        K_F4,    K_sF4,    K_cF4,    K_mF4,    K_yF4 },
{ GDK_KP_Space,     K_SP,    K_sSP,    K_cSP,    K_mSP    ,K_ySP    },
{ GDK_KP_Tab,       K_TAB,   K_sTAB,   K_cTAB,   K_mTAB   ,K_yTAB   },
{ GDK_KP_Equal,     K_equal, 0,        K_cEqual, K_mEqual, K_yEqual },

{ GDK_KP_Enter,     K_CR,    K_sCR,     K_cCR,    K_mCR,    K_yCR    },
{ GDK_KP_Home,      K_HOME,  K_sHOME,   K_cHOME,  K_mHOME,  K_yHOME  },
{ GDK_KP_Up,        K_UP,    K_sUP,     K_cUP,    K_mUP,    K_yUP    },
{ GDK_KP_Page_Up,   K_PGUP,  K_sPGUP,   K_cPGUP,  K_mPGUP,  K_yPGUP  },
{ GDK_KP_Left,      K_LEFT,  K_sLEFT,   K_cLEFT,  K_mLEFT,  K_yLEFT  },
{ GDK_KP_Begin,     K_MIDDLE,K_sMIDDLE, K_cMIDDLE,K_mMIDDLE,K_yMIDDLE},
{ GDK_KP_Right,     K_RIGHT, K_sRIGHT,  K_cRIGHT, K_mRIGHT, K_yRIGHT },
{ GDK_KP_End,       K_END,   K_sEND,    K_cEND,   K_mEND,   K_yEND   },
{ GDK_KP_Down,      K_DOWN,  K_sDOWN,   K_cDOWN,  K_mDOWN,  K_yDOWN  },
{ GDK_KP_Page_Down, K_PGDN,  K_sPGDN,   K_cPGDN,  K_mPGDN,  K_yPGDN  },
{ GDK_KP_Insert,    K_INS,   K_sINS,    K_cINS,   K_mINS,   K_yINS   },
{ GDK_KP_Delete,    K_DEL,   K_sDEL,    K_cDEL,   K_mDEL,   K_yDEL   }

};

void iupdrvKeyEncode(int key, unsigned int *keyval, unsigned int *state)
{
  int i, iupcode = key & 0xFF; /* 0-255 interval */
  int count = sizeof(gtkkey_map)/sizeof(gtkkey_map[0]);
  for (i = 0; i < count; i++)
  {
    Igtk2iupkey* key_map = &(gtkkey_map[i]);
    if (key_map->iupcode == iupcode)
    {
      *keyval = key_map->gtkcode;
      *state = 0;

      if (iupcode != key)
      {
        if (key_map->c_iupcode == key)
          *state = GDK_CONTROL_MASK;
        else if (key_map->m_iupcode == key)
          *state = GDK_MOD1_MASK;
        else if (key_map->y_iupcode == key)
          *state = GDK_MOD4_MASK;
        else if (key_map->s_iupcode == key)
          *state = GDK_SHIFT_MASK;
      }
      return;
    }
    else if (key_map->s_iupcode == key)   /* There are Shift keys below 256 */
    {
      *keyval = key_map->gtkcode;
      *state = GDK_SHIFT_MASK;

      if ((*keyval >= GDK_a) &&
          (*keyval <= GDK_z))
      {
        /* remap to upper case */
        *keyval -= GDK_a-GDK_A;
      }
      return;
    }
  }
}

static int gtkKeyMap2Iup(int state, int i)
{
  int code = 0;
  if (state & GDK_CONTROL_MASK)   /* Ctrl */
    code = gtkkey_map[i].c_iupcode;
  else if (state & GDK_MOD1_MASK ||
           state & GDK_MOD5_MASK) /* Alt */
    code = gtkkey_map[i].m_iupcode;
  else if (state & GDK_MOD4_MASK) /* Apple/Win */
    code = gtkkey_map[i].y_iupcode;
  else if (state & GDK_LOCK_MASK) /* CapsLock */
  {
    if ((state & GDK_SHIFT_MASK) || !iupKeyCanCaps(gtkkey_map[i].iupcode))
      return gtkkey_map[i].iupcode;
    else
      code = gtkkey_map[i].s_iupcode;
  }
  else if (state & GDK_SHIFT_MASK) /* Shift */
    code = gtkkey_map[i].s_iupcode;
  else
    return gtkkey_map[i].iupcode;

  if (!code)
    code = gtkkey_map[i].iupcode;

  return code;
}

int iupgtkKeyDecode(GdkEventKey *evt)
{
  int i;
  int count = sizeof(gtkkey_map)/sizeof(gtkkey_map[0]);
  guint keyval = evt->keyval;

  if ((evt->state & GDK_MOD2_MASK) && /* NumLock */
      (keyval >= GDK_KP_Home) &&
      (keyval <= GDK_KP_Delete))
  {
    /* remap to numeric keys */
    guint remap_numkey[] = {GDK_KP_7, GDK_KP_4, GDK_KP_8, GDK_KP_6, GDK_KP_2, GDK_KP_9, GDK_KP_3, GDK_KP_1, GDK_KP_5, GDK_KP_0, GDK_KP_Decimal};
    keyval = remap_numkey[keyval-GDK_KP_Home];
  }

  for (i = 0; i < count; i++)
  {
    if (gtkkey_map[i].gtkcode == keyval)
      return gtkKeyMap2Iup(evt->state, i);
  }

  return 0;
}

static int iupObjectIsNativeContainer(Ihandle* ih)
{
  if (ih->iclass->childtype != IUP_CHILDNONE && 
      ih->iclass->nativetype != IUP_TYPEVOID)
    return 1;
  else
    return 0;
}

gboolean iupgtkKeyPressEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle *ih)
{
  int result;
  int code = iupgtkKeyDecode(evt);
  if (code == 0) 
    return FALSE;

  /* Avoid duplicate calls if a child of a native container contains the focus.
     GTK will call the callback for the child and for the container.
     Ignore the one sent to the parent. For now only IupDialog and IupTabs
     have keyboard signals intercepted.
     */
  if (iupObjectIsNativeContainer(ih))
  {
    GtkWindow* win = (GtkWindow*)IupGetDialog(ih)->handle;
    GtkWidget *widget_focus = gtk_window_get_focus(win);
    if (widget_focus && widget_focus != widget)
      return FALSE;
  }

  result = iupKeyCallKeyCb(ih, code);
  if (result == IUP_CLOSE)
  {
    IupExitLoop();
    return FALSE;
  }
  if (result == IUP_IGNORE)
    return TRUE;

  /* in the previous callback the dialog could be destroyed */
  if (iupObjectCheck(ih))
  {
    /* this is called only for canvas */
    if (ih->iclass->nativetype == IUP_TYPECANVAS) 
    {
      result = iupKeyCallKeyPressCb(ih, code, 1);
      if (result == IUP_CLOSE)
      {
        IupExitLoop();
        return FALSE;
      }
      if (result == IUP_IGNORE)
        return TRUE;
    }

    if (!iupKeyProcessNavigation(ih, code, evt->state & GDK_SHIFT_MASK))
      return TRUE;

    /* compensate the show-help limitation. 
     * It is not called on F1, only on Shift+F1 and Ctrl+F1. */
    if (code == K_F1)
    {
      Icallback cb = IupGetCallback(ih, "HELP_CB");
      if (cb)
      {
        if (cb(ih) == IUP_CLOSE) 
          IupExitLoop();
      }
    }
  }

  (void)widget;
  return FALSE;
}

gboolean iupgtkKeyReleaseEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle *ih)
{
  /* this is called only for canvas */
  int result;
  int code = iupgtkKeyDecode(evt);
  if (code == 0) 
    return FALSE;

  result = iupKeyCallKeyPressCb(ih, code, 0);
  if (result == IUP_CLOSE)
  {
    IupExitLoop();
    return FALSE;
  }
  if (result == IUP_IGNORE)
    return TRUE;

  (void)widget;
  return FALSE;
}

void iupgtkButtonKeySetStatus(guint state, unsigned int but, char* status, int doubleclick)
{
  if (state & GDK_SHIFT_MASK)
    iupKEY_SETSHIFT(status);

  if (state & GDK_CONTROL_MASK)
    iupKEY_SETCONTROL(status); 

  if ((state & GDK_BUTTON1_MASK) || but==1)
    iupKEY_SETBUTTON1(status);

  if ((state & GDK_BUTTON2_MASK) || but==2)
    iupKEY_SETBUTTON2(status);

  if ((state & GDK_BUTTON3_MASK) || but==3)
    iupKEY_SETBUTTON3(status);

  if ((state & GDK_BUTTON4_MASK) || but==4)
    iupKEY_SETBUTTON4(status);

  if ((state & GDK_BUTTON5_MASK) || but==5)
    iupKEY_SETBUTTON5(status);

  if (state & GDK_MOD1_MASK || state & GDK_MOD5_MASK) /* Alt */
    iupKEY_SETALT(status);

  if (state & GDK_MOD4_MASK) /* Apple/Win */
    iupKEY_SETSYS(status);

  if (doubleclick)
    iupKEY_SETDOUBLE(status);
}

