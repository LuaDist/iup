require'iuplua'

local norm = iup.normalizer{}
local dialog = iup.dialog
{
   title="a dialog", size="QUARTERxQUARTER",
   iup.vbox
   {
      CGAP = "5x5", CMARGIN = "5x5",
      iup.hbox
      {
         iup.label{title = 'param 1', NORMALIZERGROUP = norm};
         iup.text{};
      };
      iup.hbox
      {
         iup.label{title = 'param 2 with long name', NORMALIZERGROUP = norm};
         iup.text{};
      };
   };
}
norm.normalize = "HORIZONTAL"
iup.Destroy(norm)

dialog:show()
iup.MainLoop()

dialog:destroy()
