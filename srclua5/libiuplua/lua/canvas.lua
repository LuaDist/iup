------------------------------------------------------------------------------
-- Canvas class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "canvas",
  parent = iup.WIDGET,
  creation = "-",
  callback = {
    action = "ff",
    button_cb = "nnnns",
    enterwindow_cb = "",
    leavewindow_cb = "",
    motion_cb = "nns",
    resize_cb = "nn",
    scroll_cb = "nff",
    keypress_cb = "nn",
    wom_cb = "n",
    wheel_cb = "fnns",
    mdiactivate_cb = "",
    touch_cb = "nnns",
    focus_cb = "n",
  }
}

function ctrl.createElement(class, param)
   return iup.Canvas()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
