require("iuplua")
require("iuplua_pplot")

tblSurnames = {Blogs=4,
Smith=5,
Jones=12,
Weaver=3,
Whitesmith=4
}

plot = iup.pplot{
  title = "Surname Distribution",
  marginbottom="65",
  marginleft="65",
  axs_xlabel=" ",
  axs_ylabel="Count",
  legendshow="YES",
  legendpos="TOPLEFT",
  axs_xmin = "0",
  axs_ymin = "0",
  axs_yautomin = "NO",
  axs_xautomin = "NO",
  axs_xautotick = "no",
  ds_showvalues = "YES"
}

iup.PPlotBegin(plot, 1)
i = 0
for strSurname, iQty in pairs(tblSurnames) do
    iup.PPlotAddStr(plot, strSurname, iQty)
    i = i + 1
end
iup.PPlotEnd(plot)

plot.ds_mode = "BAR"
plot.ds_legend = "Surnames" 

d = iup.dialog{plot, size="800x200", title="PPlot"}

d:show()

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
