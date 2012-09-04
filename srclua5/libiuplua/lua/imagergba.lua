------------------------------------------------------------------------------
-- ImageRGBA class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "imagergba",
  parent = iup.WIDGET,
  creation = "nns", -- fake definition
  funcname = "ImageRGBA", 
  callback = {},
  createfunc = [[ 
static int ImageRGBA(lua_State *L)
{
  int w = luaL_checkint(L, 1);
  int h = luaL_checkint(L, 2);
  unsigned char *pixels = iuplua_checkuchar_array(L, 3, w*h*4);
  Ihandle *ih = IupImageRGBA(w, h, pixels);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  free(pixels);
  return 1;
}
 
]]
}

function ctrl.createElement(class, param)
   return iup.ImageRGBA(param.width, param.height, param.pixels)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
