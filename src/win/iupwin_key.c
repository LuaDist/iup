/** \file
 * \brief Windows Driver keyboard mapping
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>

#include <windows.h>

#include "iup.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_key.h"

#include "iup_drv.h"
#include "iupwin_drv.h"
                   
                   
typedef struct _Iwin2iupkey
{
  int wincode;
  int iupcode;
  int s_iupcode;
  int c_iupcode;
  int m_iupcode;
  int y_iupcode;
} Iwin2iupkey;

static Iwin2iupkey winkey_map[] = {

{ VK_ESCAPE,    K_ESC,   K_sESC,    K_cESC,   K_mESC,   K_yESC   },
{ VK_PAUSE,     K_PAUSE, K_sPAUSE,  K_cPAUSE, K_mPAUSE, K_yPAUSE },
{ VK_SNAPSHOT,  K_Print, K_sPrint,  K_cPrint, K_mPrint, K_yPrint },
{ VK_APPS,      K_Menu,  K_sMenu,   K_cMenu,  K_mMenu,  K_yMenu  },
                                              
{ VK_HOME,      K_HOME,  K_sHOME,   K_cHOME,  K_mHOME,  K_yHOME  },
{ VK_UP,        K_UP,    K_sUP,     K_cUP,    K_mUP,    K_yUP    },
{ VK_PRIOR,     K_PGUP,  K_sPGUP,   K_cPGUP,  K_mPGUP,  K_yPGUP  },
{ VK_LEFT,      K_LEFT,  K_sLEFT,   K_cLEFT,  K_mLEFT,  K_yLEFT  },
{ VK_CLEAR,     K_MIDDLE,K_sMIDDLE, K_cMIDDLE,K_mMIDDLE,K_yMIDDLE},
{ VK_RIGHT,     K_RIGHT, K_sRIGHT,  K_cRIGHT, K_mRIGHT, K_yRIGHT },
{ VK_END,       K_END,   K_sEND,    K_cEND,   K_mEND,   K_yEND   },
{ VK_DOWN,      K_DOWN,  K_sDOWN,   K_cDOWN,  K_mDOWN,  K_yDOWN  },
{ VK_NEXT,      K_PGDN,  K_sPGDN,   K_cPGDN,  K_mPGDN,  K_yPGDN  },
{ VK_INSERT,    K_INS,   K_sINS,    K_cINS,   K_mINS,   K_yINS   },
{ VK_DELETE,    K_DEL,   K_sDEL,    K_cDEL,   K_mDEL,   K_yDEL   },
{ VK_SPACE,     K_SP,    K_sSP,     K_cSP,    K_mSP,    K_ySP    },
{ VK_TAB,       K_TAB,   K_sTAB,    K_cTAB,   K_mTAB,   K_yTAB   },
{ VK_RETURN,    K_CR,    K_sCR,     K_cCR,    K_mCR,    K_yCR    },
{ VK_BACK,      K_BS,    K_sBS,     K_cBS,    K_mBS,    K_yBS    },

/* VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39) */
{ '1',   K_1, K_exclam,      K_c1, K_m1, K_y1 },
{ '2',   K_2, K_at,          K_c2, K_m2, K_y2 },
{ '3',   K_3, K_numbersign,  K_c3, K_m3, K_y3 },
{ '4',   K_4, K_dollar,      K_c4, K_m4, K_y4 },
{ '5',   K_5, K_percent,     K_c5, K_m5, K_y5 },
{ '6',   K_6, K_circum,      K_c6, K_m6, K_y6 },
{ '7',   K_7, K_ampersand,   K_c7, K_m7, K_y7 },
{ '8',   K_8, K_asterisk,    K_c8, K_m8, K_y8 },
{ '9',   K_9, K_parentleft,  K_c9, K_m9, K_y9 },
{ '0',   K_0, K_parentright, K_c0, K_m0, K_y0 },

/* VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A) */
{ 'A',   K_a, K_A, K_cA, K_mA, K_yA},
{ 'B',   K_b, K_B, K_cB, K_mB, K_yB},
{ 'C',   K_c, K_C, K_cC, K_mC, K_yC},
{ 'D',   K_d, K_D, K_cD, K_mD, K_yD},
{ 'E',   K_e, K_E, K_cE, K_mE, K_yE},
{ 'F',   K_f, K_F, K_cF, K_mF, K_yF},
{ 'G',   K_g, K_G, K_cG, K_mG, K_yG},
{ 'H',   K_h, K_H, K_cH, K_mH, K_yH},
{ 'I',   K_i, K_I, K_cI, K_mI, K_yI},
{ 'J',   K_j, K_J, K_cJ, K_mJ, K_yJ},
{ 'K',   K_k, K_K, K_cK, K_mK, K_yK},
{ 'L',   K_l, K_L, K_cL, K_mL, K_yL},
{ 'M',   K_m, K_M, K_cM, K_mM, K_yM},
{ 'N',   K_n, K_N, K_cN, K_mN, K_yN},
{ 'O',   K_o, K_O, K_cO, K_mO, K_yO},
{ 'P',   K_p, K_P, K_cP, K_mP, K_yP},
{ 'Q',   K_q, K_Q, K_cQ, K_mQ, K_yQ},
{ 'R',   K_r, K_R, K_cR, K_mR, K_yR},
{ 'S',   K_s, K_S, K_cS, K_mS, K_yS},
{ 'T',   K_t, K_T, K_cT, K_mT, K_yT},
{ 'U',   K_u, K_U, K_cU, K_mU, K_yU},
{ 'V',   K_v, K_V, K_cV, K_mV, K_yV},
{ 'W',   K_w, K_W, K_cW, K_mW, K_yW},
{ 'X',   K_x, K_X, K_cX, K_mX, K_yX},
{ 'Y',   K_y, K_Y, K_cY, K_mY, K_yY},
{ 'Z',   K_z, K_Z, K_cZ, K_mZ, K_yZ},

{ VK_F1,  K_F1,  K_sF1,  K_cF1,  K_mF1,  K_yF1  },
{ VK_F2,  K_F2,  K_sF2,  K_cF2,  K_mF2,  K_yF2  },
{ VK_F3,  K_F3,  K_sF3,  K_cF3,  K_mF3,  K_yF3  },
{ VK_F4,  K_F4,  K_sF4,  K_cF4,  K_mF4,  K_yF4  },
{ VK_F5,  K_F5,  K_sF5,  K_cF5,  K_mF5,  K_yF5  },
{ VK_F6,  K_F6,  K_sF6,  K_cF6,  K_mF6,  K_yF6  },
{ VK_F7,  K_F7,  K_sF7,  K_cF7,  K_mF7,  K_yF7  },
{ VK_F8,  K_F8,  K_sF8,  K_cF8,  K_mF8,  K_yF8  },
{ VK_F9,  K_F9,  K_sF9,  K_cF9,  K_mF9,  K_yF9  },
{ VK_F10, K_F10, K_sF10, K_cF10, K_mF10, K_yF10 },
{ VK_F11, K_F11, K_sF11, K_cF11, K_mF11, K_yF11 },
{ VK_F12, K_F12, K_sF12, K_cF12, K_mF12, K_yF12 },

{ VK_OEM_1,      K_semicolon,    K_colon,      K_cSemicolon, K_mSemicolon, K_ySemicolon },
{ VK_OEM_2,      K_slash,        K_question,   K_cSlash,     K_mSlash,  K_ySlash },
{ VK_OEM_3,      K_grave,        K_tilde,      0, 0, 0 },
{ VK_OEM_4,      K_bracketleft,  K_braceleft,  K_cBracketleft, K_mBracketleft, K_yBracketleft },
{ VK_OEM_5,      K_backslash,    K_bar,        K_cBackslash,   K_mBackslash,   K_yBackslash },
{ VK_OEM_6,      K_bracketright, K_braceright, K_cBracketright,K_mBracketright,K_yBracketright },
{ VK_OEM_7,      K_apostrophe,   K_quotedbl,   0, 0, 0 },

{ VK_OEM_102,    K_backslash,    K_bar,        K_cBackslash,   K_mBackslash,   K_yBackslash },
{ VK_OEM_PLUS,   K_equal,        K_plus,       K_cEqual,     K_mEqual,  K_yEqual },
{ VK_OEM_COMMA,  K_comma,        K_less,       K_cComma,     K_mComma,  K_yComma },
{ VK_OEM_MINUS,  K_minus,        K_underscore, K_cMinus,     K_mMinus,  K_yMinus },
{ VK_OEM_PERIOD, K_period,       K_greater,    K_cPeriod,    K_mPeriod, K_yPeriod },

{ VK_NUMPAD0,   K_0, K_0,  K_c0, K_m0, K_y0 },
{ VK_NUMPAD1,   K_1, K_1,  K_c1, K_m1, K_y1 },
{ VK_NUMPAD2,   K_2, K_2,  K_c2, K_m2, K_y2 },
{ VK_NUMPAD3,   K_3, K_3,  K_c3, K_m3, K_y3 },
{ VK_NUMPAD4,   K_4, K_4,  K_c4, K_m4, K_y4 },
{ VK_NUMPAD5,   K_5, K_5,  K_c5, K_m5, K_y5 },
{ VK_NUMPAD6,   K_6, K_6,  K_c6, K_m6, K_y6 },
{ VK_NUMPAD7,   K_7, K_7,  K_c7, K_m7, K_y7 },
{ VK_NUMPAD8,   K_8, K_8,  K_c8, K_m8, K_y8 },
{ VK_NUMPAD9,   K_9, K_9,  K_c9, K_m9, K_y9 },
{ VK_MULTIPLY,  K_asterisk, K_sAsterisk, K_cAsterisk, K_mAsterisk, K_yAsterisk },
{ VK_ADD,       K_plus,     K_sPlus,     K_cPlus,     K_mPlus,     K_yPlus },
{ VK_SUBTRACT,  K_minus,    K_sMinus,    K_cMinus,    K_mMinus,    K_yMinus },
{ VK_DECIMAL,   K_period,   K_sPeriod,   K_cPeriod,   K_mPeriod,   K_yPeriod },
{ VK_DIVIDE,    K_slash,    K_sSlash,    K_cSlash,    K_mSlash,    K_ySlash },
{ VK_SEPARATOR, K_comma,    K_sComma,    K_cComma,    K_mComma,    K_yComma }
};

/* the following tables compensate variations in combinations on the 
   same phisical key */

static Iwin2iupkey keytable_abnt[] = {
{ '6',   K_6, K_circum,      K_c6, K_m6, K_y6 },  /* diaeresis ¨ */

{ VK_OEM_1,    K_ccedilla,     K_Ccedilla,   K_cCcedilla, K_mCcedilla, K_yCcedilla },
{ VK_OEM_2,    K_semicolon,    K_colon,      K_cSemicolon, K_mSemicolon, K_ySemicolon  },
{ VK_OEM_3,    K_apostrophe,   K_quotedbl,   0,   0,   0  },
{ VK_OEM_4,    K_acute,        K_grave,      0,   0,   0  },
{ VK_OEM_5,    K_bracketright, K_braceright, K_cBracketleft, K_mBracketleft, K_yBracketleft  },
{ VK_OEM_6,    K_bracketleft,  K_braceleft,  K_cBracketright, K_mBracketright, K_yBracketright  },
{ VK_OEM_7,    K_tilde,        K_circum,     0,   0,   0  },

{ 0xC1,        K_slash,        K_question,   K_cSlash, K_mSlash, K_ySlash  },
{ 0xC2,        K_period,       K_sPeriod,    K_cPeriod, K_mPeriod, K_yPeriod },

{ VK_SEPARATOR,K_period,       K_sPeriod,    K_cPeriod, K_mPeriod, K_yPeriod },
{ VK_DECIMAL,  K_comma,        K_sComma,     K_cComma, K_mComma, K_yComma }
};

static Iwin2iupkey keytable_azerty[] = {
{ '1',   K_ampersand,   K_1, K_c1, K_m1, K_y1 },
{ '2',   K_e,           K_2, K_c2, K_m2, K_y2 },  /* actually "e´" */
{ '3',   K_quotedbl,    K_3, K_c3, K_m3, K_y3 },
{ '4',   K_apostrophe,  K_4, K_c4, K_m4, K_y4 },
{ '5',   K_parentleft,  K_5, K_c5, K_m5, K_y5 },
{ '6',   K_minus,       K_6, K_c6, K_m6, K_y6 },
{ '7',   K_e,           K_7, K_c7, K_m7, K_y7 },  /* actually "`e" */
{ '8',   K_underscore,  K_8, K_c8, K_m8, K_y8 },
{ '9',   K_ccedilla,    K_9, K_c9, K_m9, K_y9 },
{ '0',   K_a,           K_0, K_c0, K_m0, K_y0 },  /* actually "`a" */

{ VK_OEM_1,  K_dollar,  K_dollar,   0, 0, 0 },  /* actually "pound" */
{ VK_OEM_2,  K_colon,    K_slash,   0, 0, 0 },
{ VK_OEM_3,  K_u,      K_percent,   0, 0, 0 },  /* actually "`u" */
{ VK_OEM_4,  K_parentright,  K_o,   0, 0, 0 },
{ VK_OEM_5,  K_asterisk,     K_m,   0, 0, 0 },  /* actually "micro" */
{ VK_OEM_6,  K_circum,  K_circum,   0, 0, 0 },  /* diaeresis ¨ */
{ VK_OEM_7,  K_2,              0,   0, 0, 0 },  /* actually "^2" */
{ VK_OEM_8,  K_exclam,         0,   0, 0, 0 },

{ VK_OEM_102,    K_less,      K_greater,  0, 0, 0 },
{ VK_OEM_PLUS,   K_plus,      K_equal,     K_cEqual,     K_mEqual,  K_yEqual },
{ VK_OEM_COMMA,  K_comma,     K_question,  K_cComma,     K_mComma,  K_yComma },
{ VK_OEM_PERIOD, K_semicolon, K_period,    K_cSemicolon, K_mSemicolon, K_ySemicolon }

/* These are available with the Alt+Ctrl combination, not used in IUP
       K_at  @                          K_braceleft }
       K_numbersign  #                  K_backslash  \
       K_bar |                          K_bracketright ]
       K_bracketleft [                  K_braceright }
 */
};

void iupdrvKeyEncode(int key, unsigned int *keyval, unsigned int *state)
{
  int i, iupcode = key & 0xFF; /* 0-255 interval */
  int count = sizeof(winkey_map)/sizeof(winkey_map[0]);
  for (i = 0; i < count; i++)
  {
    Iwin2iupkey* key_map = &(winkey_map[i]);
    if (key_map->iupcode == iupcode)
    {
      *keyval = key_map->wincode;
      *state = 0;

      if (iupcode != key)
      {
        if (key_map->c_iupcode == key)
          *state = VK_CONTROL;
        else if (key_map->m_iupcode == key)
          *state = VK_MENU;
        else if (key_map->y_iupcode == key)
          *state = VK_LWIN;
        else if (key_map->s_iupcode == key)
          *state = VK_SHIFT;
      }
      return;
    }
    else if (key_map->s_iupcode == key)   /* There are Shift keys below 256 */
    {
      *keyval = key_map->wincode;
      *state = VK_SHIFT;
      return;
    }
  }
}

static int winKeyMap2Iup(Iwin2iupkey* table, int i)
{
  int code = 0;
  if (GetKeyState(VK_CONTROL) & 0x8000)        /* Ctrl */
    code = table[i].c_iupcode;
  else if (GetKeyState(VK_MENU) & 0x8000)      /* Alt */
    code = table[i].m_iupcode;
  else if ((GetKeyState(VK_LWIN) & 0x8000) ||  /* Apple/Win */
           (GetKeyState(VK_RWIN) & 0x8000))
    code = table[i].y_iupcode;
  else if (GetKeyState(VK_CAPITAL) & 0x01)     /* CapsLock */
  {
    if ((GetKeyState(VK_SHIFT) & 0x8000) || !iupKeyCanCaps(table[i].iupcode))
      return table[i].iupcode;
    else
      code = table[i].s_iupcode;
  }
  else if (GetKeyState(VK_SHIFT) & 0x8000)     /* Shift */
    code = table[i].s_iupcode;
  else
    return table[i].iupcode;

  if (!code)
    code = table[i].iupcode;

  return code;
}

int iupwinKeyDecode(int wincode)
{
  HKL k;
  int i, count;

  k = GetKeyboardLayout(0);    
  if ((int)HIWORD(k) == 0x0416) /* ABNT */
  {
    count = sizeof(keytable_abnt)/sizeof(keytable_abnt[0]);
    for (i = 0; i < count; i++)
    {
      if (keytable_abnt[i].wincode == wincode)
        return winKeyMap2Iup(keytable_abnt, i);
    }
  }
  else if (((int)HIWORD(k) & 0x00FF) == 0x000C) /* AZERTY (French) */
  {
    count = sizeof(keytable_azerty)/sizeof(keytable_azerty[0]);
    for (i = 0; i < count; i++)
    {
      if (keytable_azerty[i].wincode == wincode)
        return winKeyMap2Iup(keytable_azerty, i);
    }
  }

  count = sizeof(winkey_map)/sizeof(winkey_map[0]);
  for (i = 0; i < count; i++)
  {
    if (winkey_map[i].wincode == wincode)
      return winKeyMap2Iup(winkey_map, i);
  }

  return 0;
}

int iupwinKeyEvent(Ihandle* ih, int wincode, int press)
{
  int result, code;

  if (!ih->iclass->is_interactive)
    return 1;

  code = iupwinKeyDecode(wincode);
  if (code == 0)
    return 1;

  if (press)
  {
    result = iupKeyCallKeyCb(ih, code);
    if (result == IUP_CLOSE)
    {
      IupExitLoop();
      return 1;
    }
    else if (result == IUP_IGNORE)
      return 0;

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
          return 1;
        }
        else if (result == IUP_IGNORE)
          return 0;
      }
    }

    if (!iupKeyProcessNavigation(ih, code, (GetKeyState(VK_SHIFT) & 0x8000)))
      return 0;
  }
  else
  {
    /* this is called only for canvas */
    if (ih->iclass->nativetype == IUP_TYPECANVAS)
    {
      result = iupKeyCallKeyPressCb(ih, code, 0);
      if (result == IUP_CLOSE)
      {
        IupExitLoop();
        return 1;
      }
      else if (result == IUP_IGNORE)
        return 0;
    }
  }

  return 1;
}

void iupwinButtonKeySetStatus(WORD keys, char* status, int doubleclick)
{
  if (keys & MK_SHIFT)
    iupKEY_SETSHIFT(status);

  if (keys & MK_CONTROL)
    iupKEY_SETCONTROL(status); 

  if (keys & MK_LBUTTON)
    iupKEY_SETBUTTON1(status);

  if (keys & MK_MBUTTON)
    iupKEY_SETBUTTON2(status);

  if (keys & MK_RBUTTON)
    iupKEY_SETBUTTON3(status);

  if (doubleclick)
    iupKEY_SETDOUBLE(status);

  if (GetKeyState(VK_MENU) & 0x8000)
    iupKEY_SETALT(status);

  if ((GetKeyState(VK_LWIN) & 0x8000) || (GetKeyState(VK_RWIN) & 0x8000))
    iupKEY_SETSYS(status);

  if (keys & MK_XBUTTON1)
    iupKEY_SETBUTTON4(status);

  if (keys & MK_XBUTTON2)
    iupKEY_SETBUTTON5(status);
}
