------------------------------------------------------------------------------
-- Tabs class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "tabs",
  parent = iup.BOX,
  creation = "-",
  callback = {
    tabchange_cb = "ii",
    tabchangepos_cb = "nn",
  },
}

function ctrl.createElement(class, param)
  return iup.Tabs()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
