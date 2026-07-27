-- Bisect to find which test causes segfault
local function test(name, fn)
  print("Running: " .. name)
  local ok, err = pcall(fn)
  if ok then
    print("  ✓ PASS")
    return true
  else
    print("  ✗ FAIL: " .. tostring(err))
    return false
  end
end

-- Run tests 1-10 (known to pass)
test("Test 1-10", function()
  -- Empty table
  local t = {}
  for k, v in pairs(t) do end
  
  -- Array-only
  local t = {1, 2, 3, 4, 5}
  local sum = 0
  for k, v in pairs(t) do sum = sum + v end
  
  -- Hash-only
  local t = {a=1, b=2, c=3}
  sum = 0
  for k, v in pairs(t) do sum = sum + v end
  
  -- Mixed
  local t = {10, 20, 30, x=100, y=200}
  sum = 0
  for k, v in pairs(t) do sum = sum + v end
  
  -- Large array
  local t = {}
  for i = 1, 1000 do t[i] = i end
  sum = 0
  for k, v in pairs(t) do sum = sum + v end
  
  -- Sparse
  local t = {[1]=1, [10]=10, [100]=100, [1000]=1000}
  sum = 0
  for k, v in pairs(t) do sum = sum + v end
  
  -- String keys
  local t = {foo="bar", hello="world", test="value"}
  local count = 0
  for k, v in pairs(t) do count = count + 1 end
  
  -- Nested
  local t1 = {a=1, b=2}
  local t2 = {x=10, y=20}
  sum = 0
  for k1, v1 in pairs(t1) do
    for k2, v2 in pairs(t2) do
      sum = sum + v1 + v2
    end
  end
  
  -- Break
  local t = {1, 2, 3, 4, 5}
  sum = 0
  for k, v in pairs(t) do
    sum = sum + v
    if v == 3 then break end
  end
  
  -- ipairs vs pairs
  local t = {10, 20, 30}
  local sum1, sum2 = 0, 0
  for k, v in ipairs(t) do sum1 = sum1 + v end
  for k, v in pairs(t) do sum2 = sum2 + v end
end)

-- Test 11 separately (JIT stress)
test("Test 11: JIT stress", function()
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  for i = 1, 10000 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  assert(sum == 150000)
end)

-- Test 12 separately
test("Test 12: Nil values", function()
  local t = {a=1, b=nil, c=3}
  local count = 0
  for k, v in pairs(t) do count = count + 1 end
  assert(count == 2)
end)

print("\n✓ All tests completed")
