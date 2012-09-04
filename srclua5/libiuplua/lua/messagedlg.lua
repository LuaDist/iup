------------------------------------------------------------------------------
-- MessageDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "messagedlg",
  parent = iup.WIDGET,
  creation = "",
  funcname = "MessageDlg",
  callback = {}
} 

function ctrl.popup(handle, x, y)
  iup.Popup(handle,x,y)
end

function ctrl.createElement(class, param)
   return iup.MessageDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")

