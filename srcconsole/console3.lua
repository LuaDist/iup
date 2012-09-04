-- Utilities

-- Dummy require
function require()
end

function printvars()
  local n,v = nextvar(nil)
  print("--printvars Start--")
  while n ~= nil do
    print(tostring(n).."="..tostring(v))
    n,v = nextvar(n)
  end
  print("--printvars End--")
end

function printtable(t)
  local n,v = next(t, nil)
  print("--printtable Start--")
  while n ~= nil do
    print(tostring(n).."="..tostring(v))
    n,v = next(t, n)
  end
  print("--printtable End--")
end

function print_version_info()
  print(_VERSION .. " " .. iup._LUA_COPYRIGHT)
  if (im) then print(im._VERSION .. " " .. im._COPYRIGHT) end
  if (cd and cd._VERSION) then print(cd._VERSION .. " " .. cd._COPYRIGHT) end
  print(iup._VERSION .. " " .. iup._COPYRIGHT)
  print("")
  print("IUP Info")
  print("  System: " .. iup.GetGlobal("SYSTEM"))
  print("  System Version: " .. iup.GetGlobal("SYSTEMVERSION"))
  local mot = iup.GetGlobal("MOTIFVERSION")
  if (mot) then print("  Motif Version: ", mot) end
  print("  Screen Size: " .. iup.GetGlobal("SCREENSIZE"))
  print("  Screen Depth: " .. iup.GetGlobal("SCREENDEPTH"))
  if (iup.GL_VENDOR) then print("  OpenGL Vendor: " .. iup.GL_VENDOR) end
  if (iup.GL_RENDERER) then print("  OpenGL Renderer: " .. iup.GL_RENDERER) end
  if (iup.GL_VERSION) then print("  OpenGL Version: " .. iup.GL_VERSION) end
end

-- IUPLUA Full Application  

lastfile = nil -- Last file open

mulCommands = iupmultiline{expand=IUP_YES, size="200x120", font="COURIER_NORMAL_10"}   
poslabel    = iuplabel{title="0:0", size="50x"} 
filelabel   = iuplabel{title="", size="50x", expand="HORIZONTAL"} 

mulCommands.caretcb = function(self, lin, col)
   poslabel.title = lin..":"..col
end

butExecute = iupbutton{size="50x15", title = "Execute", action="dostring(mulCommands.value)"}
butClearCommands = iupbutton{size="50x15", title = "Clear", action = "mulCommands.value = ''  filelabel.title = ''  lastfile = nil"}
butLoadFile = iupbutton{size="50x15", title = "Load..."}
butSaveasFile = iupbutton{size="50x15", title = "Save As..."}
butSaveFile = iupbutton{size="50x15", title = "Save"}

function butSaveFile:action()
  if (lastfile == nil) then
    butSaveasFile:action() 
  else
    novoarq = openfile (lastfile, "w+")
    if (novoarq ~= nil) then
      write (novoarq,mulCommands.value)
      closefile (novoarq) 
    else
      error ("Cannot Save file "..filename)
    end
  end
end

function butSaveasFile:action()
  local filedlg = iupfiledlg{dialogtype = "SAVE", title = "Save File", filter = "*.wlua", filterinfo = "Lua files",allownew=yes}
  IupPopup(filedlg,IUP_LEFT, IUP_LEFT)
  local status = filedlg.status
  lastfile = filedlg.value
  filelabel.title = lastfile
  IupDestroy(filedlg)
  if status ~= "-1" then 
    if (lastfile == nil) then
      error ("Cannot Save file "..lastfile)
    end
    local novoarq = openfile (lastfile, "w+")
    if (novoarq ~= nil) then
      write (novoarq,mulCommands.value)
      closefile (novoarq)
    else
      error ("Cannot Save file")
    end
  end
end

function butLoadFile:action()
  local filedlg = iupfiledlg{dialogtype="OPEN", title="Load File", filter="*.wlua", filterinfo="Lua Files", allownew="NO"}
  filedlg:popup(IUP_CENTER, IUP_CENTER)
  local status = filedlg.status
  local newfile = filedlg.value
  IupDestroy(filedlg)
  if (status == "-1") or (status == "1") then 
    if (status == "1") then
      error ("Cannot load file "..newfile)
    end
  else
    local fp = openfile (newfile, "r")
    if (fp == nil) then
      error ("Cannot load file "..newfile)
    else
      mulCommands.value = read (fp,"*a") 
      closefile (fp) 
      lastfile = newfile
      filelabel.title = lastfile
    end
  end
end

vbxConsole = iupvbox 
{
  iupframe{iuphbox{iupvbox{butLoadFile, butSaveFile, butSaveasFile, butClearCommands, butExecute; margin="0x0", gap="10"}, iupvbox{filelabel, mulCommands, poslabel; alignment="ARIGHT"}; alignment="ATOP"}; title="Commands"}
  ;alignment="ACENTER", margin="5x5", gap="5" 
}

-- Main Menu Definition.

mnuMain = iupmenu
{
  iupsubmenu
  {
    iupmenu
    {
      iupitem{title="Exit", action="return IUP_CLOSE"}
    }; title="File"
  },
  iupsubmenu{iupmenu
  {
    iup.item{title="Print Version Info...", action=print_version_info},
    iupitem{title="About...", action="dlgAbout:popup(IUP_CENTER, IUP_CENTER)"}
  };title="Help"}
}

-- Main Dialog Definition.

dlgMain = iupdialog{vbxConsole; title="Complete IupLua3 Interpreter", menu=mnuMain, close_cb = "return IUP_CLOSE"}

-- About Dialog Definition.

dlgAbout = iupdialog 
{
  iupvbox
  {
    iuplabel{title="Complete IupLua3 Interpreter"}, 
    iupfill{size="5"},
    iupfill{size="5"},
    iupframe
    {
       iupvbox
       {
          iuplabel{title="Tecgraf/PUC-Rio"},
          iuplabel{title="Mark/Ovídio/Scuri"},
          iuplabel{title="iup@tecgraf.puc-rio.br"} 
       }
    },
    iupfill{size="5"},
    iupbutton{title="OK", action="return IUP_CLOSE", size="50X20"} 
    ;margin="10x10", alignment="ACENTER" 
  }
  ;maxbox=IUP_NO, minbox=IUP_NO, resize=IUP_NO, title="About"
}

-- Displays the Main Dialog 

dlgMain:show()

IupMainLoop()

IupDestroy(dlgMain)
IupDestroy(dlgAbout)
