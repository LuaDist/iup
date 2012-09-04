------------------------------------------------------------------------------
-- Split class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "split",
  parent = iup.WIDGET,
  creation = "II",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Split(param[1], param[2])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
