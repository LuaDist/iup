------------------------------------------------------------------------------
-- ColorDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "colordlg",
  parent = iup.WIDGET,
  creation = "",
  funcname = "ColorDlg",
  callback = {}
} 

function ctrl.popup(handle, x, y)
  iup.Popup(handle,x,y)
end

function ctrl.createElement(class, param)
   return iup.ColorDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")

