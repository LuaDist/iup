------------------------------------------------------------------------------
-- VBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "vbox",
  parent = iup.BOX,
  creation = "-",
  callback = {}
}

function ctrl.append (handle, elem)
  iup.Append(handle, elem)
end

function ctrl.createElement(class, param)
   return iup.Vbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
