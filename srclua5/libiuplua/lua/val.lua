------------------------------------------------------------------------------
-- Val class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "val",
  parent = iup.WIDGET,
  creation = "S",
  callback = {
    mousemove_cb = "d",
    button_press_cb = "d",
    button_release_cb = "d",
  },
}

function ctrl.createElement(class, param)
   return iup.Val(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
