------------------------------------------------------------------------------
-- Label class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "label",
  parent = iup.WIDGET,
  creation = "S",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Label(param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
