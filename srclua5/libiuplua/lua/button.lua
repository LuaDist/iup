------------------------------------------------------------------------------
-- Button class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "button",
  parent = iup.WIDGET,
  creation = "S-",
  callback = {
    action = "", 
  }
} 

function ctrl.createElement(class, param)
  return iup.Button(param.title)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
