-- Test if flushing JIT state between loops prevents segfault
print("=== JIT Flush Test ===\n")

local function run_test(num)
  print(string.format("Test %d: Running pairs() loop...", num))
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  
  for i = 1, 1000 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  
  print(string.format("  Result: %d", sum))
  assert(sum == 15000, "Sum mismatch")
  
  -- Flush JIT state after each test
  jit.flush()
end

-- Run multiple tests with JIT flush between them
for i = 1, 15 do
  run_test(i)
  collectgarbage("collect")
end

print("\n✓ All 15 tests completed successfully with JIT flush")
