
------------------------------------------------------------------------------
-- Callback handler  
------------------------------------------------------------------------------

iup.callbacks = {}

function iup.CallMethod(name, ...)
  local handle = ... -- the first argument is always the handle
  local func = handle[name]
  if (not func) then
    return
  end
  
  if type(func) == "function" then
    return func(...)
  elseif type(func) == "string" then  
    local temp = self
    self = handle
    local result = iup.dostring(func)
    self = temp
    return result
  else
    return iup.ERROR
  end
end

function iup.RegisterCallback(name, func, type)
  if not iup.callbacks[name] then iup.callbacks[name] = {} end
  local cb = iup.callbacks[name]
  if type then
    cb[type] = func
  else
    cb[1] = func
  end
end

------------------------------------------------------------------------------
-- Meta Methods 
------------------------------------------------------------------------------


local widget_gettable = function(object, index)
  local p = object
  local v
  while 1 do
    v = rawget(p, index)
    if v then return v end
    p = rawget(p, "parent")
      if not p then return nil end
  end
end

iup.NewClass("iup widget")
iup.SetMethod("iup widget", "__index", widget_gettable)


local ihandle_gettable = function(handle, index)
  local INDEX = string.upper(index)
  if (iup.callbacks[INDEX]) then 
   local object = iup.GetWidget(handle)
   if (not object or type(object)~="table") then error("invalid iup handle") end
   return object[index]
  else
    local value = iup.GetAttribute(handle, INDEX)
    if (not value) then
      local object = iup.GetWidget(handle)
      if (not object or type(object)~="table") then error("invalid iup handle") end
      return object[index]
    elseif type(value)== "number" or type(value) == "string" then
      local ih = iup.GetHandle(value)
      if ih then return ih
      else return value end
    else
      return value 
    end
  end
end

local ihandle_settable = function(handle, index, value)
  local ti = type(index)
  local tv = type(value)
  local object = iup.GetWidget(handle)
  if (not object or type(object)~="table") then error("invalid iup handle") end
  if ti == "number" or ti == "string" then -- check if a valid C name
    local INDEX = string.upper(index)
    local cb = iup.callbacks[INDEX]
    if (cb) then -- if a callback name
      local func = cb[1]
      if (not func) then
        func = cb[iup.GetClassName(handle)]
      end
      iup.SetCallback(handle, INDEX, func, value) -- register the pre-defined C callback
      object[index] = value -- store also in Lua
    elseif iup.GetClass(value) == "iup handle" then -- if a iup handle
      local name = iup.SetHandleName(value)
      iup.SetAttribute(handle, INDEX, name)
      object[index] = nil -- if there was something in Lua remove it
    elseif tv == "string" or tv == "number" or tv == "nil" then -- if a common value
      iup.SetAttribute(handle, INDEX, value)
      object[index] = nil -- if there was something in Lua remove it
    else
      object[index] = value -- store also in Lua
    end
  else
    object[index] = value -- store also in Lua
  end
end

iup.NewClass("iup handle")
iup.SetMethod("iup handle", "__index", ihandle_gettable)
iup.SetMethod("iup handle", "__newindex", ihandle_settable)
iup.SetMethod("iup handle", "__tostring", iup.ihandle_tostring) -- implemented in C
iup.SetMethod("iup handle", "__eq", iup.ihandle_compare) -- implemented in C


------------------------------------------------------------------------------
-- Utilities 
------------------------------------------------------------------------------

function iup.SetHandleName(v)  -- used also by radio and zbox
  local name = iup.GetName(v)
  if not name then
    local autoname = string.format("_IUPLUA_NAME(%s)", tostring(v))
    iup.SetHandle(autoname, v)
    return autoname
  end
  return name
end

function iup.RegisterWidget(ctrl) -- called by all the controls initialization functions
  iup[ctrl.nick] = function(param)
    if (not ctrl.constructor) then print(ctrl.nick) end
    return ctrl:constructor(param)
  end
end

function iup.RegisterHandle(handle, typename)

  iup.SetClass(handle, "iup handle")
  
  local object = iup.GetWidget(handle)
  if not object then

    local class = iup[string.upper(typename)]
    if not class then
      class = WIDGET
    end

    local object = { parent=class, handle=handle }
    iup.SetClass(object, "iup widget")
    iup.SetWidget(handle, object)
  end
  
  return handle
end

------------------------------------------------------------------------------
-- Widget class (top class) 
------------------------------------------------------------------------------

iup.WIDGET = {
  callback = {}
}

function iup.WIDGET.show(object)
  iup.Show(object.handle)
end

function iup.WIDGET.hide(object)
  iup.Hide(object.handle)
end

function iup.WIDGET.map(object)
  iup.Map(object.handle)
end

function iup.WIDGET.unmap(object)
  iup.Unmap(object.handle)
end

function iup.WIDGET.destroy(object)
  iup.Destroy(object.handle)
end

function iup.WIDGET.constructor(class, param)
  local handle = class:createElement(param)
  local object = { 
    parent = class,
    handle = handle
  }
  iup.SetClass(handle, "iup handle")
  iup.SetClass(object, "iup widget")
  iup.SetWidget(handle, object)
  object:setAttributes(param)
  return handle
end

function iup.WIDGET.setAttributes(object, param)
  local handle = object.handle
  for i,v in pairs(param) do 
    if type(i) == "number" and iup.GetClass(v) == "iup handle" then
      -- We should not set this or other elements (such as iuptext)
      -- will erroneosly inherit it
      rawset(object, i, v)
    else
      -- this will call settable metamethod
      handle[i] = v
    end
  end
end

-- all the objects in the hierarchy must be "iup widget"
-- Must repeat this call for every new widget
iup.SetClass(iup.WIDGET, "iup widget")


------------------------------------------------------------------------------
-- Box class (inherits from WIDGET) 
------------------------------------------------------------------------------

iup.BOX = {
  parent = iup.WIDGET
}

function iup.BOX.setAttributes(object, param)
  local handle = rawget(object, "handle")
  local n = #param
  for i = 1, n do
    if iup.GetClass(param[i]) == "iup handle" then 
      iup.Append(handle, param[i]) 
    end
  end
  iup.WIDGET.setAttributes(object, param)
end

iup.SetClass(iup.BOX, "iup widget")


------------------------------------------------------------------------------
-- Compatibility functions.
------------------------------------------------------------------------------

iup.error_message_popup = nil

function iup._ERRORMESSAGE(msg,traceback)
  msg = msg..(traceback or "")
  if (iup.error_message_popup) then
    iup.error_message_popup.value = msg
  else  
    local bt = iup.button{title="Ok", size="60", action="iup.error_message_popup = nil; return iup.CLOSE"}
    local ml = iup.multiline{expand="YES", readonly="YES", value=msg, size="300x150"}
    local vb = iup.vbox{ml, bt; alignment="ACENTER", margin="10x10", gap="10"}
    local dg = iup.dialog{vb; title="Error Message",defaultesc=bt,defaultenter=bt,startfocus=bt}
    iup.error_message_popup = ml
    dg:popup(CENTER, CENTER)
    dg:destroy()
    iup.error_message_popup = nil
  end
end

iup.pack = function (...) return {...} end

function iup.protectedcall(f, msg)
  if not f then 
    iup._ERRORMESSAGE(msg)
    return 
  end
  local ret = iup.pack(pcall(f))
  if not ret[1] then 
    iup._ERRORMESSAGE(ret[2])
    return
  else  
    table.remove(ret, 1)
    return unpack(ret)   --must replace this by table.unpack when 5.1 is not supported
  end
end

function iup.dostring(s) return iup.protectedcall(loadstring(s)) end
function iup.dofile(f) return iup.protectedcall(loadfile(f)) end

function iup.RGB(r, g, b)
  return string.format("%d %d %d", 255*r, 255*g, 255*b)
end

-- This will allow both names to be used in the same application
-- also will allow static linking to work with require for the main library (only)
if _G.package then
   _G.package.loaded["iuplua"] = iup
   iup._M = iup
   iup._PACKAGE = "iuplua"
end

function iup.layoutdialog(obj)
  return iup.LayoutDialog(obj[1])
end
