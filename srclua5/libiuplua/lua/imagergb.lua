------------------------------------------------------------------------------
-- ImageRGB class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "imagergb",
  parent = iup.WIDGET,
  creation = "nns", -- fake definition
  funcname = "ImageRGB", 
  callback = {},
  createfunc = [[ 
static int ImageRGB(lua_State *L)
{
  int w = luaL_checkint(L, 1);
  int h = luaL_checkint(L, 2);
  unsigned char *pixels = iuplua_checkuchar_array(L, 3, w*h*3);
  Ihandle *ih = IupImageRGB(w, h, pixels);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  free(pixels);
  return 1;
}
 
]]
}

function ctrl.createElement(class, param)
   return iup.ImageRGB(param.width, param.height, param.pixels)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
