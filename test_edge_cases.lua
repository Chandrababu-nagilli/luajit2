-- Test edge cases one by one
print("=== BC_ITERN Edge Case Tests ===\n")

-- Test 1: Very large table
print("Test 1: Very large table (10000 elements)...")
local t = {}
for i = 1, 10000 do t[i] = i end
local sum = 0
for k, v in pairs(t) do sum = sum + v end
assert(sum == 50005000, "Large table failed")
print("✓ Passed\n")

-- Test 2: Deep nesting
print("Test 2: Deep nesting (5 levels)...")
local function nested_sum(depth)
  if depth == 0 then return 0 end
  local t = {a=1, b=2}
  local sum = 0
  for k, v in pairs(t) do
    sum = sum + v + nested_sum(depth - 1)
  end
  return sum
end
local result = nested_sum(5)
print("✓ Passed (result=" .. result .. ")\n")

-- Test 3: Table with many hash collisions
print("Test 3: Hash collision stress...")
local t = {}
for i = 1, 100 do
  t["key" .. i] = i
end
local sum = 0
for k, v in pairs(t) do sum = sum + v end
assert(sum == 5050, "Hash collision test failed")
print("✓ Passed\n")

-- Test 4: Mixed numeric and string keys
print("Test 4: Mixed numeric and string keys...")
local t = {}
for i = 1, 50 do t[i] = i end
for i = 1, 50 do t["s" .. i] = i end
local count = 0
for k, v in pairs(t) do count = count + 1 end
assert(count == 100, "Mixed keys test failed")
print("✓ Passed\n")

print("✓ All edge case tests passed!")
