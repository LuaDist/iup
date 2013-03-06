require'iuplua'

local label1 = iup.label{title = 'param 1'} 
local label2 = iup.label{title = 'param 2 with long name'}

local norm = iup.normalizer{label1, label2} 
norm.normalize = "HORIZONTAL"
norm:destroy()

local dialog = iup.dialog
{
   title="a dialog", size="QUARTERxQUARTER",
   iup.vbox
   {
      cgap = "5x5",
      cmargin = "5x5",
      iup.hbox
      {
         label1,
         iup.text{},
      },
      iup.hbox
      {
         label2,
         iup.text{},
      }
   }
}

dialog:show()
iup.MainLoop()

dialog:destroy()
iup.Close()
