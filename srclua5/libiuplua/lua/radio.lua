------------------------------------------------------------------------------
-- Radio class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "radio",
  parent = iup.WIDGET,
  creation = "I",
  callback = {}
} 

-- TODO: remove this code in the future, it seems to be useless an incorrect
function ctrl.SetChildrenNames(obj)
  if obj then
    if obj.parent.parent == iup.BOX then
      local i = 1
      while obj[i] do
        ctrl.SetChildrenNames (obj[i])
        i = i+1
      end
    elseif obj.parent == iup.FRAME then
      ctrl.SetChildrenNames (obj[1])
    else
      iup.SetHandleName(obj)
    end
  end
end

function ctrl.createElement(class, param)
   ctrl.SetChildrenNames(param[1])
   return iup.Radio(param[1])
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
