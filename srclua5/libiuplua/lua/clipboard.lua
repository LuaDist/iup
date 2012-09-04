------------------------------------------------------------------------------
-- Clipboard class 
------------------------------------------------------------------------------
local ctrl = {
  nick     = "clipboard",
  parent   = iup.WIDGET,
  creation = "",
  callback = {
  },
} 

function ctrl.createElement(class, param)
  return iup.Clipboard()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
