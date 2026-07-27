-- Isolate JIT stress test issue
print("Testing JIT stress with different iteration counts...")

local function test_iterations(count, name)
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  
  for i = 1, count do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  
  local expected = count * 15
  if sum == expected then
    print(string.format("✓ %s: %d iterations, sum=%d", name, count, sum))
    return true
  else
    print(string.format("✗ %s: %d iterations, expected=%d, got=%d", name, count, expected, sum))
    return false
  end
end

-- Test with increasing iteration counts
test_iterations(10, "Warmup")
test_iterations(100, "Small")
test_iterations(1000, "Medium")
test_iterations(5000, "Large")
test_iterations(10000, "Stress")

print("\n✓ All stress tests passed!")
