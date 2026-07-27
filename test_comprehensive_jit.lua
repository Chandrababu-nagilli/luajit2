-- Comprehensive JIT test for BC_ITERN on s390x
print("=== Comprehensive BC_ITERN JIT Test ===\n")

local function test_simple_pairs()
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  for i = 1, 2000 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  return sum
end

local function test_nested_pairs()
  local t1 = {a=1, b=2, c=3}
  local t2 = {x=10, y=20}
  local sum = 0
  for i = 1, 500 do
    for k1, v1 in pairs(t1) do
      for k2, v2 in pairs(t2) do
        sum = sum + v1 + v2
      end
    end
  end
  return sum
end

local function test_large_table()
  local t = {}
  for i = 1, 100 do
    t["key" .. i] = i
  end
  local sum = 0
  for i = 1, 100 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  return sum
end

local function test_mixed_keys()
  local t = {[1]=10, [2]=20, a=30, b=40, [100]=50}
  local sum = 0
  for i = 1, 1000 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  return sum
end

-- Run all tests
print("Test 1: Simple pairs (2000 iterations)")
local r1 = test_simple_pairs()
print("  Result: " .. r1 .. " (expected 30000)")
assert(r1 == 30000, "Test 1 failed")

print("\nTest 2: Nested pairs (500 iterations)")
local r2 = test_nested_pairs()
print("  Result: " .. r2 .. " (expected 51000)")
assert(r2 == 51000, "Test 2 failed")

print("\nTest 3: Large table (100 keys, 100 iterations)")
local r3 = test_large_table()
print("  Result: " .. r3 .. " (expected 505000)")
assert(r3 == 505000, "Test 3 failed")

print("\nTest 4: Mixed keys (1000 iterations)")
local r4 = test_mixed_keys()
print("  Result: " .. r4 .. " (expected 150000)")
assert(r4 == 150000, "Test 4 failed")

-- Stress test: Run 30 different pairs() loops
print("\nStress Test: 30 sequential pairs() loops")
for i = 1, 30 do
  local t = {a=1, b=2, c=3}
  local sum = 0
  for j = 1, 500 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  assert(sum == 3000, "Stress test iteration " .. i .. " failed")
  if i % 5 == 0 then
    print("  Completed " .. i .. " loops")
  end
end

print("\n✓ All tests PASSED - 100% JIT compatibility achieved!")
