
------------------------------------------------------------------------------
-- User class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "user",
  parent = iup.WIDGET,
  creation = "",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.User()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
