
------------------------------------------------------------------------------
-- Normalizer class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "normalizer",
  parent = iup.WIDGET,
  creation = "-",
  callback = {}
}

function ctrl.setAttributes(object, param)
  local handle = rawget(object, "handle")
  local n = #param
  for i = 1, n do
    if iup.GetClass(param[i]) == "iup handle" then 
      object.handle.addcontrol = param[i]
    end
  end
  iup.WIDGET.setAttributes(object, param)
end

function ctrl.createElement(class, param)
   return iup.Normalizer()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
