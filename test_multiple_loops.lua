-- Test running multiple pairs() loops in sequence
print("=== Multiple Loops Test ===\n")

local function run_test(num)
  print(string.format("Test %d: Running pairs() loop...", num))
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  
  for i = 1, 1000 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  
  print(string.format("  Result: %d (expected 15000)", sum))
  assert(sum == 15000, "Sum mismatch")
end

-- Run multiple tests in sequence
for i = 1, 15 do
  run_test(i)
  collectgarbage("collect")  -- Try to clean up between tests
end

print("\n✓ All 15 tests completed successfully")
