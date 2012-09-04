------------------------------------------------------------------------------
-- Template to create control classes for IupLua5
-- The Lua module is used by the "generator.lua" to build a C module,
-- and loaded during iuplua_open to initialize the control.
------------------------------------------------------------------------------
local ctrl = {
  nick = "mycontrol", -- name of the control, used in the control creation: iup.mycontrol{}
                      -- also used for the generated C module
  parent = iup.WIDGET, -- used to define a few methods used fro creation and set attribute
  creation = "nn", -- the creation parameters in Lua
      -- "n"  = int
      -- "d" = double
      -- "s" = char*
      -- "S" = optional char*, can be nil
      -- "i" = Ihandle*
      -- "-" = NULL, no parameters in Lua, but a NULL parameter in C
      -- "a" = char* array in a table
      -- "t" = int array in a table
      -- "v" = Ihandle* array in a table
      
  funcname = "myControl", -- [optional] name of the function used in C  
                          -- default is ctrl.nick with first letter uppercase
                          
  callback = {            -- partial list of callbacks
                          -- only the callbacks that are not already defined by other controls needs to be defined
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
    focus_cb = "n",
    value_cb = {"nn", ret = "s"}, -- ret is return type, default is n ("int")

 -- the following types can be used for callback parameters:    
 -- n = "int",
 -- s = "char *",
 -- i = "Ihandle *",
 -- c = "unsigned char ",
 -- d = "double",
 -- f = "float",
 -- v = "Ihandle **",
 --
 -- Other parameters must be implemented in C using the extrafuncs module
 
 -- IMPORTANT: callbacks with the same name in different controls
 -- are assumed to have the same parameters, that's why they are defined only once
 -- When callbacks conflict using the same name, but different parameters
 -- generator.lua must be edited to include the callback in the list of conflicting callbacks
 -- "action" is a common callback that conflicts
 -- In the callback list, just declare the callback with the parameters used in that control.
  }
  
  include = "iupmycontrol.h", -- [optional] header to be included, it is where the creation function is declared.
  extrafuncs = 1, -- [optional] additional module in C called by the initialization function
  
  createfunc = [[         -- [optional] creation function in C, 
                          -- used if creation parameters needs some interpretation in C
                          -- not to be used together with funcname
#include<stdlib.h>
static int myControl (lua_State * L)
{
  xxxx;
  yyyy;
  return 1;
} 
]]

  extracode = [[        -- [optional] extra fucntions to be defined in C.
]]

}

-- must be defined so the WIDGET constructor can call it
function ctrl.createElement(class, param)  
   return iup.myControl()
end

-- here you can add some custom methods to the class
function ctrl.popup(handle, x, y)
  iup.Popup(handle,x,y)
end

iup.RegisterWidget(ctrl) -- will make iup.mycontrol available
iup.SetClass(ctrl, "iup widget") -- register the class in the registry
