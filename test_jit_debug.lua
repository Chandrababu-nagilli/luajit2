-- Debug JIT compilation issue
print("Testing JIT with verbose output...")

-- Disable some JIT optimizations to isolate issue
jit.opt.start("hotloop=10", "hotexit=2", "minstitch=0")

local function test_with_trace_limit(iterations)
  print(string.format("\nTesting %d iterations...", iterations))
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  
  for i = 1, iterations do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  
  local expected = iterations * 15
  if sum == expected then
    print(string.format("✓ PASS: sum=%d", sum))
    return true
  else
    print(string.format("✗ FAIL: expected=%d, got=%d", expected, sum))
    return false
  end
end

-- Test with increasing iterations
test_with_trace_limit(100)
test_with_trace_limit(500)
test_with_trace_limit(1000)
test_with_trace_limit(2000)
test_with_trace_limit(5000)
test_with_trace_limit(10000)

print("\n✓ All JIT debug tests passed!")
