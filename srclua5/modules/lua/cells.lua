------------------------------------------------------------------------------
-- Cells class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "cells",
  parent = iup.WIDGET,
  creation = "",
  callback = {
    mouseclick_cb = "nnnnnns",
    mousemotion_cb = "nnnns",
    scrolling_cb = "nn",
--    draw_cb = "nnnnnnn",   -- already registered by the matrix
    width_cb = "n",
    height_cb = "n",
    nlines_cb = "",
    ncols_cb = "",
    hspan_cb = "nn",
    vspan_cb = "nn",
   },
  include = "iupcontrols.h",
}

function ctrl.redraw(handle)
   handle.repaint = "YES"
end

function ctrl.createElement(class, param)
   return iup.Cells()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
