------------------------------------------------------------------------------
-- Tree class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "tree",
  parent = iup.WIDGET,
  creation = "",
  callback = {
    selection_cb = "nn",
    multiselection_cb = "nn",  -- fake definition to be replaced by treefuncs module
    multiunselection_cb = "nn",  -- fake definition to be replaced by treefuncs module
    branchopen_cb = "n",
    branchclose_cb = "n",
    executeleaf_cb = "n",
    noderemoved_cb = "s",  -- fake definition to be replaced by treefuncs module
    rename_cb = "ns",
    showrename_cb = "n",
    rightclick_cb = "n",
    dragdrop_cb = "nnnn",
  },
  extrafuncs = 1,
}

iup.TREEREFTABLE={}

function iup.TreeSetNodeAttributes(handle, id, attrs)
  for attr, val in pairs(attrs) do
    handle[attr..id] = val
  end
end

function iup.TreeSetAncestorsAttributes(handle, ini, attrs)
  ini = handle["parent"..ini]
  local stack = {}
    while ini do
      table.insert(stack, 1, ini)
      ini = handle["parent"..ini]
    end
  for i = 1, #stack do
    iup.TreeSetNodeAttributes(handle, stack[i], attrs)
  end
end

function iup.TreeSetDescentsAttributes(handle, ini, attrs)
  local id = ini
  for i = 1, handle["childcount"..ini] do
    id = id+1
    iup.TreeSetNodeAttributes(handle, id, attrs)
    if handle["kind"..id] == "BRANCH" then
      id = iup.TreeSetDescentsAttributes(handle, id, attrs)
    end
  end
  return id
end

function iup.TreeSetAttributeHandle(handle, name, value)
   if iup.GetClass(value) == "iup handle" then value = iup.SetHandleName(value) end
   iup.SetAttribute(handle, name, value)
end

-- must be after the branch has nodes
function iup.TreeSetState(handle, tnode, id)
  if tnode.state then iup.SetAttribute(handle, "STATE"..id, tnode.state) end
end

function iup.TreeSetNodeAttrib(handle, tnode, id)
  if tnode.color then iup.SetAttribute(handle, "COLOR"..id, tnode.color) end
  if tnode.titlefont then iup.SetAttribute(handle, "TITLEFONT"..id, tnode.titlefont) end
  if tnode.marked then iup.SetAttribute(handle, "MARKED"..id, tnode.marked) end
  if tnode.image then iup.TreeSetAttributeHandle(handle, "IMAGE"..id, tnode.image) end
  if tnode.imageexpanded then iup.TreeSetAttributeHandle(handle, "IMAGEEXPANDED"..id, tnode.imageexpanded) end
  if tnode.userid then iup.TreeSetUserId(handle, id, tnode.userid) end
end

function iup.TreeAddNodesRec(handle, t, id)
  if t == nil then return end
  local cont = #t
  while cont >= 0 do
    local tnode = t[cont]
    if type(tnode) == "table" then
      if tnode.branchname then
        iup.SetAttribute(handle, "ADDBRANCH"..id, tnode.branchname)
        iup.TreeSetNodeAttrib(handle, tnode, id+1)
        iup.TreeAddNodesRec(handle, tnode, id+1)
        iup.TreeSetState(handle, tnode, id+1)
      elseif tnode.leafname then
        iup.SetAttribute(handle, "ADDLEAF"..id, tnode.leafname)
        iup.TreeSetNodeAttrib(handle, tnode, id+1)
      end
    else
      if tnode then
        iup.SetAttribute(handle, "ADDLEAF"..id, tnode)
      end
    end
    cont = cont - 1
   end
end

function iup.TreeAddNodes(handle, t, id)
  if (not id) then
    id = 0  -- default is the root
    if t.branchname then iup.SetAttribute(handle, "TITLE0", t.branchname) end
    iup.TreeSetNodeAttrib(handle, t, 0)
  end
  iup.TreeAddNodesRec(handle, t, id)
  if (id == 0) then iup.TreeSetState(handle, t, 0) end
end

-- backward compatibility
iup.TreeSetValue = iup.TreeAddNodes

function ctrl.createElement(class, param)
  return iup.Tree()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iup widget")
