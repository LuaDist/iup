------------------------------------------------------------------------------
-- ZBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "zbox",
  parent = iup.BOX,
  creation = "-",
  callback = {}
}

function ctrl.append (handle, elem)
  iup.SetHandleName(elem)
  iup.Append(handle, elem)
end

function ctrl.SetChildrenNames(obj)
  if obj then
    local i = 1
    while obj[i] do
      iup.SetHandleName(obj[i])
      i = i+1
    end
  end
end

function ctrl.createElement(class, param)
   ctrl.SetChildrenNames(param)
   return iup.Zbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
