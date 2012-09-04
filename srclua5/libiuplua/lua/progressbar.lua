------------------------------------------------------------------------------
-- ProgressBar class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "progressbar",
  parent = iup.WIDGET,
  creation = "",
  funcname = "ProgressBar",
  callback = {}
} 

function ctrl.createElement(class, param)
   return iup.ProgressBar()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
