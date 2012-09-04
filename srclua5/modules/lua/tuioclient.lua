------------------------------------------------------------------------------
-- TuioClient class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "tuioclient",
  parent = iup.WIDGET,
  creation = "N",  -- optional integer
  funcname = "TuioClient",
  callback = {
  },
  include = "iuptuio.h",
  extracode = [[ 

int iuptuiolua_open(lua_State* L)
{
  if (iuplua_opencall_internal(L))
    IupTuioOpen();
    
  iuplua_get_env(L);
  iuptuioclientlua_open(L);
  
  return 0;
}

/* obligatory to use require"iupluatuio" */
int luaopen_iupluatuio(lua_State* L)
{
  return iuptuiolua_open(L);
}

]]
}

function ctrl.createElement(class, param)
  return iup.TuioClient(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
