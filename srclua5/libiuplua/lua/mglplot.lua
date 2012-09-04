------------------------------------------------------------------------------
-- MglPlot class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "mglplot",
  parent = iup.WIDGET,
  creation = "",
  funcname = "MglPlot",
  callback = {
--    select_cb = "nnffn",
--    selectbegin_cb = "",
--    selectend_cb = "",
    predraw_cb = "",
    postdraw_cb = "",
--    edit_cb = "nnffff",  -- fake definitions to be replaced by mglplotfuncs module
--    editbegin_cb = "",
--    editend_cb = "",
--    delete_cb = "nnff",
--    deletebegin_cb = "",
--    deleteend_cb = "",
  },
  include = "iup_mglplot.h",
  extrafuncs = 1,
}

function ctrl.createElement(class, param)
   return iup.MglPlot(param.action)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
