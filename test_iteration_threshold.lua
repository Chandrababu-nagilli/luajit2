-- Find the exact iteration threshold where segfault occurs
print("=== Finding Iteration Threshold ===\n")

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
    print(string.format("✓ %s (%d iterations): PASS", name, count))
    return true
  else
    print(string.format("✗ %s (%d iterations): FAIL (expected %d, got %d)", name, count, expected, sum))
    return false
  end
end

-- Binary search for threshold
test_iterations(100, "Warmup")
test_iterations(500, "Small")
test_iterations(1000, "Medium")
test_iterations(2000, "Large")
test_iterations(3000, "Larger")
test_iterations(4000, "Very Large")
test_iterations(5000, "Huge")
test_iterations(6000, "Massive")
test_iterations(7000, "Enormous")
test_iterations(8000, "Gigantic")
test_iterations(9000, "Colossal")
test_iterations(10000, "Maximum")

print("\n✓ All tests completed")
