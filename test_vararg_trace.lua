#!/usr/bin/env luajit
-- Test vararg JIT with trace dump to see what's happening

local jit = require("jit")
jit.on()

-- Enable verbose trace output
jit.opt.start("hotloop=10", "hotexit=2")

print("=== Vararg Trace Test ===")
print()

-- Simple vararg function
local function vararg_sum(...)
  local sum = 0
  local n = select('#', ...)
  for i = 1, n do
    sum = sum + select(i, ...)
  end
  return sum
end

print("Calling vararg function 100 times to trigger hotcall...")
for i = 1, 100 do
  local result = vararg_sum(1, 2, 3, 4, 5)
  if i == 1 then
    print("First call result:", result)
  end
end

print("Final call result:", vararg_sum(10, 20, 30, 40, 50))
print()

-- Check if any traces were compiled
local jutil = require("jit.util")
local info = jit.util.funcinfo
local bc = jit.util.funcbc

print("Function info:")
local fi = info(vararg_sum)
if fi then
  print("  Name:", fi.name or "anonymous")
  print("  Source:", fi.source)
  print("  Line defined:", fi.linedefined)
  print("  Params:", fi.params)
  print("  Is vararg:", fi.isvararg)
  print("  Bytecodes:", fi.bytecodes)
end

print()
print("=== Test Complete ===")
