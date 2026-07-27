#!/usr/bin/env luajit
-- Comprehensive test suite for s390x JIT implementation
-- Tests all NYI items and edge cases

local jit = require("jit")
local ffi = require("ffi")

print("=== LuaJIT s390x NYI Test Suite ===")
print("Version:", jit.version)
print("Architecture:", jit.arch)
print("JIT enabled:", jit.status())
print()

local tests_passed = 0
local tests_failed = 0

local function test(name, fn)
  io.write(string.format("%-50s ... ", name))
  local ok, err = pcall(fn)
  if ok then
    print("PASS")
    tests_passed = tests_passed + 1
  else
    print("FAIL:", err)
    tests_failed = tests_failed + 1
  end
end

-- Test 1: Vararg functions (BC_FUNCV/BC_JFUNCV)
test("Vararg function basic", function()
  local function vararg_sum(...)
    local sum = 0
    for i = 1, select('#', ...) do
      sum = sum + select(i, ...)
    end
    return sum
  end
  
  -- Warm up (but vararg functions don't get hotcall counting)
  for i = 1, 100 do
    vararg_sum(1, 2, 3, 4, 5)
  end
  
  local result = vararg_sum(10, 20, 30, 40, 50)
  assert(result == 150, "Expected 150, got " .. result)
end)

-- Test 2: Vararg with mixed types
test("Vararg mixed types", function()
  local function vararg_concat(...)
    local result = ""
    for i = 1, select('#', ...) do
      result = result .. tostring(select(i, ...))
    end
    return result
  end
  
  for i = 1, 100 do
    vararg_concat("a", 1, "b", 2)
  end
  
  local result = vararg_concat("test", 123, "end")
  assert(result == "test123end", "Expected 'test123end', got '" .. result .. "'")
end)

-- Test 3: Iterator loops (BC_ITERN)
test("Iterator loop basic", function()
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  
  -- Warm up to trigger potential hotloop
  for i = 1, 100 do
    local s = 0
    for k, v in pairs(t) do
      s = s + v
    end
  end
  
  for k, v in pairs(t) do
    sum = sum + v
  end
  
  assert(sum == 15, "Expected 15, got " .. sum)
end)

-- Test 4: Iterator with array part
test("Iterator array traversal", function()
  local t = {10, 20, 30, 40, 50}
  local sum = 0
  
  for i = 1, 100 do
    local s = 0
    for k, v in pairs(t) do
      s = s + v
    end
  end
  
  for k, v in pairs(t) do
    sum = sum + v
  end
  
  assert(sum == 150, "Expected 150, got " .. sum)
end)

-- Test 5: MOD operation with integers
test("MOD integer operation", function()
  local function mod_test(a, b)
    return a % b
  end
  
  -- Warm up
  for i = 1, 100 do
    mod_test(17, 5)
  end
  
  local result = mod_test(17, 5)
  assert(result == 2, "Expected 2, got " .. result)
end)

-- Test 6: MOD with negative numbers (Lua semantics)
test("MOD negative numbers", function()
  local function mod_test(a, b)
    return a % b
  end
  
  for i = 1, 100 do
    mod_test(-17, 5)
  end
  
  local result = mod_test(-17, 5)
  assert(result == 3, "Expected 3, got " .. result)  -- Lua: -17 % 5 = 3
end)

-- Test 7: MOD with floats
test("MOD float operation", function()
  local function mod_test(a, b)
    return a % b
  end
  
  for i = 1, 100 do
    mod_test(17.5, 5.2)
  end
  
  local result = mod_test(17.5, 5.2)
  -- Lua modulo: 17.5 - floor(17.5/5.2)*5.2 = 17.5 - 3*5.2 = 17.5 - 15.6 = 1.9
  assert(math.abs(result - 1.9) < 0.001, "Expected ~1.9, got " .. result)
end)

-- Test 8: Complex nested loops with iterators
test("Nested iterator loops", function()
  local t1 = {a=1, b=2}
  local t2 = {x=10, y=20}
  local sum = 0
  
  for i = 1, 50 do
    local s = 0
    for k1, v1 in pairs(t1) do
      for k2, v2 in pairs(t2) do
        s = s + v1 * v2
      end
    end
  end
  
  for k1, v1 in pairs(t1) do
    for k2, v2 in pairs(t2) do
      sum = sum + v1 * v2
    end
  end
  
  assert(sum == 90, "Expected 90, got " .. sum)  -- (1+2)*(10+20) = 90
end)

-- Test 9: Vararg function returning multiple values
test("Vararg multiple returns", function()
  local function vararg_multi(...)
    return select('#', ...), select(1, ...), select(select('#', ...), ...)
  end
  
  for i = 1, 100 do
    vararg_multi(1, 2, 3, 4, 5)
  end
  
  local count, first, last = vararg_multi(10, 20, 30, 40)
  assert(count == 4 and first == 10 and last == 40, 
         string.format("Expected 4,10,40 got %s,%s,%s", count, first, last))
end)

-- Test 10: MOD in loop (stress test)
test("MOD in hot loop", function()
  local sum = 0
  for i = 1, 1000 do
    sum = sum + (i % 7)
  end
  
  -- Expected: sum of (i % 7) for i=1..1000
  -- Pattern repeats every 7: 1,2,3,4,5,6,0
  -- 1000/7 = 142 complete cycles + 6 remainder
  -- Sum per cycle = 1+2+3+4+5+6+0 = 21
  -- Total = 142*21 + (1+2+3+4+5+6) = 2982 + 21 = 3003
  assert(sum == 3003, "Expected 3003, got " .. sum)
end)

-- Test 11: Iterator with modifications (edge case)
test("Iterator stability", function()
  local t = {a=1, b=2, c=3}
  local keys = {}
  
  for i = 1, 100 do
    local k_list = {}
    for k, v in pairs(t) do
      table.insert(k_list, k)
    end
  end
  
  for k, v in pairs(t) do
    table.insert(keys, k)
  end
  
  assert(#keys == 3, "Expected 3 keys, got " .. #keys)
end)

-- Test 12: Vararg with tail call
test("Vararg tail call", function()
  local function inner(...)
    local sum = 0
    for i = 1, select('#', ...) do
      sum = sum + select(i, ...)
    end
    return sum
  end
  
  local function outer(...)
    return inner(...)
  end
  
  for i = 1, 100 do
    outer(1, 2, 3)
  end
  
  local result = outer(5, 10, 15, 20)
  assert(result == 50, "Expected 50, got " .. result)
end)

-- Summary
print()
print("=== Test Results ===")
print(string.format("Passed: %d", tests_passed))
print(string.format("Failed: %d", tests_failed))
print(string.format("Total:  %d", tests_passed + tests_failed))
print()

if tests_failed == 0 then
  print("✓ All tests passed!")
  os.exit(0)
else
  print("✗ Some tests failed")
  os.exit(1)
end
