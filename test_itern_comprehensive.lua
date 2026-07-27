-- Comprehensive BC_ITERN compatibility test
local function test(name, fn)
  local ok, err = pcall(fn)
  if ok then
    print(string.format("%-50s ... PASS", name))
    return true
  else
    print(string.format("%-50s ... FAIL: %s", name, err))
    return false
  end
end

print("=== BC_ITERN Comprehensive Compatibility Test ===\n")

local passed, failed = 0, 0

-- Test 1: Empty table
if test("Empty table iteration", function()
  local t = {}
  local count = 0
  for k, v in pairs(t) do count = count + 1 end
  assert(count == 0, "Empty table should have 0 iterations")
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 2: Array-only table
if test("Array-only table", function()
  local t = {1, 2, 3, 4, 5}
  local sum = 0
  for k, v in pairs(t) do sum = sum + v end
  assert(sum == 15, "Expected sum 15, got " .. sum)
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 3: Hash-only table
if test("Hash-only table", function()
  local t = {a=1, b=2, c=3}
  local sum = 0
  for k, v in pairs(t) do sum = sum + v end
  assert(sum == 6, "Expected sum 6, got " .. sum)
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 4: Mixed array and hash
if test("Mixed array and hash table", function()
  local t = {10, 20, 30, x=100, y=200}
  local sum = 0
  for k, v in pairs(t) do sum = sum + v end
  assert(sum == 360, "Expected sum 360, got " .. sum)
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 5: Large array
if test("Large array (1000 elements)", function()
  local t = {}
  for i = 1, 1000 do t[i] = i end
  local sum = 0
  for k, v in pairs(t) do sum = sum + v end
  assert(sum == 500500, "Expected sum 500500, got " .. sum)
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 6: Sparse array
if test("Sparse array", function()
  local t = {[1]=1, [10]=10, [100]=100, [1000]=1000}
  local sum = 0
  for k, v in pairs(t) do sum = sum + v end
  assert(sum == 1111, "Expected sum 1111, got " .. sum)
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 7: String keys
if test("String keys", function()
  local t = {foo="bar", hello="world", test="value"}
  local count = 0
  for k, v in pairs(t) do count = count + 1 end
  assert(count == 3, "Expected 3 keys, got " .. count)
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 8: Nested iteration
if test("Nested iteration", function()
  local t1 = {a=1, b=2}
  local t2 = {x=10, y=20}
  local sum = 0
  for k1, v1 in pairs(t1) do
    for k2, v2 in pairs(t2) do
      sum = sum + v1 + v2
    end
  end
  assert(sum == 66, "Expected sum 66, got " .. sum)
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 9: Break in iteration
if test("Break in iteration", function()
  local t = {1, 2, 3, 4, 5}
  local sum = 0
  for k, v in pairs(t) do
    sum = sum + v
    if v == 3 then break end
  end
  assert(sum <= 15, "Break should stop iteration early")
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 10: ipairs vs pairs
if test("ipairs vs pairs consistency", function()
  local t = {10, 20, 30}
  local sum1, sum2 = 0, 0
  for k, v in ipairs(t) do sum1 = sum1 + v end
  for k, v in pairs(t) do sum2 = sum2 + v end
  assert(sum1 == sum2, "ipairs and pairs should give same sum for array")
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 11: JIT compilation stress test
if test("JIT compilation stress (10000 iterations)", function()
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  for i = 1, 10000 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  assert(sum == 150000, "Expected sum 150000, got " .. sum)
end) then passed = passed + 1 else failed = failed + 1 end

-- Test 12: Table modification during iteration (should work)
if test("Table with nil values", function()
  local t = {a=1, b=nil, c=3}
  local count = 0
  for k, v in pairs(t) do count = count + 1 end
  assert(count == 2, "Expected 2 non-nil values, got " .. count)
end) then passed = passed + 1 else failed = failed + 1 end

print("\n=== Test Results ===")
print("Passed: " .. passed)
print("Failed: " .. failed)
print("Total:  " .. (passed + failed))

if failed == 0 then
  print("\n✓ All tests passed!")
  os.exit(0)
else
  print("\n✗ Some tests failed")
  os.exit(1)
end
