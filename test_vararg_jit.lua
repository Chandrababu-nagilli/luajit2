#!/usr/bin/env luajit
-- Test if vararg functions can be JIT compiled after our changes

local jit = require("jit")
jit.on()

print("=== Vararg JIT Compilation Test ===")
print("JIT enabled:", jit.status())
print()

-- Test 1: Simple vararg function
local function vararg_sum(...)
  local sum = 0
  for i = 1, select('#', ...) do
    sum = sum + select(i, ...)
  end
  return sum
end

print("Test 1: Warming up vararg function...")
local start = os.clock()
for i = 1, 10000 do
  vararg_sum(1, 2, 3, 4, 5)
end
local vararg_time = os.clock() - start
print(string.format("Vararg function: %.6f seconds", vararg_time))
print("Result:", vararg_sum(10, 20, 30, 40, 50))
print()

-- Test 2: Regular function for comparison
local function regular_sum(a, b, c, d, e)
  return a + b + c + d + e
end

print("Test 2: Warming up regular function...")
start = os.clock()
for i = 1, 10000 do
  regular_sum(1, 2, 3, 4, 5)
end
local regular_time = os.clock() - start
print(string.format("Regular function: %.6f seconds", regular_time))
print("Result:", regular_sum(10, 20, 30, 40, 50))
print()

-- Test 3: Check trace status
print("Test 3: Checking JIT trace status...")
local status, err = pcall(function()
  -- Try to get trace info if available
  local jutil = require("jit.util")
  print("JIT util available")
end)

if not status then
  print("JIT util not available (expected on some builds)")
end

print()
print("=== Performance Comparison ===")
if vararg_time > 0 and regular_time > 0 then
  local ratio = vararg_time / regular_time
  print(string.format("Vararg/Regular ratio: %.2fx", ratio))
  if ratio < 2.0 then
    print("✓ Vararg functions appear to be JIT compiled (similar performance)")
  else
    print("✗ Vararg functions likely using interpreter (much slower)")
  end
else
  print("⚠ Could not measure performance accurately")
end

print()
print("=== Test Complete ===")
