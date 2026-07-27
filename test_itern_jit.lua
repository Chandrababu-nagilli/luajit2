#!/usr/bin/env luajit
-- Test BC_ITERN JIT compilation on s390x

local jit = require("jit")
jit.on()

print("=== BC_ITERN JIT Compilation Test ===")
print("JIT enabled:", jit.status())
print()

-- Test 1: Simple iterator loop (should trigger BC_ITERN)
local function test_simple_iterator()
  local t = {10, 20, 30, 40, 50}
  local sum = 0
  
  -- This should use BC_ITERN when JIT compiled
  for i, v in ipairs(t) do
    sum = sum + v
  end
  
  return sum
end

print("Test 1: Warming up simple iterator...")
local start = os.clock()
for i = 1, 10000 do
  test_simple_iterator()
end
local simple_time = os.clock() - start
print(string.format("Simple iterator: %.6f seconds", simple_time))
print("Result:", test_simple_iterator())
print()

-- Test 2: Generic pairs iterator
local function test_pairs_iterator()
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  
  for k, v in pairs(t) do
    sum = sum + v
  end
  
  return sum
end

print("Test 2: Warming up pairs iterator...")
start = os.clock()
for i = 1, 10000 do
  test_pairs_iterator()
end
local pairs_time = os.clock() - start
print(string.format("Pairs iterator: %.6f seconds", pairs_time))
print("Result:", test_pairs_iterator())
print()

-- Test 3: Array traversal with next()
local function test_next_iterator()
  local t = {100, 200, 300, 400, 500}
  local sum = 0
  
  for k, v in next, t do
    sum = sum + v
  end
  
  return sum
end

print("Test 3: Warming up next iterator...")
start = os.clock()
for i = 1, 10000 do
  test_next_iterator()
end
local next_time = os.clock() - start
print(string.format("Next iterator: %.6f seconds", next_time))
print("Result:", test_next_iterator())
print()

-- Test 4: Mixed array/hash table
local function test_mixed_iterator()
  local t = {10, 20, 30, x=40, y=50, z=60}
  local sum = 0
  
  for k, v in pairs(t) do
    sum = sum + v
  end
  
  return sum
end

print("Test 4: Warming up mixed iterator...")
start = os.clock()
for i = 1, 10000 do
  test_mixed_iterator()
end
local mixed_time = os.clock() - start
print(string.format("Mixed iterator: %.6f seconds", mixed_time))
print("Result:", test_mixed_iterator())
print()

print("=== Performance Summary ===")
print(string.format("Simple iterator: %.6f seconds", simple_time))
print(string.format("Pairs iterator:  %.6f seconds", pairs_time))
print(string.format("Next iterator:   %.6f seconds", next_time))
print(string.format("Mixed iterator:  %.6f seconds", mixed_time))
print()

-- Check if performance is reasonable (JIT compiled iterators should be fast)
local avg_time = (simple_time + pairs_time + next_time + mixed_time) / 4
if avg_time < 0.01 then
  print("✓ Iterator performance is excellent (likely JIT compiled)")
elseif avg_time < 0.05 then
  print("✓ Iterator performance is good (possibly JIT compiled)")
else
  print("⚠ Iterator performance is slow (may be using interpreter)")
end

print()
print("=== Test Complete ===")
