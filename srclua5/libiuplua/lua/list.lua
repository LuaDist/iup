------------------------------------------------------------------------------
-- List class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "list",
  parent = iup.WIDGET,
  creation = "-",
  callback = {
     action = "snn", 
     multiselect_cb = "s",
     edit_cb = "ns",
     dropdown_cb = "n",
     dblclick_cb = "ns",
   }
} 

function ctrl.createElement(class, param)
   return iup.List()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
