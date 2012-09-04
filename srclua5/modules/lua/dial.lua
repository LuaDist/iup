------------------------------------------------------------------------------
-- Dial class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "dial",
  parent = iup.WIDGET,
  creation = "S",
  callback = {
     mousemove_cb = "d",       -- already registered by the val, but has conflict with matrix, so we must set it here again
--     button_press_cb = "d",    -- already registered by the val
--     button_release_cb = "d",  -- already registered by the val
  },
  include = "iupcontrols.h",
}

function ctrl.createElement(class, param)
   return iup.Dial(param[1])
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
