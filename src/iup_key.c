/** \file
 * \brief Manage keys encoding and decoding.
 *
 * See Copyright Notice in "iup.h"
 */

#include <memory.h>
#include <stdio.h> 

#include "iup.h"
#include "iupkey.h"
#include "iupcbs.h"

#include "iup_key.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_drv.h"
#include "iup_focus.h"


static struct
{
   char *name;
   int code;
} ikey_map_list[]= 
  {
    {"K_exclam",         K_exclam},               
    {"K_grave",          K_grave},             
    {"K_quotedbl",       K_quotedbl},             
    {"K_numbersign",     K_numbersign},           
    {"K_dollar",         K_dollar},               
    {"K_percent",        K_percent},              
    {"K_ampersand",      K_ampersand},            
    {"K_apostrophe",     K_apostrophe},           
    {"K_parentleft",     K_parentleft},            
    {"K_parentright",    K_parentright},          
    {"K_asterisk",       K_asterisk},    {"K_sAsterisk",K_sAsterisk},{"K_cAsterisk",K_cAsterisk},{"K_mAsterisk",K_mAsterisk},{"K_yAsterisk",K_yAsterisk},          
    {"K_plus",           K_plus},        {"K_sPlus",   K_sPlus},     {"K_cPlus",   K_cPlus},     {"K_mPlus",   K_mPlus},   {"K_yPlus",   K_yPlus},          
    {"K_comma",          K_comma},       {"K_sComma",  K_sComma},    {"K_cComma",  K_cComma},    {"K_mComma",  K_mComma},  {"K_yComma",  K_yComma},         
    {"K_minus",          K_minus},       {"K_sMinus",  K_sMinus},    {"K_cMinus",  K_cMinus},    {"K_mMinus",  K_mMinus},  {"K_yMinus",  K_yMinus},         
    {"K_period",         K_period},      {"K_sPeriod", K_sPeriod},   {"K_cPeriod", K_cPeriod},   {"K_mPeriod", K_mPeriod}, {"K_yPeriod", K_yPeriod},        
    {"K_slash",          K_slash},       {"K_sSlash",  K_sSlash},    {"K_cSlash",  K_cSlash},    {"K_mSlash",  K_mSlash},  {"K_ySlash",  K_ySlash},         
    {"K_colon",          K_colon},                
    {"K_semicolon ",     K_semicolon},    {"K_cSemicolon ",     K_cSemicolon},    {"K_mSemicolon ",     K_mSemicolon},    {"K_ySemicolon ",     K_ySemicolon},       
    {"K_less",           K_less},                                                                                                 
    {"K_equal",          K_equal},        {"K_cEqual",          K_cEqual},        {"K_mEqual",          K_mEqual},        {"K_yEqual",          K_yEqual},       
    {"K_greater",        K_greater},                                                                                              
    {"K_question",       K_question},                                                                                             
    {"K_bracketleft",    K_bracketleft},  {"K_cBracketleft",    K_cBracketleft},  {"K_mBracketleft",    K_mBracketleft},  {"K_yBracketleft",    K_yBracketleft},        
    {"K_backslash",      K_backslash},    {"K_cBackslash",      K_cBackslash},    {"K_mBackslash",      K_mBackslash},    {"K_yBackslash",      K_yBackslash},                                                                                 
    {"K_bracketright",   K_bracketright}, {"K_cBracketright",   K_cBracketright}, {"K_mBracketright",   K_mBracketright}, {"K_yBracketright",   K_yBracketright},      
    {"K_circum",         K_circum},               
    {"K_underscore",     K_underscore},           
    {"K_at",             K_at},                   
    {"K_braceleft",      K_braceleft},           
    {"K_bar",            K_bar},                  
    {"K_braceright",     K_braceright},           
    {"K_tilde",          K_tilde}, 
    {"K_acute",          K_acute}, 
    {"K_ccedilla"       ,K_ccedilla},  {"K_Ccedilla" ,K_Ccedilla}, {"K_cCcedilla" ,K_cCcedilla}, {"K_mCcedilla" ,K_mCcedilla}, {"K_yCcedilla" ,K_yCcedilla},
    {"K_ESC"   ,K_ESC},    {"K_sESC"   ,K_sESC},   {"K_cESC"   ,K_cESC},   {"K_mESC"   ,K_mESC},    {"K_yESC"   ,K_yESC},
    {"K_PAUSE" ,K_PAUSE},  {"K_sPAUSE" ,K_sPAUSE}, {"K_cPAUSE" ,K_cPAUSE}, {"K_mPAUSE" ,K_mPAUSE},  {"K_yPAUSE" ,K_yPAUSE},
    {"K_HOME"  ,K_HOME},   {"K_sHOME"  ,K_sHOME},  {"K_cHOME"  ,K_cHOME},  {"K_mHOME"  ,K_mHOME},   {"K_yHOME"  ,K_yHOME},  
    {"K_UP"    ,K_UP},     {"K_sUP"    ,K_sUP},    {"K_cUP"    ,K_cUP},    {"K_mUP"    ,K_mUP},     {"K_yUP"    ,K_yUP},    
    {"K_PGUP"  ,K_PGUP},   {"K_sPGUP"  ,K_sPGUP},  {"K_cPGUP"  ,K_cPGUP},  {"K_mPGUP"  ,K_mPGUP},   {"K_yPGUP"  ,K_yPGUP},  
    {"K_LEFT"  ,K_LEFT},   {"K_sLEFT"  ,K_sLEFT},  {"K_cLEFT"  ,K_cLEFT},  {"K_mLEFT"  ,K_mLEFT},   {"K_yLEFT"  ,K_yLEFT},  
    {"K_MIDDLE",K_MIDDLE}, {"K_sMIDDLE",K_sMIDDLE},{"K_cMIDDLE",K_cMIDDLE},{"K_mMIDDLE",K_mMIDDLE}, {"K_yMIDDLE",K_yMIDDLE},
    {"K_RIGHT" ,K_RIGHT},  {"K_sRIGHT" ,K_sRIGHT}, {"K_cRIGHT" ,K_cRIGHT}, {"K_mRIGHT" ,K_mRIGHT},  {"K_yRIGHT" ,K_yRIGHT}, 
    {"K_END"   ,K_END},    {"K_sEND"   ,K_sEND},   {"K_cEND"   ,K_cEND},   {"K_mEND"   ,K_mEND},    {"K_yEND"   ,K_yEND},   
    {"K_DOWN"  ,K_DOWN},   {"K_sDOWN"  ,K_sDOWN},  {"K_cDOWN"  ,K_cDOWN},  {"K_mDOWN"  ,K_mDOWN},   {"K_yDOWN"  ,K_yDOWN},  
    {"K_PGDN"  ,K_PGDN},   {"K_sPGDN"  ,K_sPGDN},  {"K_cPGDN"  ,K_cPGDN},  {"K_mPGDN"  ,K_mPGDN},   {"K_yPGDN"  ,K_yPGDN},  
    {"K_INS"   ,K_INS},    {"K_sINS"   ,K_sINS},   {"K_cINS"   ,K_cINS},   {"K_mINS"   ,K_mINS},    {"K_yINS"   ,K_yINS},   
    {"K_DEL"   ,K_DEL},    {"K_sDEL"   ,K_sDEL},   {"K_cDEL"   ,K_cDEL},   {"K_mDEL"   ,K_mDEL},    {"K_yDEL"   ,K_yDEL},   
    {"K_SP"    ,K_SP},     {"K_sSP"    ,K_sSP},    {"K_cSP"    ,K_cSP},    {"K_mSP"    ,K_mSP},     {"K_ySP"    ,K_ySP},
    {"K_TAB"   ,K_TAB},    {"K_sTAB"   ,K_sTAB},   {"K_cTAB"   ,K_cTAB},   {"K_mTAB"   ,K_mTAB},    {"K_yTAB"   ,K_yTAB},
    {"K_CR"    ,K_CR},     {"K_sCR"    ,K_sCR},    {"K_cCR"    ,K_cCR},    {"K_mCR"    ,K_mCR},     {"K_yCR"    ,K_yCR},
    {"K_BS"    ,K_BS},     {"K_sBS"    ,K_sBS},    {"K_cBS"    ,K_cBS},    {"K_mBS"    ,K_mBS},     {"K_yBS"    ,K_yBS},
    {"K_Print" ,K_Print},  {"K_sPrint" ,K_sPrint}, {"K_cPrint" ,K_cPrint}, {"K_mPrint" ,K_mPrint},  {"K_yPrint" ,K_yPrint},
    {"K_Menu"  ,K_Menu},   {"K_sMenu"  ,K_sMenu},  {"K_cMenu"  ,K_cMenu},  {"K_mMenu"  ,K_mMenu},   {"K_yMenu"  ,K_yMenu},
    {"K_0",   K_0},                       {"K_c0",   K_c0},     {"K_m0", K_m0},     {"K_y0", K_y0},                       
    {"K_1",   K_1},                       {"K_c1",   K_c1},     {"K_m1", K_m1},     {"K_y1", K_y1},                       
    {"K_2",   K_2},                       {"K_c2",   K_c2},     {"K_m2", K_m2},     {"K_y2", K_y2},                       
    {"K_3",   K_3},                       {"K_c3",   K_c3},     {"K_m3", K_m3},     {"K_y3", K_y3},                       
    {"K_4",   K_4},                       {"K_c4",   K_c4},     {"K_m4", K_m4},     {"K_y4", K_y4},                       
    {"K_5",   K_5},                       {"K_c5",   K_c5},     {"K_m5", K_m5},     {"K_y5", K_y5},                       
    {"K_6",   K_6},                       {"K_c6",   K_c6},     {"K_m6", K_m6},     {"K_y6", K_y6},                       
    {"K_7",   K_7},                       {"K_c7",   K_c7},     {"K_m7", K_m7},     {"K_y7", K_y7},                        
    {"K_8",   K_8},                       {"K_c8",   K_c8},     {"K_m8", K_m8},     {"K_y8", K_y8},
    {"K_9",   K_9},                       {"K_c9",   K_c9},     {"K_m9", K_m9},     {"K_y9", K_y9},
    {"K_a",   K_a},   {"K_A",    K_A},    {"K_cA"    ,K_cA},    {"K_mA"   ,K_mA},   {"K_yA"   ,K_yA}, 
    {"K_b",   K_b},   {"K_B",    K_B},    {"K_cB"    ,K_cB},    {"K_mB"   ,K_mB},   {"K_yB"   ,K_yB}, 
    {"K_c",   K_c},   {"K_C",    K_C},    {"K_cC"    ,K_cC},    {"K_mC"   ,K_mC},   {"K_yC"   ,K_yC}, 
    {"K_d",   K_d},   {"K_D",    K_D},    {"K_cD"    ,K_cD},    {"K_mD"   ,K_mD},   {"K_yD"   ,K_yD}, 
    {"K_e",   K_e},   {"K_E",    K_E},    {"K_cE"    ,K_cE},    {"K_mE"   ,K_mE},   {"K_yE"   ,K_yE}, 
    {"K_f",   K_f},   {"K_F",    K_F},    {"K_cF"    ,K_cF},    {"K_mF"   ,K_mF},   {"K_yF"   ,K_yF}, 
    {"K_g",   K_g},   {"K_G",    K_G},    {"K_cG"    ,K_cG},    {"K_mG"   ,K_mG},   {"K_yG"   ,K_yG}, 
    {"K_h",   K_h},   {"K_H",    K_H},    {"K_cH"    ,K_cH},    {"K_mH"   ,K_mH},   {"K_yH"   ,K_yH}, 
    {"K_i",   K_i},   {"K_I",    K_I},    {"K_cI"    ,K_cI},    {"K_mI"   ,K_mI},   {"K_yI"   ,K_yI}, 
    {"K_j",   K_j},   {"K_J",    K_J},    {"K_cJ"    ,K_cJ},    {"K_mJ"   ,K_mJ},   {"K_yJ"   ,K_yJ}, 
    {"K_k",   K_k},   {"K_K",    K_K},    {"K_cK"    ,K_cK},    {"K_mK"   ,K_mK},   {"K_yK"   ,K_yK}, 
    {"K_l",   K_l},   {"K_L",    K_L},    {"K_cL"    ,K_cL},    {"K_mL"   ,K_mL},   {"K_yL"   ,K_yL}, 
    {"K_m",   K_m},   {"K_M",    K_M},    {"K_cM"    ,K_cM},    {"K_mM"   ,K_mM},   {"K_yM"   ,K_yM}, 
    {"K_n",   K_n},   {"K_N",    K_N},    {"K_cN"    ,K_cN},    {"K_mN"   ,K_mN},   {"K_yN"   ,K_yN}, 
    {"K_o",   K_o},   {"K_O",    K_O},    {"K_cO"    ,K_cO},    {"K_mO"   ,K_mO},   {"K_yO"   ,K_yO}, 
    {"K_p",   K_p},   {"K_P",    K_P},    {"K_cP"    ,K_cP},    {"K_mP"   ,K_mP},   {"K_yP"   ,K_yP}, 
    {"K_q",   K_q},   {"K_Q",    K_Q},    {"K_cQ"    ,K_cQ},    {"K_mQ"   ,K_mQ},   {"K_yQ"   ,K_yQ}, 
    {"K_r",   K_r},   {"K_R",    K_R},    {"K_cR"    ,K_cR},    {"K_mR"   ,K_mR},   {"K_yR"   ,K_yR}, 
    {"K_s",   K_s},   {"K_S",    K_S},    {"K_cS"    ,K_cS},    {"K_mS"   ,K_mS},   {"K_yS"   ,K_yS}, 
    {"K_t",   K_t},   {"K_T",    K_T},    {"K_cT"    ,K_cT},    {"K_mT"   ,K_mT},   {"K_yT"   ,K_yT}, 
    {"K_u",   K_u},   {"K_U",    K_U},    {"K_cU"    ,K_cU},    {"K_mU"   ,K_mU},   {"K_yU"   ,K_yU}, 
    {"K_v",   K_v},   {"K_V",    K_V},    {"K_cV"    ,K_cV},    {"K_mV"   ,K_mV},   {"K_yV"   ,K_yV}, 
    {"K_w",   K_w},   {"K_W",    K_W},    {"K_cW"    ,K_cW},    {"K_mW"   ,K_mW},   {"K_yW"   ,K_yW}, 
    {"K_x",   K_x},   {"K_X",    K_X},    {"K_cX"    ,K_cX},    {"K_mX"   ,K_mX},   {"K_yX"   ,K_yX}, 
    {"K_y",   K_y},   {"K_Y",    K_Y},    {"K_cY"    ,K_cY},    {"K_mY"   ,K_mY},   {"K_yY"   ,K_yY}, 
    {"K_z",   K_z},   {"K_Z",    K_Z},    {"K_cZ"    ,K_cZ},    {"K_mZ"   ,K_mZ},   {"K_yZ"   ,K_yZ}, 
    {"K_F1"  ,K_F1},  {"K_sF1"  ,K_sF1},  {"K_cF1"   ,K_cF1},   {"K_mF1"  ,K_mF1},  {"K_yF1"  ,K_yF1}, 
    {"K_F2"  ,K_F2},  {"K_sF2"  ,K_sF2},  {"K_cF2"   ,K_cF2},   {"K_mF2"  ,K_mF2},  {"K_yF2"  ,K_yF2}, 
    {"K_F3"  ,K_F3},  {"K_sF3"  ,K_sF3},  {"K_cF3"   ,K_cF3},   {"K_mF3"  ,K_mF3},  {"K_yF3"  ,K_yF3}, 
    {"K_F4"  ,K_F4},  {"K_sF4"  ,K_sF4},  {"K_cF4"   ,K_cF4},   {"K_mF4"  ,K_mF4},  {"K_yF4"  ,K_yF4}, 
    {"K_F5"  ,K_F5},  {"K_sF5"  ,K_sF5},  {"K_cF5"   ,K_cF5},   {"K_mF5"  ,K_mF5},  {"K_yF5"  ,K_yF5}, 
    {"K_F6"  ,K_F6},  {"K_sF6"  ,K_sF6},  {"K_cF6"   ,K_cF6},   {"K_mF6"  ,K_mF6},  {"K_yF6"  ,K_yF6}, 
    {"K_F7"  ,K_F7},  {"K_sF7"  ,K_sF7},  {"K_cF7"   ,K_cF7},   {"K_mF7"  ,K_mF7},  {"K_yF7"  ,K_yF7}, 
    {"K_F8"  ,K_F8},  {"K_sF8"  ,K_sF8},  {"K_cF8"   ,K_cF8},   {"K_mF8"  ,K_mF8},  {"K_yF8"  ,K_yF8}, 
    {"K_F9"  ,K_F9},  {"K_sF9"  ,K_sF9},  {"K_cF9"   ,K_cF9},   {"K_mF9"  ,K_mF9},  {"K_yF9"  ,K_yF9}, 
    {"K_F10" ,K_F10}, {"K_sF10" ,K_sF10}, {"K_cF10"  ,K_cF10},  {"K_mF10" ,K_mF10}, {"K_yF10" ,K_yF10},
    {"K_F11" ,K_F11}, {"K_sF11" ,K_sF11}, {"K_cF11"  ,K_cF11},  {"K_mF11" ,K_mF11}, {"K_yF11" ,K_yF11},
    {"K_F12" ,K_F12}, {"K_sF12" ,K_sF12}, {"K_cF12"  ,K_cF12},  {"K_mF12" ,K_mF12}, {"K_yF12" ,K_yF12},
    {NULL      ,0}
  };

static char* ikey_map[IUP_NUMMAXCODES];

void iupKeyInit(void)
{
  int i;
  memset(ikey_map, 0, IUP_NUMMAXCODES*sizeof(char*));
  for (i = 0; ikey_map_list[i].name; i++)
    ikey_map[ikey_map_list[i].code] = ikey_map_list[i].name;
}

int iupKeyCanCaps(int code)
{
  if (code >= K_a && code <= K_z)
    return 1;
  if (code == K_ccedilla)
    return 1;
  return 0;
}

char* iupKeyCodeToName(int code)
{
  if (code < 0 || code > IUP_NUMMAXCODES)
    return NULL;
  return ikey_map[code];
}

int iupKeyNameToCode(const char *name)
{
  int i;
  for (i = 0; ikey_map_list[i].name; i++)
    if (iupStrEqual(name, ikey_map_list[i].name))
      return ikey_map_list[i].code;
      
  return 0;
}

void iupKeyForEach(void (*func)(const char *name, int code, void* user_data), void* user_data)
{
  int i;
  for (i = 0; ikey_map_list[i].name; ++i)
    func(ikey_map_list[i].name, ikey_map_list[i].code, user_data);
}

int iupKeyCallKeyCb(Ihandle *ih, int code)
{
  char* name = iupKeyCodeToName(code);
  for (; ih; ih = ih->parent)
  {
    IFni cb = NULL;
    if (name)
      cb = (IFni)IupGetCallback(ih, name);
    if (!cb)
      cb = (IFni)IupGetCallback(ih, "K_ANY");

    if (cb)
    {
      int ret = cb(ih, code);
      if (ret != IUP_CONTINUE)
        return ret;
    }
  }
  return IUP_DEFAULT;
}

int iupKeyCallKeyPressCb(Ihandle *ih, int code, int press)
{
  IFnii cb = (IFnii)IupGetCallback(ih, "KEYPRESS_CB");
  if (cb) return cb(ih, code, press);
  return IUP_DEFAULT;
}

int iupKeyProcessNavigation(Ihandle* ih, int key, int shift)
{
  /* this is called after K_ANY is processed, 
     so the user may change its behavior */

  if (key == K_cTAB)
  {
    int is_multiline = (IupClassMatch(ih, "text") && IupGetInt(ih, "MULTILINE"));
    if (is_multiline)
    {
      if (shift)
        IupPreviousField(ih);
      else
        IupNextField(ih);
      return 0;   /* abort default processing */
    }
  }
  else if (key == K_TAB || key == K_sTAB)
  {
    int is_multiline = (IupClassMatch(ih, "text") && IupGetInt(ih, "MULTILINE"));
    if (!is_multiline)
    {
      if (key == K_sTAB)
        IupPreviousField(ih);
      else
        IupNextField(ih);
      return 0;   /* abort default processing */
    }
  }
  else if (key == K_UP || key == K_DOWN)
  {
    int is_button = (IupClassMatch(ih, "button") || 
                     IupClassMatch(ih, "toggle"));
    if (is_button)
    {
      if (key == K_UP)
        iupFocusPrevious(ih);
      else
        iupFocusNext(ih);
      return 0;   /* abort default processing */
    }
  }
  else if (key==K_ESC)
  {
    Ihandle* bt = IupGetAttributeHandle(IupGetDialog(ih), "DEFAULTESC");
    if (iupObjectCheck(bt) && IupClassMatch(bt, "button"))
      iupdrvActivate(bt);
    return 0;   /* abort default processing */
  }
  else if (key==K_CR || key==K_cCR)
  {
    int is_multiline = (IupClassMatch(ih, "text") && IupGetInt(ih, "MULTILINE"));
    if ((key==K_CR && !is_multiline) || (key==K_cCR && is_multiline))
    {
      Ihandle* bt = IupGetAttributeHandle(IupGetDialog(ih), "DEFAULTENTER");
      if (iupObjectCheck(bt) && IupClassMatch(bt, "button"))
        iupdrvActivate(bt);
      return 0;   /* abort default processing */
    }
  }

  return 1;
}

