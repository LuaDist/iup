------------------------------------------------------------------------------
-- FileDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "filedlg",
  parent = iup.WIDGET,
  creation = "",
  callback = {
    file_cb = "ss",
  },
  funcname = "FileDlg"
} 

function ctrl.popup(handle, x, y)
  iup.Popup(handle,x,y)
end

function ctrl.createElement(class, param)
   return iup.FileDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")

