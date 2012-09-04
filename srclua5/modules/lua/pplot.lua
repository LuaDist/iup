------------------------------------------------------------------------------
-- PPlot class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "pplot",
  parent = iup.WIDGET,
  creation = "",
  funcname = "PPlot",
  callback = {
    select_cb = "nnffn",
    selectbegin_cb = "",
    selectend_cb = "",
    predraw_cb = "n",   -- fake definitions to be replaced by pplotfuncs module
    postdraw_cb = "n",  -- fake definitions to be replaced by pplotfuncs module
    edit_cb = "nnffff",  -- fake definitions to be replaced by pplotfuncs module
    editbegin_cb = "",
    editend_cb = "",
    delete_cb = "nnff",
    deletebegin_cb = "",
    deleteend_cb = "",
  },
  include = "iup_pplot.h",
  extrafuncs = 1,
}

function ctrl.createElement(class, param)
   return iup.PPlot(param.action)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
