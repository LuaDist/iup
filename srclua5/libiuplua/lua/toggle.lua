------------------------------------------------------------------------------
-- Toggle class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "toggle",
  parent = iup.WIDGET,
  creation = "S-",
  callback = {
    action = "n",
  }
} 

function ctrl.createElement(class, param)
  return iup.Toggle(param.title)
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
