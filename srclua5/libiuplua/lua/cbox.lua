------------------------------------------------------------------------------
-- Cbox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "cbox",
  parent = iup.BOX,
  creation = "-",
  callback = {},
}

function ctrl.createElement(class, param)
  return iup.Cbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
