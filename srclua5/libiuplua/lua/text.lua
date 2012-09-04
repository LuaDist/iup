------------------------------------------------------------------------------
-- Text class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "text",
  parent = iup.WIDGET,
  creation = "-", 
  callback = {
    action = "ns",
    caret_cb = "nnn", 
    valuechanged_cb = "",  -- used by many other controls
  }
}

function ctrl.createElement(class, param)
   return iup.Text()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
