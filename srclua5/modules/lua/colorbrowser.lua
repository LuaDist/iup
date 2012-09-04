------------------------------------------------------------------------------
-- ColorBrowser class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "colorbrowser",
  parent = iup.WIDGET,
  creation = "",
  callback = {
    drag_cb = "ccc",
    change_cb = "ccc",
  },
  funcname = "ColorBrowser",
  include = "iupcontrols.h",
}

function ctrl.createElement(class, param)
   return iup.ColorBrowser(param.action)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
