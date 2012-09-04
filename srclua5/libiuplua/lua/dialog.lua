------------------------------------------------------------------------------
-- Dialog class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "dialog",
  parent = iup.WIDGET,
  creation = "I",
  callback = {
    map_cb = "",
    unmap_cb = "",
    destroy_cb = "",
    close_cb = "",
    show_cb = "n",
    move_cb = "nn",
    tips_cb = "nn",
    copydata_cb = "sn",
    trayclick_cb = "nnn",
    dropfiles_cb = "snnn",
    dragbegin_cb = "nn",
    dragdatasize_cb = "s",
    dragdata_cb = "son",
    dragend_cb = "n",
    dropdata_cb = "sonnn",
  }
}

function ctrl.createElement(class, param)
   return iup.Dialog(param[1])
end

function ctrl.popup(handle, x, y)
  iup.Popup(handle,x,y)
end

function ctrl.showxy(handle, x, y)
  return iup.ShowXY(handle, x, y)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
