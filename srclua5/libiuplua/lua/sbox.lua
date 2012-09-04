------------------------------------------------------------------------------
-- Sbox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "sbox",
  parent = iup.WIDGET,
  creation = "I",
  callback = {}
}

function ctrl.createElement(class, param)
   return iup.Sbox(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
