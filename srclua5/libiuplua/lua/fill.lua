------------------------------------------------------------------------------
-- Fill class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "fill",
  parent = iup.WIDGET,
  creation = "",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Fill()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
