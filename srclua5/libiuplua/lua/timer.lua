------------------------------------------------------------------------------
-- Timer class 
------------------------------------------------------------------------------
local ctrl = {
  nick     = "timer",
  parent   = iup.WIDGET,
  creation = "",
  callback = {
    action_cb = "", 
  },
} 

function ctrl.createElement(class, param)
  return iup.Timer()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
