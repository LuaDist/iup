------------------------------------------------------------------------------
-- Frame class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "frame",
  parent = iup.WIDGET,
  creation = "I",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Frame(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
