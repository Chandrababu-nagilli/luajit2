-- Minimal test case to trigger the issue
print("=== Trace Analysis Test ===")

-- Enable JIT dump to see what's being compiled
jit.opt.start("hotloop=2", "hotexit=1")

local function simple_pairs()
  local t = {a=1, b=2, c=3}
  local sum = 0
  
  -- This should trigger trace compilation after 2 iterations
  for i = 1, 5 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  
  return sum
end

print("Running simple_pairs...")
local result = simple_pairs()
print("Result: " .. result)
print("Expected: 30")

-- Now try nested case that causes segfault
local function nested_pairs()
  local t1 = {a=1, b=2}
  local t2 = {x=10, y=20}
  local sum = 0
  
  for i = 1, 5 do
    for k1, v1 in pairs(t1) do
      for k2, v2 in pairs(t2) do
        sum = sum + v1 + v2
      end
    end
  end
  
  return sum
end

print("\nRunning nested_pairs...")
local result2 = nested_pairs()
print("Result: " .. result2)
print("Expected: 330")

print("\n✓ Test completed successfully")
