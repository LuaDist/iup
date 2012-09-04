------------------------------------------------------------------------------
-- SpinBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "spinbox",
  parent = iup.WIDGET,
  creation = "i",
  callback = {
    spin_cb = "n",
  },
}

function ctrl.createElement(class, param)
   return iup.Spinbox(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
