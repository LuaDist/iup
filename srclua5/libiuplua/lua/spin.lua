------------------------------------------------------------------------------
-- Spin class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "spin",
  parent = iup.WIDGET,
  creation = "",
  callback = {
    spin_cb = "n",
  },
}

function ctrl.createElement(class, param)
   return iup.Spin(param.action)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
