------------------------------------------------------------------------------
-- GLCanvas class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "glcanvas",
  parent = iup.WIDGET,
  creation = "-",
  funcname = "GLCanvas",
  include = "iupgl.h",
  callback = {
    action = "nn",
  },
  extrafuncs = 1,
  extracode = [[ 
int iupgllua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupGLCanvasOpen();

  iuplua_get_env(L);
  iupglcanvaslua_open(L);
  return 0;
}

/* obligatory to use require"iupluagl" */
int luaopen_iupluagl(lua_State* L)
{
  return iupgllua_open(L);
}

]]
}

function ctrl.createElement(class, param)
   return iup.GLCanvas()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
